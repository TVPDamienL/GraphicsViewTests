#pragma once


#include "PaintToolBase.h"
#include "CurveBase.h"

#include <QColor>
#include <qobject.h>

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
    virtual  void  DrawLine( const QPoint& iP1, const QPoint& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 ) override;
    virtual  QRect  EndDrawing( sPointData iPointData ) override;


protected:
    virtual  float   _GetStepInPixelValue() const;


private:
    void _DrawDot( QImage* iImage, int x, int y, float iPressure, float iRotation );
    void _DrawDotF( int x, int y, float iPressure, float iRotation );
    void _BuildMipMap();


private:
    cCurveBase< float > mProfile;
    bool                mApplyProfile;
    QImage*             mTipRendered = 0;

    QVector< QImage* > mMipMap;
    QVector< const float* >  mMipMapF;

    // Float images
    float*                  mTipRenderedF = 0;

    float               _mToolSizeAfterPressure;
};

