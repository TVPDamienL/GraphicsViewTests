#include "cHUDView.h"

#include "cHUDTransform.h"

cHUDView::~cHUDView()
{
}


cHUDView::cHUDView( QWidget* iParent ) :
    QWidget( iParent )
{
    auto transform = new cHUDTransform();
    transform->SetFrame( QRect( 100, 100, 100, 100 ) );
    mHUDObjects.push_back( transform );
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


