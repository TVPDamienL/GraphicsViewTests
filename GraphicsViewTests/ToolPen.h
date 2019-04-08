#pragma once


#include "ToolStamp.h"
#include "CurveBase.h"

#include <QObject>

class cToolPen :
    public cToolStamp
{
    Q_OBJECT

public:
    virtual ~cToolPen();
    cToolPen( QObject* iParent = Q_NULLPTR );


public:
    Qt::ItemFlags   flags( const QModelIndex& iIndex ) const override;
    virtual void  buildTool() override;


protected:
    virtual void  RenderTips( int x, int y ) override;

private:
    cCurveBase< float > mProfile;
};

