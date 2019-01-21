#include "cHUDView.h"

#include "cHUDHandle.h"

cHUDView::~cHUDView()
{
}


cHUDView::cHUDView( QWidget* iParent ) :
    QWidget( iParent )
{
    mHUDObjects.push_back( new cHUDHandle() );
}


void
cHUDView::paintEvent( QPaintEvent * iEvent )
{
    QPainter painter( this );
    for( auto hud : mHUDObjects )
    {
        hud->Draw( &painter );
    }
}


