#include "ToolBase.h"



ToolBase::ToolBase(QObject *parent)
    : QStandardItemModel(parent)
{
}


ToolBase::~ToolBase()
{
}


void
ToolBase::PathAddPoint( const QPoint & iPoint )
{
    mPath.push_back( iPoint );
}
