/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2022 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef RG_DSSIPLUGININSTANCE_H
#define RG_DSSIPLUGININSTANCE_H

#include <vector>
#include <set>
#include <map>
#include <QString>
#include "base/Instrument.h"

#if __linux__
#include "dssi.h"
#endif

#include "RingBuffer.h"
#include "RunnablePluginInstance.h"
#include "Scavenger.h"
#include <pthread.h>

namespace Rosegarden
{

class DSSIPluginInstance : public RunnablePluginInstance
{
public:
    ~DSSIPluginInstance() override;

    bool isOK() const override {
#if __linux__
        return m_instanceHandle != nullptr;
#else
        return false;
#endif
    }

    InstrumentId getInstrument() const { return m_instrument; }
    QString getIdentifier() const override { return m_identifier; }
    int getPosition() const { return m_position; }

    void run(const RealTime &) override;

    void setPortValue(unsigned int portNumber, float value) override;
    float getPortValue(unsigned int portNumber) override;
    QString configure(QString key, QString value) override;
    void sendEvent(const RealTime &eventTime,
                           const void *event) override;

    size_t getBufferSize() override { return m_blockSize; }
    size_t getAudioInputCount() override { return m_audioPortsIn.size(); }
    size_t getAudioOutputCount() override { return m_idealChannelCount; }
    sample_t **getAudioInputBuffers() override { return m_inputBuffers; }
    sample_t **getAudioOutputBuffers() override { return m_outputBuffers; }

    QStringList getPrograms() override;
    QString getCurrentProgram() override;
    QString getProgram(int bank, int program) override;
    unsigned long getProgram(QString name) override;
    void selectProgram(QString program) override;

    bool isBypassed() const override { return m_bypassed; }
    void setBypassed(bool bypassed) override { m_bypassed = bypassed; }

    size_t getLatency() override;

    void silence() override;
    void discardEvents() override;
    void setIdealChannelCount(size_t channels) override; // may re-instantiate

    virtual bool isInGroup() const { return m_grouped; }
    virtual void detachFromGroup();

protected:
    // To be constructed only by DSSIPluginFactory
#if __linux__
    friend class DSSIPluginFactory;
#endif

    // Constructor that creates the buffers internally
    // 
    DSSIPluginInstance(PluginFactory *factory,
                       InstrumentId instrument,
                       QString identifier,
                       int position,
                       unsigned long sampleRate,
                       size_t blockSize,
                       int idealChannelCount
#if __linux__
                       ,const DSSI_Descriptor* descriptor
#endif
                       );
    
    // Constructor that uses shared buffers
    // 
    DSSIPluginInstance(PluginFactory *factory,
                       InstrumentId instrument,
                       QString identifier,
                       int position,
                       unsigned long sampleRate,
                       size_t blockSize,
                       sample_t **inputBuffers,
                       sample_t **outputBuffers
#if __linux__
                       ,const DSSI_Descriptor* descriptor
#endif
                       );


    void init();
    void instantiate(unsigned long sampleRate);
    void cleanup();
    void activate();
    void deactivate();
    void connectPorts();

#if __linux__
    bool handleController(snd_seq_event_t *ev);
#endif

    void setPortValueFromController(unsigned int portNumber, int controlValue);
    void selectProgramAux(QString program, bool backupPortValues);
    void checkProgramCache();

    void initialiseGroupMembership();
    void runGrouped(const RealTime &);

    InstrumentId   m_instrument;
    int                        m_position;
#if __linux__
    LADSPA_Handle              m_instanceHandle;
    const DSSI_Descriptor     *m_descriptor;

    std::vector<std::pair<unsigned long, LADSPA_Data*> > m_controlPortsIn;
    std::vector<std::pair<unsigned long, LADSPA_Data*> > m_controlPortsOut;
    std::vector<LADSPA_Data>  m_backupControlPortsIn;
#endif

    std::vector<bool>  m_portChangedSinceProgramChange;

    std::map<int, int>        m_controllerMap;

    std::vector<int>          m_audioPortsIn;
    std::vector<int>          m_audioPortsOut;

    struct ProgramControl {
        int msb;
        int lsb;
        int program;
    };
    ProgramControl m_pending;

    struct ProgramDescriptor {
        int bank;
        int program;
        QString name;
    };
    std::vector<ProgramDescriptor> m_cachedPrograms;
    bool m_programCacheValid;

#if __linux__
    RingBuffer<snd_seq_event_t> m_eventBuffer;
#endif

    size_t                    m_blockSize;
    sample_t                **m_inputBuffers;
    sample_t                **m_outputBuffers;
    bool                      m_ownBuffers;
    size_t                    m_idealChannelCount;
    size_t                    m_outputBufferCount;
    size_t                    m_sampleRate;
    float                    *m_latencyPort;

    bool                      m_run;
    bool                      m_runSinceReset;
    
    bool                      m_bypassed;
    QString                   m_program;
    bool                      m_grouped;
    RealTime                  m_lastRunTime;

    pthread_mutex_t           m_processLock;

    typedef std::set<DSSIPluginInstance *> PluginSet;
    typedef std::map<QString, PluginSet> GroupMap;
    static GroupMap m_groupMap;

#if __linux__
    static snd_seq_event_t **m_groupLocalEventBuffers;
#endif

    static size_t m_groupLocalEventBufferCount;

#if __linux__
    static Scavenger<ScavengerArrayWrapper<snd_seq_event_t *> > m_bufferScavenger;
#endif

};

};

#endif // RG_DSSIPLUGININSTANCE_H

