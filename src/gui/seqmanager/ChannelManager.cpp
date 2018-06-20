/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2018 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#define RG_MODULE_STRING "[ChannelManager]"
#define RG_NO_DEBUG_PRINT 1

#include "ChannelManager.h"

#include "base/AllocateChannels.h"
#include "base/Instrument.h"
#include "misc/Debug.h"
#include "misc/ConfigGroups.h"
#include "sound/MappedEvent.h"
#include "sound/MappedInserterBase.h"
#include "sound/Midi.h"

#include <QSettings>

namespace Rosegarden
{


ChannelManager::ChannelManager(Instrument *instrument) :
    m_instrument(0),
    m_start(),
    m_end(),
    m_startMargin(),
    m_endMargin(),
    m_channelInterval(),
    m_usingAllocator(false),
    m_triedToGetChannel(false),
    m_ready(false)
{
    // Safe even for NULL.
    connectInstrument(instrument);
}

void
ChannelManager::connectInstrument(Instrument *instrument)
{
    if (!instrument)
        return;

    // Disconnect the old instrument, if any.
    if (m_instrument)
        disconnect(m_instrument);

    // Connect to the new instrument
    connect(instrument, SIGNAL(wholeDeviceDestroyed()),
            this, SLOT(slotLosingDevice()));
    connect(instrument, SIGNAL(destroyed()),
            this, SLOT(slotLosingInstrument()));
    connect(instrument, SIGNAL(changedChannelSetup()),
            this, SLOT(slotInstrumentChanged()));
    connect(instrument, SIGNAL(channelBecomesFixed()),
            this, SLOT(slotChannelBecomesFixed()));
    connect(instrument, SIGNAL(channelBecomesUnfixed()),
            this, SLOT(slotChannelBecomesUnfixed()));

    setAllocationMode(instrument);
    m_instrument = instrument;
    slotInstrumentChanged();
}

void ChannelManager::insertController(
        TrackId trackId,
        const Instrument *instrument,
        ChannelId channel,
        RealTime insertTime,
        MidiByte controller,
        MidiByte value,
        MappedInserterBase &inserter)
{
    MappedEvent mE(instrument->getId(),
                   MappedEvent::MidiController,
                   controller,
                   value);
    mE.setRecordedChannel(channel);
    mE.setEventTime(insertTime);
    mE.setTrackId(trackId);
    inserter.insertCopy(mE);
}

void ChannelManager::insertChannelSetup(
        TrackId trackId,
        const Instrument *instrument,
        ChannelId channel,
        RealTime insertTime,
        const ControllerAndPBList &controllerAndPBList,
        MappedInserterBase &inserter)
{
    // Bank Select

    if (!instrument->hasFixedChannel()  ||
        instrument->sendsBankSelect()) {
        {
            // Bank Select MSB
            MappedEvent mE(instrument->getId(),
                           MappedEvent::MidiController,
                           MIDI_CONTROLLER_BANK_MSB,
                           instrument->getMSB());
            mE.setRecordedChannel(channel);
            mE.setEventTime(insertTime);
            mE.setTrackId(trackId);
            inserter.insertCopy(mE);
        }
        {
            // Bank Select LSB
            MappedEvent mE(instrument->getId(),
                           MappedEvent::MidiController,
                           MIDI_CONTROLLER_BANK_LSB,
                           instrument->getLSB());
            mE.setRecordedChannel(channel);
            mE.setEventTime(insertTime);
            mE.setTrackId(trackId);
            inserter.insertCopy(mE);
        }
    }

    // Program Change

    if (!instrument->hasFixedChannel()  ||
        instrument->sendsProgramChange()) {
        // Program Change
        MappedEvent mE(instrument->getId(),
                       MappedEvent::MidiProgramChange,
                       instrument->getProgramChange());
        mE.setRecordedChannel(channel);
        mE.setEventTime(insertTime);
        mE.setTrackId(trackId);
        inserter.insertCopy(mE);
    }

    // Reset All Controllers

    // This is still desirable for some users.
    QSettings settings;
    settings.beginGroup(SequencerOptionsConfigGroup);
    const bool allowReset =
            settings.value("allowresetallcontrollers", "true").toBool();
    settings.endGroup();

    if (allowReset) {
        // In case some controllers are on that we don't know about, turn
        // all controllers off.  (Reset All Controllers)
        try {
            MappedEvent mE(instrument->getId(),
                           MappedEvent::MidiController,
                           MIDI_CONTROLLER_RESET,
                           0);
            mE.setRecordedChannel(channel);
            mE.setEventTime(insertTime);
            mE.setTrackId(trackId);
            inserter.insertCopy(mE);
        } catch (...) {
            // Ignore.
        }
    }

    // Control Changes

    const StaticControllers &list = controllerAndPBList.m_controllers;

    // For each controller
    for (StaticControllerConstIterator cIt = list.begin();
         cIt != list.end(); ++cIt) {
        const MidiByte controlId = cIt->first;
        const MidiByte controlValue = cIt->second;

        //RG_DEBUG << "insertChannelSetup() : inserting controller " << (int)controlId << "value" << (int)controlValue << "on channel" << (int)channel << "for time" << reftime;

        try {
            // Put it in the inserter.
            insertController(trackId, instrument, channel, insertTime,
                             controlId, controlValue, inserter);
        } catch (...) {
            // Ignore.
        }
    }

    // Pitch Bend

    // If there's a pitch bend, insert it...
    // We only do one type of pitchbend, though GM2 allows others.
    if (controllerAndPBList.m_havePitchbend) {
        const int raised = controllerAndPBList.m_pitchbend + 8192;
        const int d1 = (raised >> 7) & 0x7f;
        const int d2 = raised & 0x7f;

        try {
            MappedEvent mE(instrument->getId(),
                           MappedEvent::MidiPitchBend,
                           d1,
                           d2);
            mE.setRecordedChannel(channel);
            mE.setEventTime(insertTime);
            mE.setTrackId(trackId);
            inserter.insertCopy(mE);
        } catch (...) {
            // Ignore.
        }
    }
}

void ChannelManager::insertEvent(
        TrackId trackId,
        const ControllerAndPBList &controllerAndPBList,
        RealTime reftime,
        MappedEvent &event,
        bool /*firstOutput*/,
        MappedInserterBase &inserter)
{
    //Profiler profiler("ChannelManager::insertEvent()", true);

    //RG_DEBUG << "insertEvent(): playing on" << (m_instrument ? m_instrument->getPresentationName().c_str() : "nothing") << "at" << reftime << (firstOutput ? "needs init" : "doesn't need init");

    // ??? firstOutput was always ignored.  What would happen if we actually
    //     honored it?  E.g.:
    //       if (firstOutput)
    //           m_ready = false;

    // We got here without being ready.  This might happen briefly
    // if a track becomes unmuted, until the meta-iterator gets around
    // to initting.
    if (!m_ready) {
        makeReady(trackId, reftime, controllerAndPBList, inserter);
        // If we're still not ready, we can't do much.
        if (!m_ready)
            return;
    }

    // !!! These checks may not be needed now, could become assertions.
    if (!m_instrument)
        return;
    if (!m_channelInterval.validChannel())
        return;

    event.setInstrument(m_instrument->getId());
    event.setRecordedChannel(m_channelInterval.getChannelId());
    event.setTrackId(trackId);
    inserter.insertCopy(event);
}

bool ChannelManager::makeReady(
        TrackId trackId,
        RealTime time,
        const ControllerAndPBList &controllerAndPBList,
        MappedInserterBase &inserter)
{
    //RG_DEBUG << "makeReady() for" << (m_instrument ? m_instrument->getPresentationName().c_str() : "nothing") << "at" << time;

    // We don't even have an instrument to play on.
    if (!m_instrument)
        return false;

    // Try to get a valid channel if we lack one.
    if (!m_channelInterval.validChannel()) {
        // We already tried to get one and failed; don't keep trying.
        if (m_triedToGetChannel)
            return false;
        
        // Try to get a channel.  This sets m_triedToGetChannel.
        allocateChannelInterval(false);
        
        // If we still don't have one, give up.
        if (!m_channelInterval.validChannel())
            return false;
    }
    
    // If this instrument is in auto channels mode
    if (!m_instrument->hasFixedChannel()) {
        insertChannelSetup(
                trackId,
                time,
                controllerAndPBList,
                inserter);
    }
    
    m_ready = true;

    return true;
}

void
ChannelManager::insertChannelSetup(
        TrackId trackId,
        RealTime insertTime,
        const ControllerAndPBList &controllerAndPBList,
        MappedInserterBase &inserter)
{
    //RG_DEBUG << "insertChannelSetup() : " << (m_instrument ? "Got instrument" : "No instrument");

    if (!m_instrument)
        return;
    if (!m_channelInterval.validChannel())
        return;

    //RG_DEBUG << "  Instrument type is " << (int)m_instrument->getType();

    // We don't do this for SoftSynth instruments.
    if (m_instrument->getType() == Instrument::Midi) {
        ChannelId channel = m_channelInterval.getChannelId();
        insertChannelSetup(trackId, m_instrument, channel, insertTime,
                           controllerAndPBList, inserter);
    }
}

void
ChannelManager::setChannelIdDirectly()
{
    Q_ASSERT(!m_usingAllocator);

    ChannelId channel = m_instrument->getNaturalChannel();

    if (m_instrument->getType() == Instrument::Midi) {
        // !!! Stopgap measure.  If we ever share allocators between
        // MIDI devices, this will have to become smarter.
        if (m_instrument->isPercussion()) {
            channel = (m_instrument->hasFixedChannel() ?
                       m_instrument->getNaturalChannel() : 9);
        }
    }

    m_channelInterval.setChannelId(channel);
}

AllocateChannels *
ChannelManager::getAllocator()
{
    Q_ASSERT(m_usingAllocator);

    if (!m_instrument)
        return 0;

    return m_instrument->getDevice()->getAllocator();
}

void
ChannelManager::connectAllocator()
{
    Q_ASSERT(m_usingAllocator);
    if (!m_channelInterval.validChannel()) { return; }
    connect(getAllocator(), SIGNAL(sigVacateChannel(ChannelId)),
            this, SLOT(slotVacateChannel(ChannelId)),
            Qt::UniqueConnection);
}

// Disconnect from the allocator's signals
// @author Tom Breton (Tehom) 
void
ChannelManager::
disconnectAllocator(void)
{
    if (m_instrument && m_usingAllocator) {
        disconnect(getAllocator(), 0, this, 0);
    }
}

// Set m_usingAllocator appropriately for instrument.  It is safe to
// pass NULL here.
// @author Tom Breton (Tehom) 
void
ChannelManager::
setAllocationMode(Instrument *instrument)
{
    if (!instrument)
        { m_usingAllocator = false; }
    else
        {
            bool wasUsingAllocator = m_usingAllocator;
            switch (instrument->getType()) {
            case Instrument::Midi :
                m_usingAllocator = !instrument->hasFixedChannel();
                break;
            case Instrument::SoftSynth:
                m_usingAllocator = false;
                break;
            case Instrument::Audio:
            case Instrument::InvalidInstrument:
            default:
                RG_DEBUG << "setAllocationMode() : Got an "
                    "audio or unrecognizable instrument type."
                         << endl;
                break;
            }

            // Clear m_channelInterval, otherwise its old value will appear valid.
            if (m_usingAllocator != wasUsingAllocator) {
                m_channelInterval.clearChannelId();
            }
        }
}    

void
ChannelManager::allocateChannelInterval(bool changedInstrument)
{
    RG_DEBUG << "allocateChannelInterval "
             << (m_usingAllocator ? "using allocator" :
                 "not using allocator")
             << "for"
             << (void *)m_instrument
             << endl;
    if (m_instrument) {
        if (m_usingAllocator) {
            // Only Midi instruments should have m_usingAllocator set.
            Q_ASSERT(m_instrument->getType() == Instrument::Midi);
            getAllocator()->
                reallocateToFit(*m_instrument, m_channelInterval,
                                m_start, m_end,
                                m_startMargin, m_endMargin,
                                changedInstrument);
            connectAllocator();
        } else {
            setChannelIdDirectly();
        }
    }

    if (m_channelInterval.validChannel()) {
        RG_DEBUG << "  Channel is valid";
    } else {
        RG_DEBUG << "  ??? Channel is invalid!  (end of allocateChannelInterval())";
    }

    m_triedToGetChannel = true;
}

void ChannelManager::freeChannelInterval(void)
{
    if (m_instrument && m_usingAllocator) {
        AllocateChannels *allocater = getAllocator();
        if (allocater) {
            allocater->freeChannelInterval(m_channelInterval);
            disconnectAllocator();
        }
        m_triedToGetChannel = false;
    }
}

// Set the instrument we are playing on, releasing any old one.
// @author Tom Breton (Tehom)
void
ChannelManager::
setInstrument(Instrument *instrument)
{
    RG_DEBUG << "setInstrument: Setting instrument to" 
             << (void *)instrument
             << "It was"
             << (void *)m_instrument
             << endl;

    if (instrument != m_instrument) {
        if (m_instrument) {
            Device *oldDevice = m_instrument->getDevice();
            Device *newDevice = instrument ? instrument->getDevice() : 0;
            // Don't hold onto a channel on a device we're no longer
            // playing thru.  Even if newDevice == 0, we free oldDevice's
            // channel.
            if (oldDevice != newDevice) {
                freeChannelInterval();
            }
        }
        allocateChannelInterval(true);
        connectInstrument(instrument);
        m_ready = false;
    }
}

// Print our status, for tracing.
// @author Tom Breton (Tehom) 
void
ChannelManager::
debugPrintStatus(void)
{
    RG_DEBUG
        << "ChannelManager "
        << (m_ready ? "doesn't need" : "needs")
        << "initting"
        << endl;
}

// Something is kicking everything off "channel" in our device.  It is
// the signaller's responsibility to put AllocateChannels right (in
// fact this signal only sent by AllocateChannels)
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotVacateChannel(ChannelId channel)
{
    if (m_channelInterval.getChannelId() == channel) {
        m_channelInterval.clearChannelId();
        disconnectAllocator();
    }
}


// Our instrument and its entire device are being destroyed.  We can
// skip setting the device's allocator right since it's going away.
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotLosingDevice(void)
{
    m_instrument = 0;
    m_channelInterval.clearChannelId();
}

// Our instrument is being destroyed.  We may or may not have
// received slotLosingDevice first.
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotLosingInstrument(void)
{
    freeChannelInterval();
    m_instrument = 0;
}

// Our instrument's channel is now fixed.
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotChannelBecomesFixed(void)
{
    RG_DEBUG << "slotChannelBecomesFixed" 
             << (m_usingAllocator ? "using allocator" :
                 "not using allocator")
             << "for"
             << (void *)m_instrument
             << endl;

    ChannelId channel = m_instrument->getNaturalChannel();
    if (!m_usingAllocator && (channel == m_channelInterval.getChannelId()))
        { return; }

    // Free the channel that we had (safe even if already fixed)
    freeChannelInterval();
    m_usingAllocator = false;

    // Set the new channel.
    setChannelIdDirectly();
    m_ready = false;
}

// Our instrument's channel is now unfixed.
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotChannelBecomesUnfixed(void)
{
    RG_DEBUG << "slotChannelBecomesUnfixed" 
             << (m_usingAllocator ? "using allocator" :
                 "not using allocator")
             << "for"
             << (void *)m_instrument
             << endl;
    // If we were already unfixed, do nothing.
    if (m_usingAllocator) { return; }

    m_usingAllocator = true;
    // We no longer have a channel interval.
    m_channelInterval.clearChannelId();
    // Get a new one.
    allocateChannelInterval(false);
    m_ready = false;
}

// Our instrument has changed how to set up the channel.
// @author Tom Breton (Tehom) 
void
ChannelManager::
slotInstrumentChanged(void)
{
    m_triedToGetChannel = false;

    // Reset to the fixedness of the instrument.  This is safe even
    // when fixedness hasn't really changed.
    if(m_instrument) {
        if(m_instrument->hasFixedChannel() ||
           (m_instrument->getType() != Instrument::Midi))
            { slotChannelBecomesFixed(); }
        else
            { slotChannelBecomesUnfixed(); }
    }

    // The above code won't always set dirty flag, so set it now.
    m_ready = false;
}


}

