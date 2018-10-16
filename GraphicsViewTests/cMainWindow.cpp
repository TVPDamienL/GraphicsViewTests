#include "cMainWindow.h"

#include "cGraphicItem.h"
#include <QGraphicsPixmapItem >
#include <QColorDialog >


#include "ColorSwatch.h"

cMainWindow::cMainWindow(QWidget *parent) :
    QMainWindow(parent),
    mToolModel( new cToolModel() )
{
    ui.setupUi(this);

    mCurrentFrame = 0;
    emit  currentFrameChangeAsked( mCurrentFrame );

    mAnimationTimer = new QTimer();
    mAnimationTimer->start( 1000 / 24 );

    mMapper = new QDataWidgetMapper( this );
    mMapper->setModel( mToolModel );
    mMapper->setOrientation( Qt::Vertical );
    mMapper->addMapping( ui.penSizeSpinBox, 0 );
    mMapper->addMapping( ui.antiAliasCheckBox, 1 );

    mMapper->toFirst();

    ui.colorSwatch->SetColor( mToolModel->getColor() );

    ui.canvas->SetToolModel( mToolModel );

    connect( ui.playButton, &QPushButton::clicked, this, &cMainWindow::PlayPressed );
    connect( ui.stopButton, &QPushButton::clicked, this, &cMainWindow::StopPressed );
    connect( this, &cMainWindow::currentFrameChangeAsked, ui.graphicsView, &cCustomGraphicsView::CurrentFrameChanged );
    connect( ui.graphicsView, &cCustomGraphicsView::currentFrameChanged, this, &cMainWindow::CurrentFrameChanged );
    connect( ui.canvas, &cCanvas::currentFrameGotPainted, ui.graphicsView, &cCustomGraphicsView::CurrentFrameGotPainted );

    connect( ui.colorSwatch, &ColorSwatch::swatchClicked, this, &cMainWindow::AskColor );

    connect( mToolModel, &QAbstractItemModel::dataChanged, this, &cMainWindow::toolDataChanged );

    CurrentFrameChanged( 0 );
}


void
cMainWindow::TimerTick()
{
    int animationCount = ui.graphicsView->GetAnimationImages().size();
    emit  currentFrameChangeAsked( ++mCurrentFrame % animationCount );
}


void
cMainWindow::PlayPressed()
{
    if( ui.playButton->text() == "Play" )
    {
        connect( mAnimationTimer, &QTimer::timeout, this, &cMainWindow::TimerTick );
        ui.playButton->setText( "Pause" );
        mAnimationTimer->start( 1000/24 );
    }
    else
    {
        disconnect( mAnimationTimer, &QTimer::timeout, this, &cMainWindow::TimerTick );
        ui.playButton->setText( "Play" );
        mAnimationTimer->stop();
    }
}


void
cMainWindow::StopPressed()
{
    disconnect( mAnimationTimer, &QTimer::timeout, this, &cMainWindow::TimerTick );
    ui.playButton->setText( "Play" );
    mAnimationTimer->stop();
    emit  currentFrameChangeAsked( 0 );
}


void
cMainWindow::CurrentFrameChanged( int iNewIndex )
{
    if( iNewIndex == -1 )
        return;

    mCurrentFrame = iNewIndex;
    auto currentItem = ui.graphicsView->GetAnimationImages().at( mCurrentFrame );
    ui.canvas->SetPixmap( currentItem->pixmap() );
}


void
cMainWindow::UpdateColor()
{
    ui.colorSwatch->SetColor( mToolModel->getColor() );
}


void
cMainWindow::AskColor()
{
    QColorDialog dialog( mToolModel->getColor(), this );
    if( dialog.exec() )
    {
        mToolModel->setColor( dialog.selectedColor() );
        UpdateColor();
    }
}


void
cMainWindow::toolDataChanged( const QModelIndex & iLeft, const QModelIndex & iRight, const QVector<int>& iRoles )
{
    if( iLeft.row() == 2 )
        UpdateColor();
}
