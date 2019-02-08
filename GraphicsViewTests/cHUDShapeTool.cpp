#include "cHUDShapeTool.h"

#include "cHUDView.h"
#include "cHUDHandle.h"

#include <QPainter>


cHUDShapeTool::~cHUDShapeTool()
{
    mShape->UnregisterEditionCallback( mCBHandle );
}


cHUDShapeTool::cHUDShapeTool( cHUDView* iParentView, cHUDObject* iParentObject, cShapeBase* iShapeTool ) :
    cHUDObject( iParentView, iParentObject )
{
    mShape = iShapeTool;
    mCBHandle = mShape->RegisterEditionCallback( [ this ]( cBaseData* sender, int iArg )
    {
        this->ShapeChanged( sender, iArg );
    });

    // FUTURE: Useless for now, as shape draw on mouse release atm, but as soon as it's done, just uncomment
    //for( auto point : mShape->Polygon() )
    //{
    //    mChildrenHUDs.push_back( new cHUDHandle( iParentView, this ) );
    //}
}


void
cHUDShapeTool::Draw( QPainter* iPainter )
{
    for( auto handle : mChildrenHUDs )
    {
        handle->Draw( iPainter );
    }

    QPen pen( Qt::red );
    pen.setWidth( 1 );
    iPainter->setPen( pen );

    iPainter->drawPolygon( GetFinalTransform().map( mShape->Polygon() ) );
}


bool
cHUDShapeTool::Event( QEvent * iEvent )
{
    // Let parent handle it
    return  false;
}


void
cHUDShapeTool::ShapeChanged( cBaseData * sender, int args )
{
    if( args == cShapeBase::eMessage::kShapeChanged )
    {
        mParentView->update();
        // FUTURE: uncomment
        //_Layout();
    }
    else if( args == cShapeBase::eMessage::kShapeStarted )
    {
        mVisible = true;
    }
    else if( args == cShapeBase::eMessage::kShapeFinished )
    {
        mVisible = false;
    }
}


void
cHUDShapeTool::_Layout()
{
    QPolygon finalP = GetFinalTransform().map( mShape->Polygon() );
    const QTransform fTrans = GetFinalTransform();
    int index = 0;
    for( auto point : mShape->Polygon() )
    {
        const QPoint mapped = point;
        mChildrenHUDs[ index ]->SetFrame( QRect( mapped.x() - 5, mapped.y() - 5, 10, 10 ) );
        ++index;
    }
}

