#include "cMainWindow.h"

#include "cGraphicItem.h"
#include <QGraphicsPixmapItem>
#include <QColorDialog>

#include "ToolSimpleBrush.h"
#include "cToolSelectionTest.h"
#include "Rectangle.h"

#include "cLayer.h"

#include "ColorSwatch.h"


cMainWindow::cMainWindow(QWidget *parent) :
    QMainWindow(parent),
    mToolPaint( new cToolSimpleBrush() ),
    mToolSelect( new cToolSelectionTest() ),
    mRectangleShape( new cShapeRectangle() ),
    mRectangleSelection( new cShapeRectangle() )
{
    ui.setupUi(this);

    mCurrentFrame = 0;
    emit  currentFrameChangeAsked( mCurrentFrame );

    mAnimationTimer = new QTimer();
    mAnimationTimer->start( 1000 / 24 );

    mClip = new cClip( 3840, 2160 );
    //mClip = new cClip( 1920, 1080 );
    mClip->AddLayer();

    UpdateUI();

    ui.colorSwatch->SetColor( mToolPaint->getColor() );

    ui.canvas->SetToolModel( mToolPaint );

    connect( ui.playButton, &QPushButton::clicked, this, &cMainWindow::PlayPressed );
    connect( ui.stopButton, &QPushButton::clicked, this, &cMainWindow::StopPressed );
    connect( this, &cMainWindow::currentFrameChangeAsked, ui.graphicsView, &cCustomGraphicsView::CurrentFrameChanged );
    connect( ui.graphicsView, &cCustomGraphicsView::currentFrameChanged, this, &cMainWindow::CurrentFrameChanged );
    connect( ui.canvas, &cCanvas::currentFrameGotPainted, ui.graphicsView, &cCustomGraphicsView::CurrentFrameGotPainted );
    connect( ui.canvas, &cCanvas::previousFrameGotPainted, ui.graphicsView, &cCustomGraphicsView::PreviousFrameGotPainted );
    connect( ui.canvas, &cCanvas::zoomChanged, ui.spinZoomFactor, &QDoubleSpinBox::setValue );
    connect( ui.spinZoomFactor, SIGNAL(valueChanged(double)), ui.canvas, SLOT(SetZoom(double)) );


    // Tool controls
    connect( ui.colorSwatch, &ColorSwatch::swatchClicked, this, &cMainWindow::AskColor );
    connect( ui.penSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)) );
    connect( ui.stepSpinBox, SIGNAL(valueChanged(int)), this, SLOT(stepChanged(int)) );
    connect( ui.antiAliasCheckBox, &QCheckBox::stateChanged, [ this ]( bool iNewState ){
        auto simpleBrush = dynamic_cast< cToolSimpleBrush* >( mToolPaint );
        if( simpleBrush )
            simpleBrush->ApplyProfile( iNewState );
    });


    connect( ui.buttonToolSelect, &QPushButton::clicked, this, &cMainWindow::ToolSelectClicked );
    connect( ui.buttonToolPaint, &QPushButton::clicked, this, &cMainWindow::ToolPaintClicked );
    connect( ui.buttonRectangle, &QPushButton::clicked, this, &cMainWindow::ToolRectangleClicked );


    CurrentFrameChanged( 0 );

    // Tools setups
    //mToolPaint->SetAlphaMask( mClip->GetSelection()->GetSelectionMask() );
    dynamic_cast< cToolSelectionTest* >( mToolSelect )->SetSelection( mClip->GetSelection() );
    mRectangleShape->SetPaintTool( mToolPaint );
    mRectangleSelection->SetPaintTool( mToolSelect );
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
    ui.canvas->SetClip( mClip );
}


void
cMainWindow::UpdateColor()
{
    ui.colorSwatch->SetColor( mToolPaint->getColor() );
}


void
cMainWindow::AskColor()
{
    QColorDialog dialog( mToolPaint->getColor(), this );
    if( dialog.exec() )
    {
        mToolPaint->setColor( dialog.selectedColor() );
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
    mToolPaint->setSize( iNew );
}


void
cMainWindow::stepChanged( int iStep )
{
    mToolPaint->setStep( iStep );
}


void
cMainWindow::UpdateUI()
{
    ui.penSizeSpinBox->setValue( mToolPaint->getSize() );
    ui.colorSwatch->SetColor( mToolPaint->getColor() );
    ui.stepSpinBox->setValue( mToolPaint->getStep() );

    auto simpleBrush = dynamic_cast< cToolSimpleBrush* >( mToolPaint );
    if( simpleBrush )
        ui.antiAliasCheckBox->setChecked( simpleBrush->ApplyProfile() );
}


void
cMainWindow::ToolPaintClicked()
{
    ui.canvas->SetSelectionMode( false );
    ui.canvas->SetToolModel( mToolPaint );
}


void
cMainWindow::ToolSelectClicked()
{
    ui.canvas->SetSelectionMode( true );
    ui.canvas->SetToolModel( mRectangleSelection );
}


void
cMainWindow::ToolRectangleClicked()
{
    ui.canvas->SetToolModel( mRectangleShape );
}



