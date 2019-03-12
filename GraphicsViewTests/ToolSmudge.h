#pragma once


#include "PaintToolBase.h"
#include "CurveBase.h"

#include <QColor>
#include <qobject.h>

class cToolSmudge :
    public cPaintToolBase
{
    Q_OBJECT

public:
    virtual ~cToolSmudge();
    cToolSmudge( QObject* iParent = Q_NULLPTR );

public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    void  buildTool();


public:
    // Overrides
    virtual  void  StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect MoveDrawing( sPointData iPointData ) override;
    virtual  void  DrawDot( int x, int y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 ) override;
    virtual  QRect  EndDrawing( sPointData iPointData ) override;


protected:
    virtual  float   _GetStepInPixelValue() const;


private:
    void _GenerateMask();
    void _SampleFromCanvas( int ix, int iy, int size );


private:
    cCurveBase< float > mProfile;
    float               mForce = 0.7;
    float*              mAlphaMask = 0;
    QImage*             mCanvasSample = 0;

    float               _mToolSizeAfterPressure;

    float*              _mFloatExtract = 0;
};

