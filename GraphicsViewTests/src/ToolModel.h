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
    Q_OBJECT

public:
    virtual ~cToolModel();
    cToolModel( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;

private:
    void    _BuildTool();


public:
    // Overrides
    virtual  void  DrawDot( int x, int y, float iPressure, float iRotation ) override;
    virtual  void  DrawLine( int x1, int y1, int x2, int y2 ) override;


private:
    // Tools
};

