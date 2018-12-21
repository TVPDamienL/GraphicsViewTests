#include "Canvas.h"


#include "EditableItem.h"

#include "colorPickerDialog.h"


#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QMimeData>

#include <QDebug>


cCanvas::cCanvas( QWidget *parent ) :
    QGraphicsView( parent ),
    cursorPixmap( 0 ),
    mToolModel( 0 ),
    mPainter( 0 )
{
    // Config
    setAcceptDrops( true );
    setAttribute(Qt::WA_TabletTracking);

    // Scene
    QGraphicsScene* scene = new QGraphicsScene( this );
    setScene( scene );
    setAlignment( Qt::AlignCenter );

    mEditableItem = new cEditableItem();
    scene->addItem( mEditableItem );

    mGridItem = new cGridItem();
    scene->addItem( mGridItem );

    QRectF sceneRect = geometry();
    setSceneRect( sceneRect );

    setStyleSheet( "background-color: #555555");
}


void
cCanvas::paintEvent( QPaintEvent * iEvent )
{
    QGraphicsView::paintEvent( iEvent );
}


void
cCanvas::dragEnterEvent( QDragEnterEvent * iEvent )
{
    iEvent->acceptProposedAction();
}


void
cCanvas::dragMoveEvent( QDragMoveEvent * iEvent )
{
    iEvent->acceptProposedAction();
}


void
cCanvas::dragLeaveEvent( QDragLeaveEvent * iEvent )
{
    iEvent->accept();
}


void
cCanvas::dropEvent( QDropEvent * iEvent )
{
    const QMimeData* mimeData = iEvent->mimeData();
    if( mimeData->hasImage() )
    {
    }
    else if( mimeData->hasUrls() )
    {
    }

    iEvent->acceptProposedAction();
}


void
cCanvas::keyPressEvent( QKeyEvent * iEvent )
{
    if( iEvent->key() == Qt::Key_Alt && QApplication::mouseButtons() & Qt::MouseButton::LeftButton )
        mState = kPan;

    if( iEvent->key() == Qt::Key_B )
    {
        mTool = kBrush;
        DrawCursor();
    }
    else if( iEvent->key() == Qt::Key_E )
    {
        mTool = kEraser;
        DrawCursor();
    }
    else if( iEvent->key() == Qt::Key_C )
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setPixmap( *mItemPixmap );
    }
    else if( iEvent->key() == Qt::Key_V )
    {
        QPainter pp( mItemPixmap );
        QClipboard* clipboard = QApplication::clipboard();
        auto px = clipboard->pixmap();

        pp.drawPixmap( 0, 0, px );
        mEditableItem->update();
    }

    QGraphicsView::keyPressEvent( iEvent );
}


void
cCanvas::keyReleaseEvent( QKeyEvent * iEvent )
{
    if( iEvent->key() == Qt::Key_Alt )
    {
        mState = kIdle;
    }
    else if( iEvent->key() == Qt::Key_Delete )
    {
        mItemPixmap = mEditableItem->mpixmap;
        mItemPixmap->fill( Qt::transparent );
        currentFrameGotPainted( *mItemPixmap );
        mEditableItem->update();
    }

    QGraphicsView::keyReleaseEvent( iEvent );
}


void
cCanvas::mousePressEvent( QMouseEvent * iEvent )
{
    mClickPos = iEvent->pos();

    if( iEvent->button() == Qt::LeftButton )
    {
        if( QApplication::keyboardModifiers() & Qt::AltModifier )
        {
            mState = kPan;
        }
        else
        {
            mState = kDrawing;
            mItemPixmap = mEditableItem->mpixmap;
            mItemPixmapAsImage = mItemPixmap->toImage();

            mToolModel->StartDrawing();

            if( mTool == kEraser )
                mPainter->setCompositionMode( QPainter::CompositionMode_Clear );
        }
    }
    else if( iEvent->button() == Qt::RightButton )
    {
        if( QApplication::keyboardModifiers() & Qt::ControlModifier )
        {
            //auto test = new colorPickerDialog( mToolModel, this );
            //test->openAtPosition( iEvent->screenPos() - QPointF( test->size().width()/2, test->size().height()/2 ) );
        }
    }

    QGraphicsView::mousePressEvent( iEvent );
}


void
cCanvas::mouseMoveEvent( QMouseEvent * iEvent )
{
    if( mState == kPan )
    {
        QPointF offset = iEvent->pos() - mClickPos;
        QPointF pos = mEditableItem->pos() + offset;
        mEditableItem->setPos( pos );
        mGridItem->setPos( pos );
    }
    else if( mState == kDrawing )
    {
        QPointF originInItemCoordinate = mEditableItem->mapFromScene( mapToScene( mClickPos.x(), mClickPos.y() ) );
        QPointF newPointInItemCoordinate = mEditableItem->mapFromScene( mapToScene( iEvent->pos().x(), iEvent->pos().y() ) );

        sPointData point;
        point.mPosition = QPoint( newPointInItemCoordinate.x(), newPointInItemCoordinate.y() );

        mToolModel->PathAddPoint( point );
        mToolModel->DrawPathFromLastRenderedPoint( &mItemPixmapAsImage );


        SetPixmap( QPixmap::fromImage( mItemPixmapAsImage )  );
        currentFrameGotPainted( *mEditableItem->mpixmap );
    }

    mClickPos = iEvent->pos();
    QGraphicsView::mouseMoveEvent( iEvent );
}


void
cCanvas::mouseReleaseEvent( QMouseEvent * iEvent )
{
    if( mState == kDrawing )
    {
        mToolModel->EndDrawing();
        currentFrameGotPainted( *mEditableItem->mpixmap );
    }

    mState = kIdle;
    QGraphicsView::mouseReleaseEvent( iEvent );
}


void
cCanvas::tabletEvent( QTabletEvent * iEvent )
{
    iEvent->accept();
    qDebug() << "Event";
}


void
cCanvas::wheelEvent( QWheelEvent * iEvent )
{
    int delta = iEvent->delta();
    if( QApplication::keyboardModifiers() & Qt::AltModifier )
    {
        if( delta > 0 )
            mEditableItem->setScale( mEditableItem->scale() * 1.5 );
        else
            mEditableItem->setScale( mEditableItem->scale() / 1.5 );

        UpCursor();
        UpdateGridItem();
    }

    QGraphicsView::wheelEvent( iEvent );
}


void
cCanvas::SetPixmap( const QPixmap & iPixmap )
{
    if( mState == kDrawing )
    {
        delete  mPainter;
        //previousFrameGotPainted( *mItemPixmap );
    }

    delete  mEditableItem->mpixmap;
    mEditableItem->mpixmap = new QPixmap( iPixmap );
    mEditableItem->update();

    if( mState == kDrawing )
    {
        mItemPixmap = mEditableItem->mpixmap;
    }
}


void
cCanvas::SetToolModel( ToolBase* iToolModel )
{
    if( mToolModel )
        disconnect( mToolModel, &QAbstractItemModel::dataChanged, this, &cCanvas::toolChanged );

    mToolModel = iToolModel;
    DrawCursor();
    connect( mToolModel, &QAbstractItemModel::dataChanged, this, &cCanvas::toolChanged );
}


void
cCanvas::UpdateGridItem()
{
    float  pixelSize = mEditableItem->sceneBoundingRect().width() / mEditableItem->boundingRect().width();

    mGridItem->psize = pixelSize;
    mGridItem->size = mEditableItem->sceneBoundingRect().size();
}


void
cCanvas::UpCursor()
{
    if( !cursorPixmap || ( cursorPixmap->height() * mEditableItem->scale() > width() ) )
        setCursor( Qt::ArrowCursor );
    else
        setCursor( QCursor( cursorPixmap->scaledToHeight( cursorPixmap->height() * mEditableItem->scale() ) ) );
}


void
cCanvas::DrawCursor()
{
    delete  cursorPixmap;
    UpCursor();
}


void
cCanvas::toolChanged( const QModelIndex & Left, const QModelIndex & Right, const QVector<int>& Roles )
{
    if( Left.row() == 0 )
        DrawCursor();
}

