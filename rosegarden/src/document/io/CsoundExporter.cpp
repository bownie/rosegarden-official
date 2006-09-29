/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
 
    This program is Copyright 2000-2006
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>
 
    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.
 
    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/


#include "CsoundExporter.h"

#include "base/Event.h"
#include "base/BaseProperties.h"
#include "base/Composition.h"
#include "base/NotationTypes.h"
#include "base/Segment.h"
#include "base/Track.h"
#include "gui/general/ProgressReporter.h"
#include <qobject.h>
#include <fstream>
#include "gui/application/RosegardenApplication.h"


namespace Rosegarden
{

CsoundExporter::CsoundExporter(QObject *parent,
                               Composition *composition,
                               std::string fileName) :
        ProgressReporter(parent, "csoundExporter"),
        m_composition(composition),
        m_fileName(fileName)
{
    // nothing else
}

CsoundExporter::~CsoundExporter()
{
    // nothing
}

static double
convertTime(Rosegarden::timeT t)
{
    return double(t) / double(Note(Note::Crotchet).getDuration());
}

bool
CsoundExporter::write()
{
    std::ofstream str(m_fileName.c_str(), std::ios::out);
    if (!str) {
        //std::cerr << "CsoundExporter::write() - can't write file" << std::endl;
        return false;
    }

    str << ";; Csound score file written by Rosegarden-4\n\n";
    if (m_composition->getCopyrightNote() != "") {
        str << ";; Copyright note:\n;; "
        //!!! really need to remove newlines from copyright note
        << m_composition->getCopyrightNote() << "\n";
    }

    int trackNo = 0;
    for (Composition::iterator i = m_composition->begin();
            i != m_composition->end(); ++i) {

        emit setProgress(int(double(trackNo++) / double(m_composition->getNbTracks()) * 100.0));
        rgapp->refreshGUI(50);

        str << "\n;; Segment: \"" << (*i)->getLabel() << "\"\n";
        str << ";; on Track: \""
        << m_composition->getTrackById((*i)->getTrack())->getLabel()
        << "\"\n";
        str << ";;\n;; Inst\tTime\tDur\tPitch\tVely\n"
        << ";; ----\t----\t---\t-----\t----\n";

        for (Segment::iterator j = (*i)->begin(); j != (*i)->end(); ++j) {

            if ((*j)->isa(Note::EventType)) {

                long pitch = 0;
                (*j)->get
                <Int>(BaseProperties::PITCH, pitch);

                long velocity = 127;
                (*j)->get
                <Int>(BaseProperties::VELOCITY, velocity);

                str << "   i"
                << (*i)->getTrack() << "\t"
                << convertTime((*j)->getAbsoluteTime()) << "\t"
                << convertTime((*j)->getDuration()) << "\t"
                << 3 + (pitch / 12) << ((pitch % 12) < 10 ? ".0" : ".")
                << pitch % 12 << "\t"
                << velocity << "\t\n";

            } else {
                str << ";; Event type: " << (*j)->getType() << std::endl;
            }
        }
    }

    int tempoCount = m_composition->getTempoChangeCount();

    if (tempoCount > 0) {

        str << "\nt ";

        for (int i = 0; i < tempoCount - 1; ++i) {

            std::pair<timeT, tempoT> tempoChange =
                m_composition->getTempoChange(i);

            timeT myTime = tempoChange.first;
            timeT nextTime = myTime;
            if (i < m_composition->getTempoChangeCount() - 1) {
                nextTime = m_composition->getTempoChange(i + 1).first;
            }

            int tempo = int(Composition::getTempoQpm(tempoChange.second));

            str << convertTime( myTime) << " " << tempo << " "
            << convertTime(nextTime) << " " << tempo << " ";
        }

        str << convertTime(m_composition->getTempoChange(tempoCount - 1).first)
        << " "
        << int(Composition::getTempoQpm(m_composition->getTempoChange(tempoCount - 1).second))
        << std::endl;
    }

    str << "\ne" << std::endl;
    str.close();
    return true;
}

}
