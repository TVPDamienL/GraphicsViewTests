#include "Canvas.h"


#include "EditableItem.h"

#include "colorPickerDialog.h"

#include "cLayer.h"


#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QDragEnterEvent>
#include <QMimeData>

#include <QDebug>
#include <QScrollBar>


cCanvas::cCanvas( QWidget *parent ) :
    QGraphicsView( parent ),
    cursorPixmap( 0 ),
    mToolModel( 0 ),
    mPainter( 0 )
{
    // Config
    setAcceptDrops( true );
    setAttribute(Qt::WA_TabletTracking);

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // Scene
    QGraphicsScene* scene = new QGraphicsScene( this );
    setScene( scene );
    setAlignment( Qt::AlignCenter );

    mHUDSelection = new cHUDItem();
    scene->addItem( mHUDSelection );
    mHUDSelection->setZValue( 10 );

    mEditableItem = new cEditableItem();
    scene->addItem( mEditableItem );

    mGridItem = new cGridItem();
    scene->addItem( mGridItem );

    QRectF sceneRect = geometry();
    setSceneRect( sceneRect );

    setStyleSheet( "background-color: #555555");

    __DebugAlphaMaskTest__ = new QImage( ":/cMainWindow/Resources/AlphaMask.png" );

    mHUDView = new cHUDView( viewport()  );
    QRect geom( 0, 0, width(), height() );
    mHUDView->setGeometry( geom );
    mHUDView->update();

    // HUDS
    mHUDTransform = new cHUDTransform( mHUDView, 0 );
    mHUDView->AddHUDObject( mHUDTransform );
}


void
cCanvas::resizeEvent( QResizeEvent * iEvent )
{
    QRect geom( 0, 0, width(), height() );
    setSceneRect( geom );
    mHUDView->setGeometry( geom );
}


void
cCanvas::tabletEvent( QTabletEvent*  iEvent )
{
    bool check = qApp->testAttribute(Qt::AA_SynthesizeMouseForUnhandledTabletEvents);

    switch( iEvent->type() )
    {
        case  QEvent::TabletPress:
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
                    if( !mSelectionMode )
                    {
                        if( mClip->GetSelection()->IsActive() )
                        {
                            mToolModel->SetAlphaMask( mClip->GetSelection()->GetSelectionMask() );
                        }
                        else
                        {
                            mToolModel->SetAlphaMask( 0 );
                        }
                    }

                    mState = kDrawing;
                    mItemPixmap = mEditableItem->mpixmap;

                    mToolModel->StartDrawing( mClip->LayerAtIndex( 0 )->Image() );

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

            QGraphicsView::tabletEvent( iEvent );
            break;
        }
        case  QEvent::TabletRelease:
        {
            if( mState == kDrawing )
            {
                auto dirty = mToolModel->EndDrawing();
                if( !dirty.isEmpty() )
                {
                    mClip->DirtyArea( dirty );
                    SetPixmap( QPixmap::fromImage( *mClip->ComposeLayers() ) );

                    if( !mSelectionMode )
                    {
                        currentFrameGotPainted( *mEditableItem->mpixmap );
                    }
                }

                if( !mSelectionMode )
                {
                    mClip->LayerAtIndex( 0 )->WriteUndoHistory();
                    currentFrameGotPainted( *mEditableItem->mpixmap );
                }
                else
                {
                    mHUDSelection->SetSelectionOutlineImage( mClip->GetSelection()->GetSelectionEdgeMask() );
                }
            }

            mState = kIdle;
            QGraphicsView::tabletEvent( iEvent );
            break;
        }
        case  QEvent::TabletMove:
        {
            if( mState == kPan )
            {
                QPointF offset = iEvent->pos() - mClickPos;
                QPointF pos = mEditableItem->pos() + offset;
                mEditableItem->setPos( pos );
                mGridItem->setPos( pos );
                mHUDSelection->setPos( pos );
                mHUDView->TranslateBy( QPoint( offset.x(), offset.y() ) );
            }
            else if( mState == kDrawing )
            {
                QPointF originInItemCoordinate = mEditableItem->mapFromScene( mapToScene( mClickPos.x(), mClickPos.y() ) );
                QPointF newPointInItemCoordinate = mEditableItem->mapFromScene( mapToScene( iEvent->pos().x(), iEvent->pos().y() ) );

                sPointData point;
                point.mPosition = QPoint( newPointInItemCoordinate.x(), newPointInItemCoordinate.y() );
                point.mPressure = iEvent->pressure();
                point.mRotation = 0.0F;

                auto dirty = mToolModel->MoveDrawing( point );
                if( !dirty.isEmpty() )
                {
                    mClip->DirtyArea( dirty );
                    SetPixmap( QPixmap::fromImage( *mClip->ComposeLayers() ) );

                    if( !mSelectionMode )
                    {
                        currentFrameGotPainted( *mEditableItem->mpixmap );
                    }
                }
            }

            mClickPos = iEvent->pos();
            QGraphicsView::tabletEvent( iEvent );
            break;
        }

        default:
            qDebug() << iEvent;
    }
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
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_Z )
    {
        mClip->LayerAtIndex( 0 )->Undo();
        _SetImage( mClip->LayerAtIndex( 0 )->Image() );
        currentFrameGotPainted( *mEditableItem->mpixmap );
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_Y )
    {
        mClip->LayerAtIndex( 0 )->Redo();
        _SetImage( mClip->LayerAtIndex( 0 )->Image() );
        currentFrameGotPainted( *mEditableItem->mpixmap );
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_D )
    {
        mClip->GetSelection()->Clear();
        mHUDSelection->SetSelectionOutlineImage( 0 );
        mHUDSelection->update();
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_T )
    {
        mClip->ExtractSelection();
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
            if( !mSelectionMode )
            {
                if( mClip->GetSelection()->IsActive() )
                {
                    mToolModel->SetAlphaMask( mClip->GetSelection()->GetSelectionMask() );
                }
                else
                {
                    mToolModel->SetAlphaMask( 0 );
                }
            }

            mState = kDrawing;
            mItemPixmap = mEditableItem->mpixmap;

            mToolModel->StartDrawing( mClip->LayerAtIndex( 0 )->Image() );

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
        mTranslation += pos;

        mTransform *= QTransform::fromTranslate( pos.x(), pos.y() );

        mEditableItem->setTransform( mTransform );
        mGridItem->setTransform( mTransform );
        mHUDSelection->setTransform( mTransform );

        mHUDView->mTranslation = mTranslation;
    }
    else if( mState == kDrawing )
    {
        QPointF originInItemCoordinate = mEditableItem->mapFromScene( mapToScene( mClickPos.x(), mClickPos.y() ) );
        QPointF newPointInItemCoordinate = mEditableItem->mapFromScene( mapToScene( iEvent->pos().x(), iEvent->pos().y() ) );

        sPointData point;
        point.mPosition = QPoint( newPointInItemCoordinate.x(), newPointInItemCoordinate.y() );
        point.mPressure = 1.0F;
        point.mRotation = 0.0F;

        auto dirty = mToolModel->MoveDrawing( point );
        if( !dirty.isEmpty() )
        {
            mClip->DirtyArea( dirty );
            SetPixmap( QPixmap::fromImage( *mClip->ComposeLayers() ) );

            if( !mSelectionMode )
            {
                currentFrameGotPainted( *mEditableItem->mpixmap );
            }
        }
    }

    mClickPos = iEvent->pos();
    QGraphicsView::mouseMoveEvent( iEvent );
}


void
cCanvas::mouseReleaseEvent( QMouseEvent * iEvent )
{
    if( mState == kDrawing )
    {
        auto dirty = mToolModel->EndDrawing();
        if( !dirty.isEmpty() )
        {
            mClip->DirtyArea( dirty );
            SetPixmap( QPixmap::fromImage( *mClip->ComposeLayers() ) );

            if( !mSelectionMode )
            {
                currentFrameGotPainted( *mEditableItem->mpixmap );
            }
        }

        if( !mSelectionMode )
        {
            mClip->LayerAtIndex( 0 )->WriteUndoHistory();
            currentFrameGotPainted( *mEditableItem->mpixmap );
        }
        else
        {
            mHUDSelection->SetSelectionOutlineImage( mClip->GetSelection()->GetSelectionEdgeMask() );
            mHUDSelection->SetSelectionInsideImage( mClip->GetSelection()->GetSelectionContentImage() );
        }
    }

    mState = kIdle;
    QGraphicsView::mouseReleaseEvent( iEvent );
}


void
cCanvas::wheelEvent( QWheelEvent * iEvent )
{
    int delta = iEvent->delta();
    if( QApplication::keyboardModifiers() & Qt::AltModifier )
    {
        double baseScale = mEditableItem->scale();
        QPoint center = mTransform.inverted().map( iEvent->pos() );

        double newScale = baseScale * 1.5;

        if( delta < 0 )
        {
            newScale = baseScale / 1.5;
        }

        ScaleFromCenter( center, newScale );

        mEditableItem->setTransform( mTransform );
        mHUDSelection->setTransform( mTransform );

        mHUDView->mScale = mScale;
        mHUDView->mTranslation = mTranslation;


        emit zoomChanged( mScale );

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
cCanvas::SetClip( cClip * iClip )
{
    mClip = iClip;
    mHUDTransform->SetSelection( mClip->GetSelection() );
    _SetImage( mClip->LayerAtIndex( 0 )->Image() );
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
cCanvas::SetSelectionMode( bool iMode )
{
    mSelectionMode = iMode;
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


void
cCanvas::ScaleFromCenter( const QPoint & iCenter, double iScale )
{
    mTransform.translate( iCenter.x(), iCenter.y() );
    mTransform.scale( iScale, iScale );
    mTransform.translate( -iCenter.x(), -iCenter.y() );

    // NO QT
    mTranslation.setX( mScale * ( iCenter.x() - iScale * iCenter.x() ) + mTranslation.x() );
    mTranslation.setY( mScale * ( iCenter.y() - iScale * iCenter.y() ) + mTranslation.y() );
    mScale *= iScale;
}


void
cCanvas::SetZoom( double iZoom )
{
    const double scalus = iZoom/mScale;
    mTransform.scale( scalus, scalus );

    mScale = iZoom;

    mEditableItem->setTransform( mTransform );
    mHUDSelection->setTransform( mTransform );
    mHUDView->mScale = mScale;
    mHUDView->mTranslation = mTranslation;

    UpCursor();
    UpdateGridItem();
}


void
cCanvas::_SetImage( const QImage* iImage )
{
    delete  mEditableItem->mpixmap;

    mEditableItem->mpixmap = new QPixmap( QPixmap::fromImage( *iImage ) );
    mEditableItem->update();

    if( mState == kDrawing )
    {
        mItemPixmap = mEditableItem->mpixmap;
    }
}

