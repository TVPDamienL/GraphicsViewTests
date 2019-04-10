#pragma once
#pragma once


#include "PaintToolBase.h"

#include <QColor>
#include <qobject.h>

class cToolStamp :
    public cPaintToolBase
{
    Q_OBJECT


public:
    enum eTipSwapStyle
    {
        kLinearLoop,
        kRandom,
        kPingPong
    };


public:
    virtual ~cToolStamp();
    cToolStamp( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    virtual  void   buildTool();

public:
    virtual  int    getSize() const;
    virtual  void   setSize( int iSize );

    virtual  QColor getColor() const;
    virtual  void   setColor( const QColor& iColor );

    virtual  float  getOpacity() const;
    virtual  void   setOpacity( float iOpacity );


public:
    // Overrides
    virtual  void   StartDrawing( QImage* iImage, sPointData iPointData ) override;
    virtual  QRect  MoveDrawing( sPointData iPointData ) override;
    virtual  void   DrawDot( float x, float y, float iPressure, float iRotation ) override;
    virtual  void   DrawLine( const QPointF& iP1, const QPointF& iP2, float iPressure1, float iRotation1, float iPressure2, float iRotation2 ) override;
    virtual  QRect  EndDrawing( sPointData iPointData ) override;
    virtual  void   CancelDrawing() override;

public:
    void AddTip( float* iTip );
    void SetTipSwapStyle( eTipSwapStyle iStyle );


protected:
    virtual void    ClearTool();
    virtual void    PrepareTool();


protected:
    virtual void    RenderTips( int x, int y ) = 0;

    void            ClearTips();
    void            _BuildMipMaps();
    void            _ClearMipMaps();


public:
    virtual void DEBUG() override;

protected:
    eTipSwapStyle                       mStyle = kLinearLoop;
    int                                 mCurrentTipIndex = 0;
    // Float images
    QVector< QVector< const float* > >  mMipMapF;
    QVector< float* >                   mTipRenderedF;
    float*                              mColorStampFOrigin = 0;
    float*                              mColorStampF = 0;

    float*                              mDryBuffer = 0;
    float*                              mStampBuffer = 0;
};

