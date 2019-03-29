#include "ShapeBase.h"


cShapeBase::~cShapeBase()
{
}


cShapeBase::cShapeBase( QObject * iParent ) :
    ToolBase( iParent )
{
}


void
cShapeBase::SetPaintTool( cPaintToolBase * iTool )
{
    mPaintTool = iTool;
}


QPolygonF
cShapeBase::Polygon() const
{
    return  mPolygon;
}
