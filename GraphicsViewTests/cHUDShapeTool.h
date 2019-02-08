#pragma once

#include "cHUDObject.h"

#include "ShapeBase.h"

class cHUDShapeTool :
    public cHUDObject
{
public:
    ~cHUDShapeTool();
    cHUDShapeTool( cHUDView* iParentView, cHUDObject* iParentObject, cShapeBase* iShapeTool );

public:
    virtual  void Draw( QPainter* iPainter ) override;
    virtual  bool  Event( QEvent* iEvent ) override;

public:
    void ShapeChanged( cBaseData* sender, int args );

private:
    void _Layout();

private:
    cShapeBase*     mShape;
    unsigned int mCBHandle;
};

