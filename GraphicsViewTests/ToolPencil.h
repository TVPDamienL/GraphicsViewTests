#pragma once


#include "PaintToolBase.h"
#include "CurveBase.h"

#include <QColor>
#include <qobject.h>

class cToolPencil :
    public cPaintToolBase
{
    Q_OBJECT

public:
    virtual ~cToolPencil();
    cToolPencil( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    void  buildTool();


public:
    // Overrides
    virtual  void  StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect MoveDrawing( sPointData iPointData ) override;

    virtual  void   DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 ) override;
    virtual  void   DrawPathFromLastRenderedPoint() override;


protected:
    virtual  float   _GetStepInPixelValue() const;


private:
    float               _mToolSizeAfterPressure;
};