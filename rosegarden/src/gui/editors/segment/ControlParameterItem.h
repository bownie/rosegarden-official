
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

#ifndef _RG_CONTROLPARAMETERITEM_H_
#define _RG_CONTROLPARAMETERITEM_H_

#include <qstring.h>
#include <klistview.h>


namespace Rosegarden
{


class ControlParameterItem : public KListViewItem
{
public:
    ControlParameterItem(int id,
                         QListView *parent,
                         QString str1,
                         QString str2,
                         QString str3,
                         QString str4,
                         QString str5,
                         QString str6,
                         QString str7,
                         QString str8,
                         QString str9):
        KListViewItem(parent, str1, str2, str3, str4, str5, str6, str7, str8),
        m_id(id) { setText(8, str9); }

    int getId() const { return m_id; }

protected:

    int     m_id;
    QString m_string9;
};


}

#endif
