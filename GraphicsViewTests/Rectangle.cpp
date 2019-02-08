#include "Rectangle.h"

#include "PaintToolBase.h"
#include "Image.Utilities.h"

#include "qcolor.h"    // for QColor
#include "ToolBase.h"  // for sPointData


cShapeRectangle::~cShapeRectangle()
{
}


cShapeRectangle::cShapeRectangle( QObject* iParent ) :
    cShapeBase( iParent )
{
    mPolygon.append( QPoint( 0, 0 ) );
    mPolygon.append( QPoint( 0, 0 ) );
    mPolygon.append( QPoint( 0, 0 ) );
    mPolygon.append( QPoint( 0, 0 ) );
}


void
cShapeRectangle::StartDrawing( QImage * iImage, sPointData iPointData )
{
    mPolygon.setPoint( 0, iPointData.mPosition );
    mPolygon.setPoint( 1, iPointData.mPosition );
    mPolygon.setPoint( 2, iPointData.mPosition );
    mPolygon.setPoint( 3, iPointData.mPosition );
    mDrawingContext = iImage;
}


QRect
cShapeRectangle::MoveDrawing( sPointData iPointData )
{
    mPolygon.setPoint( 2, iPointData.mPosition );

    mPolygon.setPoint( 1, mPolygon[ 2 ].x(), mPolygon[ 0 ].y() );
    mPolygon.setPoint( 3, mPolygon[ 0 ].x(), mPolygon[ 2 ].y() );

    EmitValueChanged( kShapeChanged );

    return  QRect(); // Doesn't dirty anything
}


QRect
cShapeRectangle::EndDrawing( sPointData iPointData )
{
    const QPoint start = mPolygon[ 0 ];
    const QPoint end = mPolygon[ 2 ];
    if( mMode == kStroke )
    {
        sPointData p1;
        p1.mPosition = start;
        p1.mPressure = 1.0;
        p1.mRotation = 0.0;

        sPointData p2;
        p2.mPosition = mPolygon[ 1 ];
        p2.mPressure = 1.0;
        p2.mRotation = 0.0;

        sPointData p3;
        p3.mPosition = end;
        p3.mPressure = 1.0;
        p3.mRotation = 0.0;

        sPointData p4;
        p4.mPosition = mPolygon[ 3 ];
        p4.mPressure = 1.0;
        p4.mRotation = 0.0;

        mPaintTool->StartDrawing( mDrawingContext, p1 );

        mPaintTool->PathAddPoint( p1 );
        mPaintTool->PathAddPoint( p2 );
        mPaintTool->PathAddPoint( p3 );
        mPaintTool->PathAddPoint( p4 );
        mPaintTool->PathAddPoint( p1 );

        mPaintTool->DrawFullPath();

        return  mPaintTool->EndDrawing( p4 );
    }
    else
    {
        const int xMin = start.x() > end.x() ? end.x() : start.x();
        const int xMax = start.x() < end.x() ? end.x() : start.x();
        const int yMin = start.y() > end.y() ? end.y() : start.y();
        const int yMax = start.y() < end.y() ? end.y() : start.y();


        QRect area( xMin, yMin, xMax - xMin, yMax - yMin );
        QColor color = mPaintTool->getColor();
        // Apply opacity to color

        NormalFill( mDrawingContext, area, color );
        return  area;
    }
}


void
cShapeRectangle::SetMode( eMode iMode )
{
    mMode = iMode;
}
