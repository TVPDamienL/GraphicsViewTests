#include "cMainWindow.h"

#include "cGraphicItem.h"
#include <QGraphicsPixmapItem>
#include <QColorDialog>

#include "ToolSimpleBrush.h"

#include "cLayer.h"

#include "ColorSwatch.h"

cMainWindow::cMainWindow(QWidget *parent) :
    QMainWindow(parent),
    mToolModel( new cToolSimpleBrush() )
{
    ui.setupUi(this);

    mCurrentFrame = 0;
    emit  currentFrameChangeAsked( mCurrentFrame );

    mAnimationTimer = new QTimer();
    mAnimationTimer->start( 1000 / 24 );


    mClip = new cClip( 1920, 1080 );
    mClip->AddLayer();

    //mMapper = new QDataWidgetMapper( this );
    //mMapper->setModel( mToolModel );
    //mMapper->setOrientation( Qt::Vertical );
    //mMapper->addMapping( ui.penSizeSpinBox, 0 );
    //mMapper->addMapping( ui.antiAliasCheckBox, 1 );

    //mMapper->toFirst();

    UpdateUI();

    ui.colorSwatch->SetColor( mToolModel->getColor() );

    ui.canvas->SetToolModel( mToolModel );

    connect( ui.playButton, &QPushButton::clicked, this, &cMainWindow::PlayPressed );
    connect( ui.stopButton, &QPushButton::clicked, this, &cMainWindow::StopPressed );
    connect( this, &cMainWindow::currentFrameChangeAsked, ui.graphicsView, &cCustomGraphicsView::CurrentFrameChanged );
    connect( ui.graphicsView, &cCustomGraphicsView::currentFrameChanged, this, &cMainWindow::CurrentFrameChanged );
    connect( ui.canvas, &cCanvas::currentFrameGotPainted, ui.graphicsView, &cCustomGraphicsView::CurrentFrameGotPainted );
    connect( ui.canvas, &cCanvas::previousFrameGotPainted, ui.graphicsView, &cCustomGraphicsView::PreviousFrameGotPainted );

    connect( ui.colorSwatch, &ColorSwatch::swatchClicked, this, &cMainWindow::AskColor );


    connect (ui.penSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)) );
    connect (ui.stepSpinBox, SIGNAL(valueChanged(int)), this, SLOT(stepChanged(int)) );


    connect( ui.antiAliasCheckBox, &QCheckBox::stateChanged, [ this ]( bool iNewState ){
        auto simpleBrush = dynamic_cast< cToolSimpleBrush* >( mToolModel );
        if( simpleBrush )
            simpleBrush->ApplyProfile( iNewState );
    });

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
    ui.canvas->SetData( mClip->LayerAtIndex( 0 )->Data(), mClip->Width(), mClip->Height() );
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
        UpdateUI();
    }
}


void
cMainWindow::toolDataChanged( const QModelIndex & iLeft, const QModelIndex & iRight, const QVector<int>& iRoles )
{
    if( iLeft.row() == 1 )
        UpdateColor();
}


void
cMainWindow::sizeChanged( int iNew )
{
    mToolModel->setSize( iNew );
}


void
cMainWindow::stepChanged( int iStep )
{
    mToolModel->setStep( iStep );
}


void
cMainWindow::UpdateUI()
{
    ui.penSizeSpinBox->setValue( mToolModel->getSize() );
    ui.colorSwatch->SetColor( mToolModel->getColor() );
    ui.stepSpinBox->setValue( mToolModel->getStep() );

    auto simpleBrush = dynamic_cast< cToolSimpleBrush* >( mToolModel );
    if( simpleBrush )
        ui.antiAliasCheckBox->setChecked( simpleBrush->ApplyProfile() );
}



