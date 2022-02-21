/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2022 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef RG_SHORTCUTWARNDIALOG_H
#define RG_SHORTCUTWARNDIALOG_H

#include <QDialog>

#include "gui/general/ActionData.h"

namespace Rosegarden
{


/// Shortcut Conflicts dialog.
class ShortcutWarnDialog : public QDialog
{
    Q_OBJECT

 public:
    ShortcutWarnDialog(ActionData::DuplicateData ddata);
    ~ShortcutWarnDialog();

 public slots:
    void OKclicked();
     
 private:
    struct KeyDuplicateButton
    {
        QString key;
        QPushButton* button;
    };
    
    typedef std::list<KeyDuplicateButton> KeyDuplicateButtonList;
    
    struct KeyDuplicateButtons
    {
        QPushButton* editButton;
        KeyDuplicateButtonList buttonList;
    };
    
    struct DuplicateButtons
    {
        QString editKey;
        std::map<QKeySequence, KeyDuplicateButtons> duplicateButtonMap;
    };
    
    DuplicateButtons m_duplicateButtons;
};

}

#endif
