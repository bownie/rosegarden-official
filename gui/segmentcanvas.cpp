
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

#include <qpopupmenu.h>

#include <klocale.h>
#include <kmessagebox.h>

#include "trackscanvas.h"
#include "Track.h"

#include "rosedebug.h"

using Rosegarden::Track;

//////////////////////////////////////////////////////////////////////
//                TrackItem
//////////////////////////////////////////////////////////////////////

TrackItem::TrackItem(QCanvas* canvas)
    : QCanvasRectangle(canvas),
      m_track(0)
{
}

TrackItem::TrackItem(const QRect &r, QCanvas* canvas)
    : QCanvasRectangle(r, canvas),
      m_track(0)
{
}

TrackItem::TrackItem(int x, int y,
                             int width, int height,
                             QCanvas* canvas)
    : QCanvasRectangle(x, y, width, height, canvas),
      m_track(0)
{
}

unsigned int TrackItem::getLength() const
{
    return rect().width() / m_widthToLengthRatio;
}

unsigned int TrackItem::getStartIndex() const
{
    return rect().x() / m_widthToLengthRatio;
}

void TrackItem::setWidthToLengthRatio(unsigned int r)
{
    m_widthToLengthRatio = r;
}

int TrackItem::getInstrument() const
{
    return m_track->getInstrument();
}

void TrackItem::setInstrument(int i)
{
    m_track->setInstrument(i);
}


unsigned int TrackItem::m_widthToLengthRatio = 1;


//////////////////////////////////////////////////////////////////////
//                TracksCanvas
//////////////////////////////////////////////////////////////////////


TracksCanvas::TracksCanvas(int gridH, int gridV,
                           QCanvas& c, QWidget* parent,
                           const char* name, WFlags f) :
    QCanvasView(&c,parent,name,f),
    m_toolType(Pencil),
    m_tool(new TrackPencil(this)),
    m_grid(gridH, gridV),
    m_brush(Qt::blue),
    m_pen(Qt::black),
    m_editMenu(new QPopupMenu(this))
{
    TrackItem::setWidthToLengthRatio(m_grid.hstep());

    m_editMenu->insertItem(I18N_NOOP("Edit"),
                           this, SLOT(onEdit()));
    m_editMenu->insertItem(I18N_NOOP("Edit Small"),
                           this, SLOT(onEditSmall()));
}

TracksCanvas::~TracksCanvas()
{
}

void
TracksCanvas::update()
{
    canvas()->update();
}

void
TracksCanvas::setTool(ToolType t)
{
    if (t == m_toolType) return;

    delete m_tool;
    m_tool = 0;
    m_toolType = t;

    switch(t) {
    case Pencil:
        m_tool = new TrackPencil(this);
        break;
    case Eraser:
        m_tool = new TrackEraser(this);
        break;
    case Mover:
        m_tool = new TrackMover(this);
        break;
    case Resizer:
        m_tool = new TrackResizer(this);
        break;
    default:
        KMessageBox::error(0, QString("TracksCanvas::setTool() : unknown tool id %1").arg(t));
    }
}

TrackItem*
TracksCanvas::findPartClickedOn(QPoint pos)
{
    QCanvasItemList l=canvas()->collisions(pos);

    if (l.count()) {

        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            if (TrackItem *item = dynamic_cast<TrackItem*>(*it))
                return item;
        }

    }

    return 0;
}

void
TracksCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    if (e->button() == LeftButton) { // delegate event handling to tool

        m_tool->handleMouseButtonPress(e);

    } else if (e->button() == RightButton) { // popup menu if over a part

        TrackItem *item = findPartClickedOn(e->pos());

        if (item) {
            m_currentItem = item;
//             kdDebug(KDEBUG_AREA) << "TracksCanvas::contentsMousePressEvent() : edit m_currentItem = "
//                                  << m_currentItem << endl;

            m_editMenu->exec(QCursor::pos());
        }
    }
}

void TracksCanvas::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == LeftButton) m_tool->handleMouseButtonRelase(e);
}

void TracksCanvas::contentsMouseMoveEvent(QMouseEvent* e)
{
    m_tool->handleMouseMove(e);
}

void
TracksCanvas::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}


void TracksCanvas::clear()
{
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
	if ( *it )
	    delete *it;
    }
}

/// called when reading a music file
TrackItem*
TracksCanvas::addPartItem(int x, int y, unsigned int nbBars)
{
    TrackItem* newPartItem = new TrackItem(x, y,
                                                   gridHStep() * nbBars,
                                                   grid().vstep(),
                                                   canvas());
    newPartItem->setPen(m_pen);
    newPartItem->setBrush(m_brush);
    newPartItem->setVisible(true);     

    return newPartItem;
}


void
TracksCanvas::onEdit()
{
    emit editTrack(m_currentItem->getTrack());
}

void
TracksCanvas::onEditSmall()
{
    emit editTrackSmall(m_currentItem->getTrack());
}

//////////////////////////////////////////////////////////////////////
//                 Track Tools
//////////////////////////////////////////////////////////////////////

TrackTool::TrackTool(TracksCanvas* canvas)
    : m_canvas(canvas),
      m_currentItem(0)
{
}

TrackTool::~TrackTool()
{
    m_canvas->setCursor(Qt::arrowCursor);
}

//////////////////////////////
// TrackPencil
//////////////////////////////

TrackPencil::TrackPencil(TracksCanvas *c)
    : TrackTool(c),
      m_newRect(false)
{
    connect(this, SIGNAL(addTrack(TrackItem*)),
            c,    SIGNAL(addTrack(TrackItem*)));
    connect(this, SIGNAL(deleteTrack(Rosegarden::Track*)),
            c,    SIGNAL(deleteTrack(Rosegarden::Track*)));
    connect(this, SIGNAL(resizeTrack(Rosegarden::Track*)),
            c,    SIGNAL(resizeTrack(Rosegarden::Track*)));

    kdDebug(KDEBUG_AREA) << "TrackPencil()\n";
}

void TrackPencil::handleMouseButtonPress(QMouseEvent *e)
{
    m_newRect = false;
    m_currentItem = 0;

    // Check if we're clicking on a rect
    //
    TrackItem *item = m_canvas->findPartClickedOn(e->pos());

    if (item) {
        // we are, so set currentItem to it
        m_currentItem = item;
        return;

    } else { // we are not, so create one

        int gx = m_canvas->grid().snapX(e->pos().x()),
            gy = m_canvas->grid().snapY(e->pos().y());

        m_currentItem = new TrackItem(gx, gy,
                                          m_canvas->grid().hstep(),
                                          m_canvas->grid().vstep(),
                                          m_canvas->canvas());
        
        m_currentItem->setPen(m_canvas->pen());
        m_currentItem->setBrush(m_canvas->brush());
        m_currentItem->setVisible(true);

        m_newRect = true;

        m_canvas->update();
    }

}

void TrackPencil::handleMouseButtonRelase(QMouseEvent*)
{
    if (!m_currentItem) return;

    // TODO : this belongs to a resizer tool
    if (m_currentItem->width() == 0) {
        kdDebug(KDEBUG_AREA) << "TracksCanvas::contentsMouseReleaseEvent() : rect deleted"
                             << endl;
        emit deleteTrack(m_currentItem->getTrack());
        m_canvas->canvas()->update();
        m_currentItem = 0;
    }

    if (m_newRect) {

        emit addTrack(m_currentItem);

    } else {
        kdDebug(KDEBUG_AREA) << "TracksCanvas::contentsMouseReleaseEvent() : shorten m_currentItem = "
                             << m_currentItem << endl;
        // readjust size of corresponding track
        emit resizeTrack(m_currentItem->getTrack());
    }

    m_currentItem = 0;
}

void TrackPencil::handleMouseMove(QMouseEvent *e)
{
    if (m_currentItem) {

	m_currentItem->setSize(m_canvas->grid().snapX(e->pos().x()) - m_currentItem->rect().x(),
                               m_currentItem->rect().height());
	m_canvas->canvas()->update();
    }
}

//////////////////////////////
// TrackEraser
//////////////////////////////

TrackEraser::TrackEraser(TracksCanvas *c)
    : TrackTool(c)
{
    m_canvas->setCursor(Qt::crossCursor);

    connect(this, SIGNAL(deleteTrack(Rosegarden::Track*)),
            c,    SIGNAL(deleteTrack(Rosegarden::Track*)));

    kdDebug(KDEBUG_AREA) << "TrackEraser()\n";
}

void TrackEraser::handleMouseButtonPress(QMouseEvent *e)
{
    m_currentItem = m_canvas->findPartClickedOn(e->pos());
}

void TrackEraser::handleMouseButtonRelase(QMouseEvent*)
{
    if (m_currentItem) emit deleteTrack(m_currentItem->getTrack());
    delete m_currentItem;
    m_canvas->canvas()->update();
    
    m_currentItem = 0;
}

void TrackEraser::handleMouseMove(QMouseEvent*)
{
}

//////////////////////////////
// TrackMover
//////////////////////////////

TrackMover::TrackMover(TracksCanvas *c)
    : TrackTool(c)
{
    m_canvas->setCursor(Qt::pointingHandCursor);

    connect(this, SIGNAL(updateTrackInstrument(TrackItem*)),
            c,    SIGNAL(updateTrackInstrument(TrackItem*)));

    kdDebug(KDEBUG_AREA) << "TrackMover()\n";
}

void TrackMover::handleMouseButtonPress(QMouseEvent *e)
{
    TrackItem *item = m_canvas->findPartClickedOn(e->pos());

    if (item) {
        m_currentItem = item;
        return;
    }
}

void TrackMover::handleMouseButtonRelase(QMouseEvent*)
{
    if (m_currentItem) {
        m_currentItem->getTrack()->setStartIndex(m_currentItem->x() / m_canvas->grid().hstep());
        kdDebug(KDEBUG_AREA) << "TrackMover::handleMouseButtonRelase() : set part start time to "
                             << m_currentItem->getTrack()->getStartIndex() << endl;
        emit updateTrackInstrument(m_currentItem);
    }

    m_currentItem = 0;
}

void TrackMover::handleMouseMove(QMouseEvent *e)
{
    if (m_currentItem) {
        m_currentItem->setX(m_canvas->grid().snapX(e->pos().x()));
        m_currentItem->setY(m_canvas->grid().snapY(e->pos().y()));
        m_canvas->canvas()->update();
    }
}

//////////////////////////////
// TrackResizer
//////////////////////////////

TrackResizer::TrackResizer(TracksCanvas *c)
    : TrackTool(c),
      m_edgeThreshold(10)
{
    m_canvas->setCursor(Qt::sizeHorCursor);

    connect(this, SIGNAL(deleteTrack(Rosegarden::Track*)),
            c,    SIGNAL(deleteTrack(Rosegarden::Track*)));
    connect(this, SIGNAL(resizeTrack(Rosegarden::Track*)),
            c,    SIGNAL(resizeTrack(Rosegarden::Track*)));

    kdDebug(KDEBUG_AREA) << "TrackResizer()\n";
}

void TrackResizer::handleMouseButtonPress(QMouseEvent *e)
{
    TrackItem* item = m_canvas->findPartClickedOn(e->pos());

    if (item && cursorIsCloseEnoughToEdge(item, e)) {
        m_currentItem = item;
    }
}

void TrackResizer::handleMouseButtonRelase(QMouseEvent*)
{
    m_currentItem = 0;
}

void TrackResizer::handleMouseMove(QMouseEvent *e)
{
    if (!m_currentItem) return;

    // change width only

    m_currentItem->setSize(m_canvas->grid().snapX(e->pos().x()) - m_currentItem->rect().x(),
                           m_currentItem->rect().height());
    
    m_canvas->canvas()->update();
    
}

bool TrackResizer::cursorIsCloseEnoughToEdge(TrackItem* p, QMouseEvent* e)
{
    return ( abs(p->rect().x() + p->rect().width() - e->x()) < m_edgeThreshold);
}
