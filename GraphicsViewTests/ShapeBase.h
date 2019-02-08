#pragma once

#include "ToolBase.h"

#include <QPolygon>

class cPaintToolBase;

class cShapeBase :
    public ToolBase
{
public:
    enum eMessage : int
    {
        kShapeStarted,
        kShapeChanged,
        kShapeFinished
    };

public:
    ~cShapeBase();
    cShapeBase( QObject* iParent = Q_NULLPTR );

public:
    void        SetPaintTool( cPaintToolBase* iTool );
    QPolygon    Polygon() const;

protected:
    QPolygon        mPolygon;
    cPaintToolBase* mPaintTool;
};