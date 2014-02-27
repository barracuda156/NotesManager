/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


#include "ButtonSelectionTableSizes.h"

#include <QMenu>
#include <QVBoxLayout>


ButtonSelectionTableSizes::ButtonSelectionTableSizes( QWidget * parent )
    : QToolButton( parent )
{    
    setPopupMode( QToolButton::InstantPopup );

    QFont font = label.font();
    font.setBold( true );
    label.setFont( font );

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addWidget( &label );
    layout->addWidget( &widgetSelectionTableSizes );
    layout->addStretch();

    QMenu * menu = new QMenu();
    menu->setLayout( layout );

    setMenu( menu );

    connect( &widgetSelectionTableSizes, SIGNAL( changeSelected(int,int) ), SLOT( changeSelected( int, int ) ) );
    connect( &widgetSelectionTableSizes, SIGNAL( selected(int,int) ), SIGNAL( selected( int, int ) ) );

    changeSelected( -1, -1 );
}


void ButtonSelectionTableSizes::changeSelected( int rows, int cols )
{
    if ( rows > 0 && cols > 0 )
        label.setText( tr( "%1x%2 table" ).arg( rows ).arg( cols ) );
    else
        label.setText( tr( "Insert table" ) );
}
