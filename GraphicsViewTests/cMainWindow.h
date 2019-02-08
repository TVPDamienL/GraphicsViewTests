#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_cMainWindow.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QTimer>
#include <QDataWidgetMapper>

#include "PaintToolBase.h"
#include "Clip.h"

class cCustomGraphicsView;
class cShapeRectangle;

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
    void currentFrameChangeAsked( int iCurrentFrame );

public slots:
    void sizeChanged( int );
    void stepChanged( int );
    void UpdateUI();
    void ToolPaintClicked();
    void ToolSelectClicked();
    void ToolRectangleClicked();

private:
    Ui::cMainWindow     ui;

    QGraphicsScene*     mScene;

    int                 mCurrentFrame;
    QTimer*             mAnimationTimer;

    QDataWidgetMapper*  mMapper;
    cPaintToolBase*     mToolPaint;
    cPaintToolBase*     mToolSelect;
    cShapeRectangle*    mRectangleShape;
    cShapeRectangle*    mRectangleSelection;

    cClip*              mClip;
};
