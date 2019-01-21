#include "cHUDHandle.h"

#include <QPainter>



cHUDHandle::cHUDHandle()
{
}


void
cHUDHandle::Draw( QPainter* iPainter )
{
    QPen pen( Qt::red );
    pen.setWidth( 1 );

    iPainter->setPen( pen );
    iPainter->drawRect( 0, 0, mSize, mSize );
}


cHUDHandle::~cHUDHandle()
{
}
