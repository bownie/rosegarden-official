
/*
    Rosegarden-4 v0.1
    A sequencer and musical notation editor.

    This program is Copyright 2000-2001
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _ROSEGARDEN_SEQUENCER_APP_H_
#define _ROSEGARDEN_SEQUENCER_APP_H_
 
// RosegardenSequencerApp is the sequencer application for Rosegarden.
// It has a Rosegarden::Sequencer object which wraps the aRTS level
// funtionality.  At this level we deal with comms with the Rosegarden
// GUI application and the high level marshalling of data.
//
//
//


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for Qt
#include <qstrlist.h>

// include files for KDE 
#include <kapp.h>
#include <kmainwindow.h>
#include <kaccel.h>

#include "Event.h"
#include "rosegardensequenceriface.h"
#include "Sequencer.h"

class KURL;
class KRecentFilesAction;

// forward declaration of the RosegardenGUI classes
class RosegardenGUIDoc;
class RosegardenGUIView;

class RosegardenSequencerApp : public KMainWindow,
                               virtual public RosegardenSequencerIface
{
  Q_OBJECT

public:
  RosegardenSequencerApp();
  ~RosegardenSequencerApp();

protected:

public slots:
  virtual void quit();

  // DCOP doesn't currently like to stream bools so we have to
  // use ints for return types
  virtual int play(const Rosegarden::timeT &position);
  virtual int stop();
    
private:

  Rosegarden::Sequencer *m_sequencer;

};
 
#endif // _ROSEGARDEN_SEQUENCER_APP_H_
