#pragma once

#include <QStandardItemModel>

class ToolBase :
    public QStandardItemModel
{
    Q_OBJECT

public:
    ToolBase(QObject *parent);
    ~ToolBase();

public:
    virtual  void  DrawDot( QImage* iImage, int x, int y ) = 0;
    virtual  void  DrawLine( QImage* iImage, int x1, int y1, int x2, int y2 ) = 0;
};
