#include "Rectangle.h"



cShapeRectangle::~cShapeRectangle()
{
}


cShapeRectangle::cShapeRectangle( QObject* iParent ) :
    ToolBase( iParent )
{
}


void
cShapeRectangle::StartDrawing( QImage * iImage, sPointData iPointData )
{
    mStartingPoint = iPointData.mPosition;
    mDrawingContext = iImage;
}


QRect
cShapeRectangle::MoveDrawing( sPointData iPointData )
{
    mEndingPoint = iPointData.mPosition;
    return  QRect(); // Doesn't dirty anything
}


QRect
cShapeRectangle::EndDrawing( sPointData iPointData )
{
    sPointData p1;
    p1.mPosition = mStartingPoint;
    p1.mPressure = 1.0;
    p1.mRotation = 0.0;

    sPointData p2;
    p2.mPosition = QPoint( mEndingPoint.x(), mStartingPoint.y() );
    p2.mPressure = 1.0;
    p2.mRotation = 0.0;

    sPointData p3;
    p3.mPosition = mEndingPoint;
    p3.mPressure = 1.0;
    p3.mRotation = 0.0;

    sPointData p4;
    p4.mPosition = QPoint( mStartingPoint.x(), mEndingPoint.y() );
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


void
cShapeRectangle::SetPaintTool( cPaintToolBase * iTool )
{
    mPaintTool = iTool;
}
