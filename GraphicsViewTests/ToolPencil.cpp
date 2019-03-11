#include "ToolPencil.h"

#include <QDebug>

#include "BenchmarkStuff.h"
#include "Image.Utilities.h"
#include "Image.UtilitiesMultiThreaded.h"
#include "Blending.h"
#include "GPUForThisApp.h"


cToolPencil::~cToolPencil()
{
}


cToolPencil::cToolPencil( QObject * iParent ) :
    cPaintToolBase( iParent )
{
    // Some debug values to work with
    mToolSize = 10;
    mColor = Qt::red;
    mStep = 0.05;

    buildTool();
}


Qt::ItemFlags
cToolPencil::flags( const QModelIndex & iIndex ) const
{
    return  cPaintToolBase::flags( iIndex) | Qt::ItemIsEditable;
}


void
cToolPencil::buildTool()
{
    QStandardItem* sizeItem = new QStandardItem( "Size" );
    sizeItem->setData( mToolSize );
    setItem( 0, 0, sizeItem );
    QStandardItem* color = new QStandardItem( "Color" );
    color->setData( mColor );
    setItem( 1, 0, color );
    QStandardItem* step = new QStandardItem( "Step" );
    step->setData( mStep );
    setItem( 2, 0, step );
}


void
cToolPencil::StartDrawing( QImage* iImage, sPointData iPointData )
{
    cPaintToolBase::StartDrawing( iImage, iPointData );
    mDirtyArea = QRect( 0, 0, 0, 0 );
}


QRect
cToolPencil::MoveDrawing( sPointData iPointData )
{
    PathAddPoint( iPointData );
    DrawPathFromLastRenderedPoint();

    return  GetDirtyAreaAndReset();
}


void
cToolPencil::DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 )
{
}


void
cToolPencil::DrawPathFromLastRenderedPoint()
{
    if( mPath.size() <= 0 )
        return;

    for( mLastRenderedPathIndex; mLastRenderedPathIndex < mPath.size() - 1; ++mLastRenderedPathIndex )
    {
        // Setting base variables : starting point, ending point and their distance
        QPoint p1 = mPath[ mLastRenderedPathIndex ].mPosition;
        QPoint p2 = mPath[ mLastRenderedPathIndex + 1 ].mPosition;
        float  pressure_p1 = mPath[ mLastRenderedPathIndex ].mPressure;
        float  pressure_p2 = mPath[ mLastRenderedPathIndex + 1 ].mPressure;
        float  rotation_p1 = mPath[ mLastRenderedPathIndex ].mRotation;
        float  rotation_p2 = mPath[ mLastRenderedPathIndex + 1 ].mRotation;

        DrawLine( p1, p2, pressure_p1, rotation_p1, pressure_p2, rotation_p2 );
    }
}


float
cToolPencil::_GetStepInPixelValue() const
{
    return  std::max( mStep * _mToolSizeAfterPressure, 1.0F );
}





