#pragma once


#include "ToolBase.h"
#include "CurveBase.h"

#include <QColor>


class cToolSimpleBrush :
    public ToolBase
{
public:
    virtual ~cToolSimpleBrush();
    cToolSimpleBrush( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;

    QColor          getColor() const;
    void            setColor( const QColor& iColor );

    void            buildTool();

private:
    void  _DrawPixel( uchar* iData, unsigned int iImageWidth, unsigned int iImageHeight,  int iX, int iY, int iR, int iG, int iB, int iA );

public:
    // Overrides
    virtual  void  DrawDot( QImage* iImage, int x, int y ) override;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) override;


private:
    cCurveBase< float > mProfile;
};

