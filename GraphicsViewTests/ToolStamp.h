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
    virtual ~cToolStamp();
    cToolStamp( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    virtual  void  buildTool() = 0;

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


protected:
    virtual void    PrepareTool();


protected:
    virtual void    RenderTip( int x, int y ) = 0;
    void            _BuildMipMap();


protected:
    // Float images
    QVector< const float* > mMipMapF;
    float*                  mTipRenderedF = 0;
    float*                  mDryBuffer = 0;
    float*                  mStampBuffer = 0;

    float                   _mToolSizeAfterPressure;
};

