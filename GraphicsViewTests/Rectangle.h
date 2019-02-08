#pragma once

#include "ToolBase.h"
#include "PaintToolBase.h"

class cShapeRectangle :
    public ToolBase
{
    Q_OBJECT

public:
    ~cShapeRectangle();
    cShapeRectangle( QObject* iParent = Q_NULLPTR );

public:
    // Overrides
    virtual  void  StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect  MoveDrawing( sPointData iPointData ) override;
    virtual  QRect EndDrawing( sPointData iPointData ) override;

public:
    void    SetPaintTool( cPaintToolBase* iTool );


private:
    QPoint mStartingPoint;
    QPoint mEndingPoint;

    cPaintToolBase* mPaintTool;
};

