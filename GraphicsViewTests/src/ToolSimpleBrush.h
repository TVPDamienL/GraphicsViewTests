#pragma once


#include "PaintToolBase.h"
#include "CurveBase.h"

#include <QColor>

class cToolSimpleBrush :
    public cPaintToolBase
{
    Q_OBJECT

public:
    virtual ~cToolSimpleBrush();
    cToolSimpleBrush( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    void  buildTool();


public:
    void  ApplyProfile( bool iApply );
    bool  ApplyProfile() const;


public:
    // Overrides
    virtual  void  StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect MoveDrawing( sPointData iPointData ) override;
    virtual  void  DrawDot( int x, int y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( int x1, int y1, int x2, int y2 ) override;


protected:
    virtual  float   _GetStepInPixelValue() const;


private:
    void _DrawDot( QImage* iImage, int x, int y, float iPressure, float iRotation );


private:
    cCurveBase< float > mProfile;
    bool                mApplyProfile;
    QImage*             mTipRendered;

    float               _mToolSizeAfterPressure;
};

