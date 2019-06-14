#include "Polygon.h"

#include "PaintToolBase.h"
#include "Image.UtilitiesMultiThreaded.h"

#include "ImageDebugger.h"

cShapePolygon::~cShapePolygon()
{
}


cShapePolygon::cShapePolygon( QObject * iParent ) :
    cShapeBase( iParent )
{
}


void
cShapePolygon::StartDrawing( QImage * iImage, sPointData iPointData )
{
    mBBox = QRect( 0, 0, 0, 0 );
    mWorkingBuffer = new QImage( iImage->width() + 2, iImage->height() + 2, QImage::Format_ARGB32_Premultiplied );
    mPaintTool->StartDrawing( iImage, iPointData );
}


QRect
cShapePolygon::MoveDrawing( sPointData iPointData )
{
    QRect dirty = mPaintTool->MoveDrawing( iPointData );
    mBBox = mBBox.united( dirty );
    return  dirty;
}


QRect
cShapePolygon::EndDrawing( sPointData iPointData )
{
    mPaintTool->EndDrawing( iPointData );

    auto path = mPaintTool->Path();
    mPaintTool->PathAddPoint( path[0] );
    mPaintTool->DrawPathFromLastRenderedPoint();

    QRect dirty = mPaintTool->GetDirtyArea();
    mBBox = mBBox.united( dirty );
    MTBlendImageNone( mPaintTool->DrawingContext(), mBBox, mWorkingBuffer, mBBox.topLeft() + QPoint( 1, 1 ) );

    return  mBBox;
}


void
cShapePolygon::CancelDrawing()
{
    mPaintTool->CancelDrawing();
}


void
cShapePolygon::SetMode( eMode iMode )
{
    mMode = iMode;
}
