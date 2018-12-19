#pragma once


#include "ToolBase.h"
#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QPen>

#include <filesystem>


class cToolModel :
    public ToolBase
{
public:
    virtual ~cToolModel();
    cToolModel( QObject* iParent = Q_NULLPTR );

public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;

    int             getSize() const;
    void            setSize( int iSize );

    QColor          getColor() const;
    void            setColor( const QColor& iColor );

    float           getStep() const;
    void            setStep( float iStep );


    void            buildTool();


    virtual  void  DrawDot( QImage* iImage, int x, int y ) override;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) override;
    virtual  void  DrawPath( QImage* iImage );

    QPoint  __DrawDotVectorTruc_( QImage* iImage, const QPoint& iStart, const QPointF& iVector, float iAmount );


private:
    // Tools
    int             mToolSize;
    QColor          mColor;
    float           mStep;
};

