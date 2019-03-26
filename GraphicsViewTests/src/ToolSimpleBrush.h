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
    virtual  int    getSize() const;
    virtual  void   setSize( int iSize );

    virtual  QColor getColor() const;
    virtual  void   setColor( const QColor& iColor );

    virtual  float  getStep() const;
    virtual  void   setStep( float iStep );

    virtual  float  getOpacity() const;
    virtual  void   setOpacity( float iOpacity );


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
    virtual  void   CancelDrawing() override;


protected:
    void            PrepareTool();


private:
    void _RenderTip( int x, int y, float iPressure, float iRotation );
    void _BuildMipMap();


private:
    cCurveBase< float > mProfile;
    bool                mApplyProfile;

    // Float images
    QVector< const float* >  mMipMapF;
    float*                  mTipRenderedF = 0;
    float*                  mDryBuffer = 0;
    float*                  mStampBuffer = 0;

    float               _mToolSizeAfterPressure;
};

