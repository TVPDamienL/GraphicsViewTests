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

private:
    void    _BuildTool();


public:
    // Overrides
    virtual  void  DrawDot( QImage* iImage, int x, int y ) override;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) override;


private:
    // Tools
};

