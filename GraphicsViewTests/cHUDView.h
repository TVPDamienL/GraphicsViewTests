#pragma once

#include "cHUDObject.h"

#include <QWidget>


class cHUDView :
    public QWidget
{
    Q_OBJECT
public:
    ~cHUDView();
    cHUDView( QWidget* iParent = nullptr );

public:
    void paintEvent( QPaintEvent* iEvent ) override;


private:
    QVector< cHUDObject* > mHUDObjects;
};

