#include "ToolBase.h"

#include <QVector2D>
#include <QDebug>

ToolBase::~ToolBase()
{
}


ToolBase::ToolBase( QObject *parent ) :
    QStandardItemModel( parent )
{
}


QImage*
ToolBase::DrawingContext()
{
    return  mDrawingContext;
}


