/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2016 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef RG_AUDIOSEGMENTMAPPER_H
#define RG_AUDIOSEGMENTMAPPER_H

#include "SegmentMapper.h"

namespace Rosegarden
{

class Segment;
class RosegardenDocument;

class AudioSegmentMapper : public SegmentMapper
{
public:
    AudioSegmentMapper(RosegardenDocument *, Segment *);

protected:
    virtual int calculateSize();

    /// dump all segment data in the file
    virtual void fillBuffer();
    // Return whether the event should be played.
    virtual bool shouldPlay(MappedEvent *evt, RealTime startTime);
};


}

#endif
