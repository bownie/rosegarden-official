
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

// include files for QT
#include <qdir.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qdragobject.h>

// include files for KDE
#include <kstdaccel.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kprocess.h>
#include <dcopclient.h>

#include <kaction.h>
#include <kstdaction.h>

// application specific includes
#include "rosegardengui.h"
#include "rosegardenguiview.h"
#include "rosegardenguidoc.h"
#include "MidiFile.h"
#include "rg21io.h"
#include "rosegardendcop.h"
#include "ktmpstatusmsg.h"
#include "TrackPerformanceHelper.h"

#define ID_STATUS_MSG 1

using std::cerr;
using std::endl;
using std::cout;

static Rosegarden::MappedComposition mappComp;

RosegardenGUIApp::RosegardenGUIApp()
    : KMainWindow(0), DCOPObject("RosegardenGUIIface"),
      m_config(kapp->config()),
      m_fileRecent(0),
      m_view(0),
      m_doc(0),
      m_selectDefaultTool(0),
      m_transportStatus(STOPPED),
      m_sequencerProcess(0)
{
    // accept dnd
    setAcceptDrops(true);

    ///////////////////////////////////////////////////////////////////
    // call inits to invoke all other construction parts
    setupActions();
    
    initStatusBar();
    initDocument();
    initView();
	
    readOptions();

    m_selectDefaultTool->activate();
    
//     ///////////////////////////////////////////////////////////////////
//     // disable menu and toolbar items at startup
//     disableCommand(ID_FILE_SAVE);
//     disableCommand(ID_FILE_SAVE_AS);
//     disableCommand(ID_FILE_PRINT);
 	
//     disableCommand(ID_EDIT_CUT);
//     disableCommand(ID_EDIT_COPY);
//     disableCommand(ID_EDIT_PASTE);
}

RosegardenGUIApp::~RosegardenGUIApp()
{
    kdDebug(KDEBUG_AREA) << "~RosegardenGUIApp()\n";
    m_sequencerProcess->blockSignals(true);
    delete m_sequencerProcess;
}


void RosegardenGUIApp::setupActions()
{
    // setup File menu
    // New Window ?
    m_fileNew  = KStdAction::openNew (this, SLOT(fileNew()),     actionCollection());
    m_fileOpen = KStdAction::open    (this, SLOT(fileOpen()),    actionCollection());
    m_fileRecent = KStdAction::openRecent(this,
                                          SLOT(fileOpenRecent(const KURL&)),
                                          actionCollection());
    m_fileSave   = KStdAction::save  (this, SLOT(fileSave()),          actionCollection());
    m_fileSaveAs = KStdAction::saveAs(this, SLOT(fileSaveAs()),        actionCollection());
    m_fileClose  = KStdAction::close (this, SLOT(fileClose()),         actionCollection());
    m_filePrint  = KStdAction::print (this, SLOT(filePrint()),         actionCollection());

    new KAction(i18n("Import MIDI file..."), 0, 0, this,
                SLOT(importMIDI()), actionCollection(),
                "file_import_midi");

    new KAction(i18n("Import Rosegarden 2.1 file..."), 0, 0, this,
                SLOT(importRG21()), actionCollection(),
                "file_import_rg21");

    m_fileQuit = KStdAction::quit  (this, SLOT(quit()),              actionCollection());

    // setup edit menu
    KStdAction::undo     (this, SLOT(editUndo()),       actionCollection());
    KStdAction::redo     (this, SLOT(editRedo()),       actionCollection());
    m_editCut = KStdAction::cut      (this, SLOT(editCut()),        actionCollection());
    m_editCopy = KStdAction::copy     (this, SLOT(editCopy()),       actionCollection());
    m_editPaste = KStdAction::paste    (this, SLOT(editPaste()),      actionCollection());

    // setup Settings menu
    m_viewToolBar = KStdAction::showToolbar  (this, SLOT(toggleToolBar()),   actionCollection());
    m_viewTracksToolBar = new KToggleAction(i18n("Show Tracks Toolbar..."), 0, this,
                                            SLOT(toggleTracksToolBar()), actionCollection(),
                                            "show_tracks_toolbar");
    m_viewStatusBar = KStdAction::showStatusbar(this, SLOT(toggleStatusBar()), actionCollection());

    KStdAction::saveOptions(this, SLOT(save_options()), actionCollection());
    KStdAction::preferences(this, SLOT(customize()),    actionCollection());

    KStdAction::keyBindings      (this, SLOT(editKeys()),     actionCollection());
    KStdAction::configureToolbars(this, SLOT(editToolbars()), actionCollection());

    KRadioAction *action = 0;
    
    // TODO : add some shortcuts here
    action = new KRadioAction(i18n("Erase"), "eraser",
                              0,
                              this, SLOT(eraseSelected()),
                              actionCollection(), "erase");
    action->setExclusiveGroup("tracktools");

    action = new KRadioAction(i18n("Draw"), "pencil",
                              0,
                              this, SLOT(drawSelected()),
                              actionCollection(), "draw");
    action->setExclusiveGroup("tracktools");

    action = new KRadioAction(i18n("Move"), "move",
                              0,
                              this, SLOT(moveSelected()),
                              actionCollection(), "move");
    action->setExclusiveGroup("tracktools");

    m_selectDefaultTool = action;

    action = new KRadioAction(i18n("Resize"), "misc", // TODO : find a better icon
                              0,
                              this, SLOT(resizeSelected()),
                              actionCollection(), "resize");
    action->setExclusiveGroup("tracktools");

    new KAction(i18n("Change Time Resolution..."), 
                0,
                this, SLOT(slotChangeTimeResolution()),
                actionCollection(), "change_time_res");

    new KAction(i18n("Edit All Tracks"),
                0,
                this, SLOT(editAllTracks()),
                actionCollection(), "edit_all_tracks");

    // Transport controls [rwb]
    //
    // We set some default key bindings - with numlock off
    // use 1 (End) and 3 (Page Down) for Rwd and Ffwd and
    // 0 (insert) and Enter for Play and Stop 
    //
    KAction *transportAction;
    transportAction = new KAction(i18n("Play"), 0, 0, this,
                                  SLOT(play()), actionCollection(),
                                  "play");
    transportAction->setGroup("transportcontrols");
    transportAction->setAccel(Key_Insert);

    transportAction = new KAction(i18n("Stop"), 0, 0, this,
                                  SLOT(stop()), actionCollection(),
                                  "stop");
    transportAction->setGroup("transportcontrols");
    transportAction->setAccel(Key_Enter);

    transportAction = new KAction(i18n("Fast Forward"), 0, 0, this,
                                  SLOT(fastforward()), actionCollection(),
                                  "fast_forward");
    transportAction->setGroup("transportcontrols");
    transportAction->setAccel(Key_PageDown);

    transportAction = new KAction(i18n("Rewind"), 0, 0, this,
                                  SLOT(rewind()), actionCollection(),
                                  "rewind");
    transportAction->setGroup("transportcontrols");
    transportAction->setAccel(Key_End);


    createGUI("rosegardenui.rc");
}


void RosegardenGUIApp::initStatusBar()
{
    ///////////////////////////////////////////////////////////////////
    // STATUSBAR
    // TODO: add your own items you need for displaying current
    // application status.
    statusBar()->insertItem(KTmpStatusMsg::getDefaultMsg(), KTmpStatusMsg::getDefaultId());
}

void RosegardenGUIApp::initDocument()
{
    m_doc = new RosegardenGUIDoc(this);
    m_doc->newDocument();
}

void RosegardenGUIApp::initView()
{ 
    ////////////////////////////////////////////////////////////////////
    // create the main widget here that is managed by KTMainWindow's view-region and
    // connect the widget to your document to display document contents.

    kdDebug(KDEBUG_AREA) << "RosegardenGUIApp::initView()" << endl;

    m_view = new RosegardenGUIView(this);
    m_doc->addView(m_view);
    setCentralWidget(m_view);
    setCaption(m_doc->getTitle());

}

void RosegardenGUIApp::openDocumentFile(const char* _cmdl)
{
    KTmpStatusMsg msg(i18n("Opening file..."), statusBar());
    
    kdDebug(KDEBUG_AREA) << "RosegardenGUIApp::openDocumentFile("
                         << _cmdl << ")" << endl;

    m_doc->saveIfModified();
    m_doc->closeDocument();
    m_doc->openDocument(_cmdl);

    initView();
}

void RosegardenGUIApp::openFile(const QString& url)
{

    setCaption(url);
    KURL *u = new KURL( url );

    if (u->isMalformed()) {
        KMessageBox::sorry(this, i18n("This is not a valid filename.\n"));
        return;
    }

    if (!u->isLocalFile()) {
        KMessageBox::sorry(this, i18n("This is not a local file.\n"));
        return;
    }

    QFileInfo info(u->path());

    if (!info.exists()) {
        KMessageBox::sorry(this, i18n("The specified file does not exist"));
        return;
    }

    if (info.isDir()) {
        KMessageBox::sorry(this, i18n("You have specified a directory"));
        return;
    }

    QFile file(u->path());

    if (!file.open(IO_ReadOnly)) {
        KMessageBox::sorry(this, i18n("You do not have read permission to this file."));
        return;
    }

    m_doc->closeDocument();
    m_doc->openDocument(u->path());
    initView();
}


RosegardenGUIDoc *RosegardenGUIApp::getDocument() const
{
    return m_doc;
}

void RosegardenGUIApp::saveOptions()
{	
    m_config->setGroup("General Options");
    m_config->writeEntry("Geometry", size());
    m_config->writeEntry("Show Toolbar", m_viewToolBar->isChecked());
    m_config->writeEntry("Show Tracks Toolbar", m_viewTracksToolBar->isChecked());
    m_config->writeEntry("Show Statusbar",m_viewStatusBar->isChecked());
    m_config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
    m_config->writeEntry("TracksToolBarPos", (int) toolBar("tracksToolBar")->barPos());

    m_fileRecent->saveEntries(m_config, "Recent Files");
}


void RosegardenGUIApp::readOptions()
{
    m_config->setGroup("General Options");

    // status bar settings
    bool viewStatusbar = m_config->readBoolEntry("Show Statusbar", true);
    m_viewStatusBar->setChecked(viewStatusbar);
    toggleStatusBar();

    bool viewToolBar = m_config->readBoolEntry("Show Toolbar", true);
    m_viewToolBar->setChecked(viewToolBar);
    toggleToolBar();

    viewToolBar = m_config->readBoolEntry("Show Tracks Toolbar", true);
    m_viewTracksToolBar->setChecked(viewToolBar);
    toggleTracksToolBar();

    // bar position settings
    KToolBar::BarPosition toolBarPos;
    toolBarPos=(KToolBar::BarPosition) m_config->readNumEntry("ToolBarPos", KToolBar::Top);
    toolBar("mainToolBar")->setBarPos(toolBarPos);

    toolBarPos=(KToolBar::BarPosition) m_config->readNumEntry("TracksToolBarPos", KToolBar::Top);
    toolBar("tracksToolBar")->setBarPos(toolBarPos);
	
    // initialize the recent file list
    //
    m_fileRecent->loadEntries(m_config);

    QSize size(m_config->readSizeEntry("Geometry"));

    if(!size.isEmpty()) {
        resize(size);
    }
}

void RosegardenGUIApp::saveProperties(KConfig *_cfg)
{
    if (m_doc->getTitle()!=i18n("Untitled") && !m_doc->isModified()) {
        // saving to tempfile not necessary
    } else {
        QString filename=m_doc->getAbsFilePath();	
        _cfg->writeEntry("filename", filename);
        _cfg->writeEntry("modified", m_doc->isModified());
		
        QString tempname = kapp->tempSaveName(filename);
        m_doc->saveDocument(tempname);
    }
}


void RosegardenGUIApp::readProperties(KConfig* _cfg)
{
    QString filename = _cfg->readEntry("filename", "");
    bool modified = _cfg->readBoolEntry("modified", false);

    if (modified) {
            bool canRecover;
            QString tempname = kapp->checkRecoverFile(filename, canRecover);
  	
            if (canRecover) {
                m_doc->openDocument(tempname);
                m_doc->setModified();
                QFileInfo info(filename);
                m_doc->setAbsFilePath(info.absFilePath());
                m_doc->setTitle(info.fileName());
                QFile::remove(tempname);
            }
        } else {
            if (!filename.isEmpty()) {
                m_doc->openDocument(filename);
            }
        }

    QString caption=kapp->caption();
    setCaption(caption+": "+m_doc->getTitle());
}		

void RosegardenGUIApp::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(QUriDrag::canDecode(event));
}

void RosegardenGUIApp::dropEvent(QDropEvent *event)
{
    // this is a very simplistic implementation of a drop event.  we
    // will only accept a dropped URL.  the Qt dnd code can do *much*
    // much more, so please read the docs there
    QStrList uri;

    // see if we can decode a URI.. if not, just ignore it
    if (QUriDrag::decode(event, uri))
    {
        // okay, we have a URI.. process it
        QString url, target;
        url = uri.first();

        // load in the file
        openURL(KURL(url));
    }
}


bool RosegardenGUIApp::queryClose()
{
    return m_doc->saveIfModified();
}

bool RosegardenGUIApp::queryExit()
{
    saveOptions();
    return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

// Not connected to anything at the moment
//
void RosegardenGUIApp::fileNewWindow()
{
    KTmpStatusMsg msg(i18n("Opening a new application window..."), statusBar());
	
    RosegardenGUIApp *new_window= new RosegardenGUIApp();
    new_window->show();
}

void RosegardenGUIApp::fileNew()
{
    KTmpStatusMsg msg(i18n("Creating new document..."), statusBar());

    if (!m_doc->saveIfModified()) {
        // here saving wasn't successful

    } else {	
        m_doc->newDocument();		

        QString caption=kapp->caption();	
        setCaption(caption+": "+m_doc->getTitle());
    }
}

void RosegardenGUIApp::openURL(const KURL& url)
{
    QString netFile = url.url();
    kdDebug(KDEBUG_AREA) << "RosegardenGUIApp::openURL: " << netFile << endl;

    if (url.isMalformed()) {
        QString string;
        string = i18n( "Malformed URL\n%1").arg(netFile);

        KMessageBox::sorry(this, string);
        return;
    }

    QString target;

    if (KIO::NetAccess::download(url, target) == false) {
        KMessageBox::error(this, i18n("Cannot download file!"));
        return;
    }

    static QRegExp midiFile("\\.mid$"), rg21File("\\.rose$");

    if (midiFile.match(url.path()) != -1) {
        importMIDIFile(target);
    } else if (rg21File.match(url.path()) != -1) {
        importRG21File(target);
    } else {
        openFile(target);
    }
    
    setCaption(url.path());
    m_fileRecent->addURL(url);
}

void RosegardenGUIApp::fileOpen()
{
    KTmpStatusMsg msg(i18n("Opening file..."), statusBar());

    KURL url = KFileDialog::getOpenURL(QString::null, "*.xml", this,
                                       i18n("Open File"));
    if ( url.isEmpty() ) { return; }

    openURL(url);
}

void RosegardenGUIApp::fileOpenRecent(const KURL &url)
{
    KTmpStatusMsg msg(i18n("Opening file..."), statusBar());
	
//     if (!doc->saveIfModified()) {
//         // here saving wasn't successful
//     } else {
//         doc->closeDocument();
//         doc->openDocument(recentFiles.at(id_));
//         QString caption=kapp->caption();	
//         setCaption(caption+": "+doc->getTitle());
//     }

//     initView();

    openURL(url);
}

void RosegardenGUIApp::fileSave()
{
    if (!m_doc->isModified()) return;

    KTmpStatusMsg msg(i18n("Saving file..."), statusBar());

    if (!m_doc->getAbsFilePath())
        fileSaveAs();
    else
        m_doc->saveDocument(m_doc->getAbsFilePath());
}

void RosegardenGUIApp::fileSaveAs()
{
    KTmpStatusMsg msg(i18n("Saving file with a new filename..."), statusBar());

    QString newName=KFileDialog::getSaveFileName(QDir::currentDirPath(),
                                                 i18n("*.xml"), this, i18n("Save as..."));
    if (!newName.isEmpty()) {
        QFileInfo saveAsInfo(newName);
        m_doc->setTitle(saveAsInfo.fileName());
        m_doc->setAbsFilePath(saveAsInfo.absFilePath());
        m_doc->saveDocument(newName);
        m_fileRecent->addURL(newName);

        QString caption=kapp->caption();	
        setCaption(caption + ": " + m_doc->getTitle());
    }
}

void RosegardenGUIApp::fileClose()
{
    KTmpStatusMsg msg(i18n("Closing file..."), statusBar());
	
    m_doc->saveIfModified();

    m_doc->closeDocument();

    m_doc->newDocument();

    initView();

    //close();
}

void RosegardenGUIApp::filePrint()
{
    KTmpStatusMsg msg(i18n("Printing..."), statusBar());

    QPrinter printer;

    if (printer.setup(this)) {
        m_view->print(&printer);
    }
}

void RosegardenGUIApp::quit()
{
    statusMsg(i18n("Exiting..."));
    saveOptions();
    // close the first window, the list makes the next one the first again.
    // This ensures that queryClose() is called on each window to ask for closing
    KMainWindow* w;
    if (memberList) {

        for(w=memberList->first(); w!=0; w=memberList->first()) {
            // only close the window if the closeEvent is accepted. If
            // the user presses Cancel on the saveIfModified() dialog,
            // the window and the application stay open.
            if (!w->close())
                break;
        }
    }	
}

void RosegardenGUIApp::editUndo()
{
    KTmpStatusMsg msg(i18n("Undo..."), statusBar());
}

void RosegardenGUIApp::editRedo()
{
    KTmpStatusMsg msg(i18n("Redo..."), statusBar());
}

void RosegardenGUIApp::editCut()
{
    KTmpStatusMsg msg(i18n("Cutting selection..."), statusBar());
}

void RosegardenGUIApp::editCopy()
{
    KTmpStatusMsg msg(i18n("Copying selection to clipboard..."), statusBar());
}

void RosegardenGUIApp::editPaste()
{
    KTmpStatusMsg msg(i18n("Inserting clipboard contents..."), statusBar());
}

void RosegardenGUIApp::toggleToolBar()
{
    KTmpStatusMsg msg(i18n("Toggle the toolbar..."), statusBar());

    if (m_viewToolBar->isChecked())
        toolBar("mainToolBar")->show();
    else
        toolBar("mainToolBar")->hide();
}

void RosegardenGUIApp::toggleTracksToolBar()
{
    KTmpStatusMsg msg(i18n("Toggle the tracks toolbar..."), statusBar());

    if (m_viewTracksToolBar->isChecked())
        toolBar("tracksToolBar")->show();
    else
        toolBar("tracksToolBar")->hide();
}

void RosegardenGUIApp::toggleStatusBar()
{
    KTmpStatusMsg msg(i18n("Toggle the statusbar..."), statusBar());

    if(!m_viewStatusBar->isChecked())
        statusBar()->hide();
    else
        statusBar()->show();
}


void RosegardenGUIApp::statusMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message permanently
    statusBar()->clear();
    statusBar()->changeItem(text, ID_STATUS_MSG);
}


void RosegardenGUIApp::statusHelpMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message of whole statusbar temporary (text, msec)
    statusBar()->message(text, 2000);
}

void RosegardenGUIApp::eraseSelected()
{
    m_view->eraseSelected();
}

void RosegardenGUIApp::drawSelected()
{
    m_view->drawSelected();
}

void RosegardenGUIApp::moveSelected()
{
    m_view->moveSelected();
}

void RosegardenGUIApp::resizeSelected()
{
    m_view->resizeSelected();
}

#include <qlayout.h>
#include <qspinbox.h>
#include <kdialog.h>

class GetTimeResDialog : public KDialog
{
public:
    GetTimeResDialog(QWidget *parent = 0,
                     const char *name = 0,
                     bool modal = false, WFlags f = 0);

    void setInitialTimeRes(unsigned int);
    
    unsigned int getTimeRes() const;
    
protected:
    QSpinBox *m_spinbox;
};

GetTimeResDialog::GetTimeResDialog(QWidget *parent,
                                   const char *name,
                                   bool modal, WFlags f)
    : KDialog(parent, name, modal, f),
      m_spinbox(0)
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->setAutoAdd(true);
    
    new QLabel("Enter new time resolution", this);
    m_spinbox = new QSpinBox(this);
}


unsigned int GetTimeResDialog::getTimeRes() const
{
    return m_spinbox->value();
}

void GetTimeResDialog::setInitialTimeRes(unsigned int v)
{
    m_spinbox->setValue(v);
}



void RosegardenGUIApp::changeTimeResolution()
{
    GetTimeResDialog *dialog = new GetTimeResDialog(this);
    
    dialog->setInitialTimeRes(0);
    dialog->show();

    if (dialog->result()) {
        
        unsigned int timeResolution = dialog->getTimeRes();
    }
    
}

const Rosegarden::MappedComposition&
RosegardenGUIApp::getSequencerSlice(const Rosegarden::timeT &sliceStart,
                                    const Rosegarden::timeT &sliceEnd)
{
  //cerr << "getSequencerSlice : " << sliceStart << " : " << sliceEnd << endl;
/*
  mappComp = new Rosegarden::MappedComposition(m_doc->getComposition(),
                                              sliceStart, sliceEnd);
*/

  // EXPERIMENT!
  //
  // [rwb] - moved this code in here from MappedComposition
  //         to reduce playback latency
  //

  mappComp.clear();
  mappComp.startTime(sliceStart);
  mappComp.endTime(sliceEnd);

  Rosegarden::timeT eventTime;

  for (Rosegarden::Composition::iterator i = m_doc->getComposition().begin();
                             i != m_doc->getComposition().end(); i++ )
  {
    // Skip the Track if it starts too late to be of
    // interest to our slice.
    if ( (*i)->getStartIndex() > sliceEnd )
      continue;

    Rosegarden::TrackPerformanceHelper helper(**i);

    for ( Rosegarden::Track::iterator j = (*i)->begin(); j != (*i)->end(); j++ )
    {
      // for the moment ensure we're all positive
      assert((*j)->getAbsoluteTime() >= 0 );

      // Skip this event if it isn't a note
      //
      if (!(*j)->isa(Rosegarden::Note::EventType))
        continue;

      // Find the performance duration, i.e. taking into account any
      // ties etc that this note may have  --cc
      // 
      timeT duration = helper.getSoundingDuration(j);

      if (duration == 0) // probably in a tied series, but not as first note
        continue;

      // get the eventTime
      eventTime = (unsigned int) (*j)->getAbsoluteTime();

      // As events are stored chronologically we can escape if
      // we're already beyond our event horizon for this slice.
      //
      if ( eventTime > sliceEnd )
        break;

      // Eliminate events before our required time
      if ( eventTime >= sliceStart && eventTime <= sliceEnd)
      {
        // insert event
        Rosegarden::MappedEvent *me = new Rosegarden::MappedEvent(**j, duration);
        me->setInstrument((*i)->getInstrument());
        mappComp.insert(me);
      }
    }
  }

  return mappComp;
}


void RosegardenGUIApp::importMIDI()
{
  KURL url = KFileDialog::getOpenURL(QString::null, "*.mid", this,
                                     i18n("Open MIDI File"));
  if (url.isEmpty()) { return; }

  QString tmpfile;
  KIO::NetAccess::download(url, tmpfile);
  importMIDIFile(tmpfile);
  
  KIO::NetAccess::removeTempFile( tmpfile );
}

void RosegardenGUIApp::importMIDIFile(const QString &file)
{
  Rosegarden::MidiFile *midiFile;

  midiFile = new Rosegarden::MidiFile(file.ascii());

  midiFile->open();

  m_doc->closeDocument();
  m_doc->newDocument();

  Rosegarden::Composition *tmpComp = midiFile->convertToRosegarden();

  m_doc->getComposition().swap(*tmpComp);

  delete tmpComp;

  // Set modification flag
  //
  m_doc->setModified();

  initView();

}

void RosegardenGUIApp::importRG21()
{
  KURL url = KFileDialog::getOpenURL(QString::null, "*.rose", this,
                                     i18n("Open Rosegarden 2.1 File"));
  if (url.isEmpty()) { return; }

  QString tmpfile;
  KIO::NetAccess::download(url, tmpfile);

  importRG21File(tmpfile);

  KIO::NetAccess::removeTempFile(tmpfile);
}

void RosegardenGUIApp::importRG21File(const QString &file)
{
  RG21Loader rg21Loader(file);
    
  m_doc->closeDocument();
  m_doc->newDocument();
  Rosegarden::Composition *tmpComp = rg21Loader.getComposition();

  m_doc->getComposition().swap(*tmpComp);

  delete tmpComp;

  // Set modification flag
  //
  m_doc->setModified();

  initView();
}

void RosegardenGUIApp::setPointerPosition(const int &position)
{
  // We do this the lazily dangerous way of setting Composition
  // time and then gui time - we should probably make this
  // an atomic operation with observers or something to make
  // it nice and encapsulated but as long as we only use this
  // modifier method for changing composition time we can probably
  // get away with it.

  // set the composition time
  m_doc->getComposition().setPosition((timeT) position);

  // and the gui time
  m_view->setPointerPosition(position);
}

void RosegardenGUIApp::play()
{
  QByteArray data;
  QCString replyType;
  QByteArray replyData;

  if (m_transportStatus == PLAYING)
    return;

  if (!m_sequencerProcess && !launchSequencer())
      return;

  // write the start position argument to the outgoing stream
  //
  QDataStream streamOut(data, IO_WriteOnly);
  streamOut << m_doc->getComposition().getPosition();

  streamOut << 20;  // playback latency
  streamOut << 10;  // fetch latency

  cout << "RosegardenGUIApp::play() - playing at tempo " << 
                m_doc->getComposition().getTempo() << endl;

  // Send Play to the Sequencer
  if (!kapp->dcopClient()->call(ROSEGARDEN_SEQUENCER_APP_NAME,
                                ROSEGARDEN_SEQUENCER_IFACE_NAME,
                                "play(Rosegarden::timeT, Rosegarden::timeT, Rosegarden::timeT)",
                                data, replyType, replyData))
  {
    // failed - pop up and disable sequencer options
    m_transportStatus = STOPPED;
    KMessageBox::error(this,
      i18n("Failed to contact RosegardenSequencer"));
  }
  else
  {
    // ensure the return type is ok
    QDataStream streamIn(replyData, IO_ReadOnly);
    int result;
    streamIn >> result;

    if (result)
    {
      // completed successfully 
      m_transportStatus = STARTING_TO_PLAY;
    }
    else
    {
      m_transportStatus = STOPPED;
      KMessageBox::error(this, i18n("Failed to start playback"));
    }
  }

}

// Send stop request to Sequencer if playing, else
// return to start of track
void RosegardenGUIApp::stop()
{
  if (m_transportStatus == STOPPED)
  {
    setPointerPosition(0);
    return;
  }

  QByteArray data;
  QCString replyType;
  QByteArray replyData;

  // Send a Stop to the Sequencer
  if (!kapp->dcopClient()->call(ROSEGARDEN_SEQUENCER_APP_NAME,
                                ROSEGARDEN_SEQUENCER_IFACE_NAME,
                                "stop()", data,
                                replyType, replyData))
  {
    // failed - pop up and disable sequencer options
    KMessageBox::error(this,
      i18n("Failed to contact RosegardenSequencer"));
  }
  else
  {
    // ensure the return type is ok
    QDataStream streamIn(replyData, IO_ReadOnly);
    int result;
    streamIn >> result;

    if (!result)
    {
      KMessageBox::error(this, i18n("Failed to stop playback"));
    }
  }
  m_transportStatus = STOPPED;
}

// Jump to previous bar
//
void RosegardenGUIApp::rewind()
{
  double barNumber = ((double) m_doc->getComposition().getPosition())/
                     ((double) m_doc->getComposition().getNbTicksPerBar());
  int newBarNumber = (int) barNumber;

  if (barNumber < 1)
    newBarNumber = 0;
  else
    if (barNumber == (double) newBarNumber)
      newBarNumber--;

  if ( m_transportStatus == PLAYING )
  {
    sendSequencerJump(newBarNumber *
                         m_doc->getComposition().getNbTicksPerBar());
  }
  else
  {
    setPointerPosition(newBarNumber *
                           m_doc->getComposition().getNbTicksPerBar());
  }
}


// Jump to next bar
//
void RosegardenGUIApp::fastforward()
{
  double barNumber = ((double) m_doc->getComposition().getPosition())/
                     ((double) m_doc->getComposition().getNbTicksPerBar());
  int newBarNumber = (int) barNumber;

  // we need to work out where the trackseditor finishes so we
  // don't skip beyond it.  Generally we need extra-Composition
  // non-destructive start and end markers for the piece.
  //
  newBarNumber++;

  if ( m_transportStatus == PLAYING )
  {
    sendSequencerJump(newBarNumber *
                         m_doc->getComposition().getNbTicksPerBar());
  }
  else
  {
    setPointerPosition(newBarNumber * 
                         m_doc->getComposition().getNbTicksPerBar());
  }

}


// This method is a callback from the Sequencer to update the GUI
// with state change information.  The GUI requests the Sequencer
// to start playing or to start recording and enters a pending
// state (see rosegardendcop.h for TransportStatus values).
// The Sequencer replies when ready with it's status.  If anything
// fails then we default (or try to default) to STOPPED at both
// the GUI and the Sequencer.
//
void RosegardenGUIApp::notifySequencerStatus(const int& status)
{
  // for the moment we don't do anything fancy
  m_transportStatus = (TransportStatus) status;
}


void RosegardenGUIApp::sendSequencerJump(const Rosegarden::timeT &position)
{
  QByteArray data;
  QDataStream streamOut(data, IO_WriteOnly);
  streamOut << position;

  if (!kapp->dcopClient()->send(ROSEGARDEN_SEQUENCER_APP_NAME,
                                ROSEGARDEN_SEQUENCER_IFACE_NAME,
                                "jumpTo(Rosegarden::timeT)",
                                data))
  {
    // failed - pop up and disable sequencer options
    m_transportStatus = STOPPED;
    KMessageBox::error(this, i18n("Failed to contact RosegardenSequencer"));
  }

  return;
}

void RosegardenGUIApp::editAllTracks()
{
    m_view->editAllTracks(&m_doc->getComposition());
}


// Sequencer auxiliary process management


bool RosegardenGUIApp::launchSequencer()
{
    if (m_sequencerProcess) return true;

    KTmpStatusMsg msg(i18n("Starting the sequencer..."), statusBar());
    
    m_sequencerProcess = new KProcess;

    (*m_sequencerProcess) << "rosegardensequencer";

    connect(m_sequencerProcess, SIGNAL(processExited(KProcess*)),
            this, SLOT(sequencerExited(KProcess*)));

    bool res = m_sequencerProcess->start();
    
    if (!res) {
        KMessageBox::error(0, i18n("Couldn't start the sequencer"));
        kdDebug(KDEBUG_AREA) << "Couldn't start the sequencer\n";
    } else {
        // TODO: This is fugly
        kapp->processEvents(2 * 1000);
        sleep(1); // sleep 1 second to allow the sequencer do its connections
    }

    return res;
}

void RosegardenGUIApp::sequencerExited(KProcess*)
{
    kdDebug(KDEBUG_AREA) << "Sequencer exited\n";

    KMessageBox::error(0, i18n("Sequencer exited"));

    m_sequencerProcess = 0;
}

