#include "Canvas.h"


#include "EditableItem.h"

#include "colorPickerDialog.h"

#include "cLayer.h"
#include "Image.LineSimplification.h"

#include "ShapeBase.h"
#include "cHUDShapeTool.h"

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
    mTool( 0 ),
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
    //scene->addItem( mGridItem );

    QRectF sceneRect = geometry();
    setSceneRect( sceneRect );

    setStyleSheet( "background-color: #555555" );

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
            MouseDown( iEvent->pos(), iEvent->button(), iEvent->pressure(), iEvent->rotation() );

            QGraphicsView::tabletEvent( iEvent );
            break;
        }
        case  QEvent::TabletMove:
        {
            MouseMove( iEvent->pos(), iEvent->pressure(), iEvent->rotation() );

            QGraphicsView::tabletEvent( iEvent );
            break;
        }
        case  QEvent::TabletRelease:
        {
            MouseUp( iEvent->pos(), iEvent->pressure(), iEvent->rotation() );

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
        mToolType = kBrush;
        DrawCursor();
    }
    else if( iEvent->key() == Qt::Key_E )
    {
        mToolType = kEraser;
        DrawCursor();
    }
    else if( iEvent->key() == Qt::Key_C )
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setImage( *mCurrentEditedImage );
    }
    else if( iEvent->key() == Qt::Key_V )
    {
        QPainter pp( mCurrentEditedImage );
        QClipboard* clipboard = QApplication::clipboard();
        auto px = clipboard->image();

        pp.drawImage( 0, 0, px );
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
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_Z )
    {
        mClip->LayerAtIndex( 0 )->Undo();
        mClip->DirtyAll();
        //currentFrameGotPainted( *mEditableItem->mpixmap );
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_Y )
    {
        mClip->LayerAtIndex( 0 )->Redo();
        mClip->DirtyAll();
        //currentFrameGotPainted( *mEditableItem->mpixmap );
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_D )
    {
        mClip->GetSelection()->Clear();
        mHUDSelection->SetSelectionOutlineImage( 0 );
        mHUDSelection->update();
    }
    else if( iEvent->modifiers() & Qt::ControlModifier && iEvent->key() == Qt::Key_T )
    {
        mClip->DirtyArea( QRect( 0, 0, mClip->Width(), mClip->Height() ) );
    }
    else if( iEvent->key() == Qt::Key_Escape )
    {
        mClip->GetSelection()->CancelTransformation();
    }
    else if( iEvent->key() == Qt::Key_Enter || iEvent->key() == Qt::Key_Return  )
    {
        mClip->GetSelection()->ApplyTransformation();
    }

    QGraphicsView::keyReleaseEvent( iEvent );
}


void
cCanvas::mousePressEvent( QMouseEvent * iEvent )
{
    MouseDown( iEvent->pos(), iEvent->button(), 1.0, 0.0 );
    QGraphicsView::mousePressEvent( iEvent );
}


void
cCanvas::mouseMoveEvent( QMouseEvent * iEvent )
{
    MouseMove( iEvent->pos(), 1.0, 0.0 );
    QGraphicsView::mouseMoveEvent( iEvent );
}


void
cCanvas::mouseReleaseEvent( QMouseEvent * iEvent )
{
    MouseUp( iEvent->pos(), 1.0, 0.0 );
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

        mHUDView->SetScale( mScale );
        mHUDView->SetTranslation( mTranslation );


        emit zoomChanged( mScale );

        UpCursor();
        UpdateGridItem();
    }

    QGraphicsView::wheelEvent( iEvent );
}


void
cCanvas::MouseDown( const QPoint& iPos, const Qt::MouseButton& iButton, double iPressure, double iRotation )
{
    mClickPos = iPos;

    if( iButton == Qt::LeftButton )
    {
        if( QApplication::keyboardModifiers() == Qt::AltModifier )
        {
            mState = kPan;
            mOriginTranslation = mTranslation;
        }
        else if( QApplication::keyboardModifiers() == ( Qt::AltModifier | Qt::ShiftModifier ) )
        {
            mState = kRotate;

            mOriginTranslation = mTranslation;
            mOriginRotation = mRotationAngle;
        }
        else
        {
            auto paintTool = dynamic_cast< cPaintToolBase* >( mTool );
            if( paintTool && !mSelectionMode )
            {
                if( mClip->GetSelection()->IsActive() )
                {
                    paintTool->SetAlphaMask( mClip->GetSelection()->GetSelectionMask() );
                }
                else
                {
                    paintTool->ClearAlphaMask();
                }
            }

            mState = kDrawing;

            sPointData point;
            QPointF clickMapped = mEditableItem->mapFromScene( mapToScene( iPos.x(), iPos.y() ) );
            point.mPosition = clickMapped;
            point.mPressure = 1.0F;
            point.mRotation = 0.0F;

            mTool->StartDrawing( mClip->CurrentLayer()->Image(), point );

            if( mToolType == kEraser )
                mPainter->setCompositionMode( QPainter::CompositionMode_Clear );
        }
    }
    else if( iButton == Qt::RightButton )
    {
        if( QApplication::keyboardModifiers() & Qt::ControlModifier )
        {
            //auto test = new colorPickerDialog( mTool, this );
            //test->openAtPosition( iEvent->screenPos() - QPointF( test->size().width()/2, test->size().height()/2 ) );
        }
    }
}


void
cCanvas::MouseMove( const QPoint& iPos, double iPressure, double iRotation )
{
    if( mState == kPan )
    {
        QPointF offset = iPos - mClickPos;

        mTranslation = mOriginTranslation;
        TranslateBy( QPoint( offset.x(), offset.y() ) );

        mEditableItem->setTransform( mTransform );
        mGridItem->setTransform( mTransform );
        mHUDSelection->setTransform( mTransform );

        mHUDView->SetTranslation( mTranslation );
    }
    else if( mState == kRotate )
    {
        auto canvasCenter   = QPoint( width()/2, height()/2 );
        auto currentClick  = iPos;

        QPointF originVector = mClickPos - canvasCenter;
        QPointF newVector = currentClick - canvasCenter;

        double firstAngle = atan2( originVector.y(), originVector.x() );
        double secondAngle = atan2( newVector.y(), newVector.x() );

        double theAngle = secondAngle - firstAngle;

        mTranslation = mOriginTranslation;
        mRotationAngle = mOriginRotation;
        _mCosAngle = cos( mOriginRotation );
        _mSinAngle = sin( mOriginRotation );

        RotateFromCenterPost( canvasCenter, theAngle );

        mEditableItem->setTransform( mTransform );
        mGridItem->setTransform( mTransform );
        mHUDSelection->setTransform( mTransform );

        mHUDView->SetTranslation( mTranslation );
        mHUDView->SetRotation( mRotationAngle );
    }
    else if( mState == kDrawing )
    {
        QPointF newPointInItemCoordinate = mEditableItem->mapFromScene( mapToScene( iPos.x(), iPos.y() ) );

        sPointData point;
        point.mPosition = newPointInItemCoordinate;
        point.mPressure = iPressure;
        point.mRotation = iRotation;

        auto dirty = mTool->MoveDrawing( point );
        if( !dirty.isEmpty() )
            mClip->DirtyArea( dirty );
    }
}


void
cCanvas::MouseUp( const QPoint& iPos, double iPressure, double iRotation )
{
    if( mState == kDrawing )
    {
        sPointData point;
        QPointF clickMapped = mEditableItem->mapFromScene( mapToScene( iPos.x(), iPos.y() ) );
        point.mPosition = clickMapped;
        point.mPressure = 1.0F;
        point.mRotation = 0.0F;

        auto dirty = mTool->EndDrawing( point );
        if( !dirty.isEmpty() )
            mClip->DirtyArea( dirty );

        if( !mSelectionMode )
        {
            mClip->CurrentLayer()->WriteUndoHistory();
            //currentFrameGotPainted( *mEditableItem->mpixmap );

            auto toolPaint = dynamic_cast< cPaintToolBase* >( mTool );
            if( toolPaint && toolPaint->Vectorial() )
            {
                mHUDView->RemoveHUDObject( mHUDPath );
                delete  mHUDPath;

                mHUDPath = new cHUDPath( mHUDView, 0, toolPaint->Path(), toolPaint, mClip );
                mHUDView->AddHUDObject( mHUDPath );
                mHUDPath->Visible( true );



                // To show a before/after line simplification
                //mHUDView->RemoveHUDObject( mHUDPath2 );
                //delete  mHUDPath2;

                //mHUDPath2 = new cHUDPath( mHUDView, 0, SimplifyLine( toolPaint->Path(), 20000000, 0.1 ) );
                //mHUDPath2->SetHandleColor( Qt::green );
                //mHUDView->AddHUDObject( mHUDPath2 );
                //mHUDPath2->Visible( true );
            }
        }
        else
        {
            mHUDSelection->SetSelectionOutlineImage( mClip->GetSelection()->GetSelectionEdgeMask() );
        }
    }

    mState = kIdle;
}



void
cCanvas::SetImage( QImage* iImage )
{
    if( mState == kDrawing )
    {
        delete  mPainter;
        //previousFrameGotPainted( *mItemPixmap );
    }

    //qDebug() << "Up";
    mCurrentEditedImage = iImage;
    mEditableItem->mImage = iImage;
    mEditableItem->update();
    repaint( mTransform.mapRect( _DirtyAreaForRepaint ) );
}


void
cCanvas::SetClip( cClip * iClip )
{
    mClip = iClip;
    mHUDTransform->SetSelection( mClip->GetSelection() );

    mClip->RegisterEditionCallback( [this]( cBaseData* sender, int arg ) {

        if( arg == cClip::eMessageClip::kDirty )
        {
            _DirtyAreaForRepaint = mClip->DirtyArea();
            SetImage( mClip->ComposeLayers() );
        }

        });

    SetImage( mClip->ComposeLayers() );
}


void
cCanvas::SetToolModel( ToolBase* iToolModel )
{
    if( mTool )
        disconnect( mTool, &QAbstractItemModel::dataChanged, this, &cCanvas::toolChanged );

    mTool = iToolModel;
    DrawCursor();

    // HUD
    auto toolAsShapeTool = dynamic_cast< cShapeBase* >( mTool );
    if( toolAsShapeTool )
    {
         mHUDShape = new cHUDShapeTool( mHUDView, 0, toolAsShapeTool );
         mHUDView->AddHUDObject( mHUDShape );
    }
    else
    {
        mHUDView->RemoveHUDObject( mHUDShape );
        delete mHUDShape;
        mHUDShape = 0;
    }

    // PaintTool


    connect( mTool, &QAbstractItemModel::dataChanged, this, &cCanvas::toolChanged );
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
cCanvas::TranslateBy( const QPoint & iOffset )
{
    mTranslation += iOffset;
    _RecomputeMatrix();
}


void
cCanvas::ScaleFromCenter( const QPoint & iCenter, double iScale )
{
    const double xPart = mScale * (iCenter.x() - iScale * iCenter.x());
    const double yPart = mScale * (iCenter.y() - iScale * iCenter.y());

    mTranslation.setX( _mCosAngle * xPart - _mSinAngle * yPart + mTranslation.x() );
    mTranslation.setY( _mSinAngle * xPart + _mCosAngle * yPart + mTranslation.y() );

    mScale *= iScale;
    _RecomputeMatrix();
}


void
cCanvas::RotateFromCenterPost( const QPoint & iCenter, double iAngle )
{
    const double cosAngle = cos( iAngle );
    const double sinAngle = sin( iAngle );

    const double xPart = mTranslation.x() - iCenter.x();
    const double yPart = mTranslation.y() - iCenter.y();

    mTranslation.setX( cosAngle * xPart - sinAngle * yPart + iCenter.x() );
    mTranslation.setY( sinAngle * xPart + cosAngle * yPart + iCenter.y() );

    mRotationAngle += iAngle;
    _mCosAngle = cos( mRotationAngle );
    _mSinAngle = sin( mRotationAngle );
    _RecomputeMatrix();
}


void
cCanvas::_RecomputeMatrix()
{
    const double cosScaled = _mCosAngle * mScale;
    const double sinScaled = _mSinAngle * mScale;

    mTransform.setMatrix( cosScaled, sinScaled, 0, -sinScaled, cosScaled, 0, mTranslation.x(), mTranslation.y(), 1 );
}


void
cCanvas::SetZoom( double iZoom )
{
    const double scalus = iZoom/mScale;
    mTransform.scale( scalus, scalus );

    mScale = iZoom;

    mEditableItem->setTransform( mTransform );
    mHUDSelection->setTransform( mTransform );
    mHUDView->SetScale( mScale );
    mHUDView->SetTranslation( mTranslation );

    UpCursor();
    UpdateGridItem();
}
