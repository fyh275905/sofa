/******************************************************************************
 *       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
 *                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program; if not, write to the Free Software Foundation, Inc., 51  *
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
 *******************************************************************************
 *                            SOFA :: Applications                             *
 *                                                                             *
 * Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
 * H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
 * M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
 *                                                                             *
 * Contact information: contact@sofa-framework.org                             *
 ******************************************************************************/
#include "WindowVisitor.h"

#ifdef SOFA_QT4
#include <Q3Header>
#include <Q3PopupMenu>
#include <QMessageBox>
#else
#include <qheader.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#endif


#ifndef SOFA_QT4
typedef QPopupMenu Q3PopupMenu;
typedef QListViewItem Q3ListViewItem;
#endif
namespace sofa
{

namespace gui
{

namespace qt
{
QPixmap *WindowVisitor::icons[WindowVisitor::OTHER+1];
WindowVisitor::WindowVisitor()
{

#ifdef SOFA_QT4
    connect(graphView, SIGNAL(rightButtonClicked ( Q3ListViewItem *, const QPoint &, int )),  this, SLOT( rightClick(Q3ListViewItem *, const QPoint &, int )));
#else
    connect(graphView, SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &, int )),  this, SLOT( rightClick(QListViewItem *, const QPoint &, int )));
#endif


    QImage * img[OTHER+1];
    img[NODE] = new QImage(10,10,32);


    img[NODE]->setAlphaBuffer(true);
    img[NODE]->fill(qRgba(0,0,0,0));
    // Workaround for qt 3.x where fill() does not set the alpha channel
    for (int y=0 ; y < 10 ; y++)
        for (int x=0 ; x < 10 ; x++)
            img[NODE]->setPixel(x,y,qRgba(0,0,0,0));

    for (int y=0 ; y < 10 ; y++)
        img[NODE]->setPixel(0,y,qRgba(0,0,0,255));


    img[OTHER] = new QImage(img[NODE]->copy());

    //BORDER!!!!-------------------------------------
    for (int x=1; x <10 ; x++)
    {
        img[NODE]   ->setPixel(x,0,qRgba(0,0,0,255));
        img[NODE]   ->setPixel(x,9,qRgba(0,0,0,255));
    }
    for (int y=0 ; y < 10 ; y++)
    {
        img[NODE]   ->setPixel(0,y,qRgba(0,0,0,255));
        img[NODE]   ->setPixel(9,y,qRgba(0,0,0,255));
    }
    //-----------------------------------------------

    img[COMMENT] = new QImage(img[NODE]->copy());
    img[COMPONENT] = new QImage(img[NODE]->copy());

    for (int y=0 ; y < 9 ; y++)
        for (int x=0 ; x < 9 ; x++)
        {
            img[NODE]   ->setPixel(x,y,qRgba(125,125,125,255));
            img[COMMENT]->setPixel(x,y,qRgba(255,255,0,255));
            img[COMPONENT]->setPixel(x,y,qRgba(255,0,255,255));
        }

    for (int y=0 ; y < 5 ; y++)
        for (int x=0 ; x < 2*y+1 ; x++)
        {
            img[OTHER]->setPixel(x,y,qRgba(0,0,255,255));
        }
    for (int y=5 ; y < 10 ; y++)
        for (int x=0 ; x < 2*(10-y) ; x++)
        {
            img[OTHER]   ->setPixel(x,y,qRgba(0,0,255,255));
        }


    icons[NODE]    = new QPixmap(*img[NODE]   );
    icons[COMMENT] = new QPixmap(*img[COMMENT]);
    icons[COMPONENT] = new QPixmap(*img[COMPONENT]);
    icons[OTHER]   = new QPixmap(*img[OTHER]  );

}

void WindowVisitor::rightClick(Q3ListViewItem *item, const QPoint &point, int index)
{
    if (!item) return;

    Q3PopupMenu *contextMenu = new Q3PopupMenu ( this, "ContextMenu" );
    if(item->childCount())
    {
        contextMenu->insertItem("Collapse", this, SLOT( collapseNode()));
        contextMenu->insertItem("Expand"  , this, SLOT( expandNode()));

        contextMenu->popup ( point, index );
    }
}


void WindowVisitor::expandNode()
{
    expandNode(graphView->currentItem());
}

void WindowVisitor::expandNode(Q3ListViewItem* item)
{
    if (!item) return;

    item->setOpen ( true );
    if ( item != NULL )
    {
        Q3ListViewItem* child;
        child = item->firstChild();
        while ( child != NULL )
        {
            item = child;
            child->setOpen ( true );
            expandNode(item);
            child = child->nextSibling();
        }
    }
}

void WindowVisitor::collapseNode()
{
    collapseNode(graphView->currentItem());
}
void WindowVisitor::collapseNode(Q3ListViewItem* item)
{
    if (!item) return;

    Q3ListViewItem* child;
    child = item->firstChild();
    while ( child != NULL )
    {
        child->setOpen ( false );
        child = child->nextSibling();
    }
    item->setOpen ( true );
}

}
}
}
