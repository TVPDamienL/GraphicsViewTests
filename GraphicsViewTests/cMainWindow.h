#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cMainWindow.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QTimer>
#include <QDataWidgetMapper>

#include "ToolModel.h"

class cCustomGraphicsView;

class cMainWindow :
    public QMainWindow
{
    Q_OBJECT

public:
    cMainWindow(QWidget *parent = Q_NULLPTR);

public:
    void TimerTick();
    void PlayPressed();
    void StopPressed();

    void CurrentFrameChanged( int iNewIndex );

    void UpdateColor();
    void AskColor();

    void toolDataChanged( const QModelIndex& iLeft, const QModelIndex& iRight, const QVector< int >& iRoles );

signals:
    void  currentFrameChangeAsked( int iCurrentFrame );

public slots:
    void sizeChanged( int );
    void stepChanged( int );
    void UpdateUI();

private:
    Ui::cMainWindow     ui;

    QGraphicsScene*     mScene;

    int                 mCurrentFrame;
    QTimer*             mAnimationTimer;

    QDataWidgetMapper*  mMapper;
    cToolModel*         mToolModel;

};
