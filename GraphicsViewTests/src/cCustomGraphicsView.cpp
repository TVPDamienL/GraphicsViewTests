#include "cCustomGraphicsView.h"

#include "cGraphicItem.h"
#include "cAddItem.h"
#include "cCurrentFrameItem.h"

#include "Clip.h"

#include <QDragEnterEvent>
#include <QMimeData>

#define YPOS 0
#define ITEMSIZE 32

cCustomGraphicsView::cCustomGraphicsView( QWidget *parent ) :
    QGraphicsView( parent ),
    mCurrentFrame( 0 )
{
    // Config
    setAcceptDrops( true );
    setDragMode( RubberBandDrag );


    // Scene
    QGraphicsScene* scene = new QGraphicsScene( this );
    setScene( scene );
    setAlignment( Qt::AlignVCenter | Qt::AlignLeft );


    auto imgA = new  cGraphicItem( this );
    imgA->setZValue( -1 );
    imgA->setFile( ":/cMainWindow/Resources/CanvasAlpha4k.png" );
    scene->addItem( imgA );
    mAnimationImages.push_back( imgA );

    //auto imgB = new  cGraphicItem( this );
    //imgB->setZValue( -1 );
    //imgB->setFile( "Resources/ImgB.png" );
    //scene->addItem( imgB );
    //mAnimationImages.push_back( imgB );

    //auto imgC = new  cGraphicItem( this );
    //imgC->setZValue( -1 );
    //imgC->setFile( "Resources/ImgC.png" );
    //scene->addItem( imgC );
    //mAnimationImages.push_back( imgC );

    //auto imgD = new  cGraphicItem( this );
    //imgD->setZValue( -1 );
    //imgD->setFile( "Resources/ImgD.png" );
    //scene->addItem( imgD );
    //mAnimationImages.push_back( imgD );

    mAddItem = new  cAddItem( this );
    mAddItem->setZValue( -1 );
    scene->addItem( mAddItem );

    mCurrentFrameItem = new cCurrentFrameItem( this );
    scene->addItem( mCurrentFrameItem );

    _UpdateItemsPosition();
    _UpdateCurrentFrameItemPosition();
    _UpdateSceneRect();
}


void
cCustomGraphicsView::dragEnterEvent( QDragEnterEvent * iEvent )
{
    iEvent->acceptProposedAction();
}


void
cCustomGraphicsView::dragMoveEvent( QDragMoveEvent * iEvent )
{
    iEvent->acceptProposedAction();
}


void
cCustomGraphicsView::dragLeaveEvent( QDragLeaveEvent * iEvent )
{
    iEvent->accept();
}


void
cCustomGraphicsView::dropEvent( QDropEvent * iEvent )
{
    const QMimeData* mimeData = iEvent->mimeData();
    if( mimeData->hasImage() )
    {
        auto item = new  cGraphicItem( this );
        item->setZValue( -1 );
        item->setPixmap( qvariant_cast< QPixmap >( mimeData->imageData() ) );
        scene()->addItem( item );
        mAnimationImages.push_back( item );
        _UpdateItemsPosition();
        _UpdateSceneRect();
    }
    else if( mimeData->hasUrls() )
    {
        QList< QUrl > urlList = mimeData->urls();
        for( auto& url : urlList )
        {
            auto item = new  cGraphicItem( this );
            item->setZValue( -1 );
            item->setFile( url.toLocalFile() );
            scene()->addItem( item );
            mAnimationImages.push_back( item );
            _UpdateItemsPosition();
        }
        _UpdateSceneRect();
    }

    iEvent->acceptProposedAction();
}


void
cCustomGraphicsView::keyPressEvent( QKeyEvent * iEvent )
{
    if( iEvent->key() == Qt::Key_Delete )
    {
        for( auto item : mAnimationImages )
            if( item->isSelected() )
                delete  item;

        _SortItems();
        _UpdateItemsPosition();

        if( mCurrentFrame >= mAnimationImages.size() )
            CurrentFrameChanged( mAnimationImages.size() - 1 );

        _UpdateSceneRect();
    }

    QGraphicsView::keyPressEvent( iEvent );
}


void
cCustomGraphicsView::mousePressEvent( QMouseEvent * iEvent )
{
    mOriginPos = iEvent->pos();

    QGraphicsView::mousePressEvent( iEvent );
}


void
cCustomGraphicsView::mouseReleaseEvent( QMouseEvent * iEvent )
{
    QGraphicsView::mouseReleaseEvent( iEvent );

    QPoint delta = iEvent->pos() - mOriginPos;

    if( delta.x() <= -1 || delta.x() >= 1 )
    {
        for( auto item : mAnimationImages )
            if( item->isSelected() )
                return;
    }

    mCurrentFrame = iEvent->pos().x() / 37;
    if( mCurrentFrame >= 0 && mCurrentFrame < mAnimationImages.size() )
    {
        _UpdateCurrentFrameItemPosition();
        emit  currentFrameChanged( mCurrentFrame );
    }
}


void
cCustomGraphicsView::AddItem()
{
    int index = scene()->items().size() - 2; // Cuz addbutton and currentFrame is within
    mAddItem->setPos( 2+ (index+1)*5 + (index+1)*32, YPOS );

    auto item = new  cGraphicItem( this );
    item->setPos( 2+ index*5 + index*ITEMSIZE, YPOS );
    item->setIndex( index );
    item->setZValue( -1 );
    item->setSelected( true );
    scene()->addItem( item );
    mAnimationImages.push_back( item );
    _UpdateSceneRect();
}


void
cCustomGraphicsView::ItemMoving( cGraphicItem* iItem, const QPointF& iNewPosition )
{
}


void
cCustomGraphicsView::ItemMoved()
{
    _SortItems();
    _UpdateItemsPosition();
    emit  currentFrameChanged( mCurrentFrame );
}


void
cCustomGraphicsView::ItemCurrentFrameMoved()
{
    auto index = mCurrentFrameItem->pos().x() / ITEMSIZE;
    if( index >= mAnimationImages.size() )
        index = mAnimationImages.size() - 1;

    if( index < 0 )
        index = 0;

    mCurrentFrame = index;
    emit  currentFrameChanged( mCurrentFrame );
}


void
cCustomGraphicsView::ItemAskToBeRemoved( cGraphicItem * iItem )
{
    delete  iItem;
    _SortItems();
    _UpdateItemsPosition();
    if( mCurrentFrame >= mAnimationImages.size() )
        mCurrentFrame = mAnimationImages.size() - 1;

    CurrentFrameChanged( mCurrentFrame );
    _UpdateSceneRect();
}


QVector< cGraphicItem* >&
cCustomGraphicsView::GetAnimationImages()
{
    return  mAnimationImages;
}


void
cCustomGraphicsView::CurrentFrameChanged( int iCurrent )
{
    mCurrentFrame = iCurrent;
    _UpdateCurrentFrameItemPosition();
    emit  currentFrameChanged( mCurrentFrame );
}


void
cCustomGraphicsView::CurrentFrameGotPainted( const QPixmap & iNewPixmap )
{
    mAnimationImages[ mCurrentFrame ]->setPixmap( iNewPixmap );
}


void
cCustomGraphicsView::PreviousFrameGotPainted( const QPixmap & iNewPixmap )
{
    int prev = mCurrentFrame - 1;
    if( prev < 0 )
        prev = mAnimationImages.size() - 1;
    mAnimationImages[ prev ]->setPixmap( iNewPixmap );
}


void cCustomGraphicsView::_SortItems()
{
    mAnimationImages.clear();
    for( auto item : scene()->items() )
    {
        if( item == mAddItem || item == mCurrentFrameItem )
            continue;

        QPointF position = item->pos();

        int i;
        for( i = 0; i < mAnimationImages.size(); ++i )
        {
            if( mAnimationImages.at( i )->pos().x() > position.x() )
                break;
        }

        auto itemAsGI = dynamic_cast< cGraphicItem* >( item );
        if( itemAsGI )
            mAnimationImages.insert( i, itemAsGI );
        else
            itemAsGI->acceptDrops(); // CRASH so we are aware in this example if any mistake is done
    }
}


void
cCustomGraphicsView::_UpdateSceneRect()
{
    int farthestX2 = mAddItem->scenePos().x() + mAddItem->boundingRect().width();
    scene()->setSceneRect( 0, 0, farthestX2, mCurrentFrameItem->boundingRect().height() );
}


void
cCustomGraphicsView::_UpdateItemsPosition()
{
    int i = 0;
    for( i = 0; i < mAnimationImages.size(); ++i )
        mAnimationImages[ i ]->setPos( 2 + i*5 + i*ITEMSIZE, YPOS );

    mAddItem->setPos( 2 + i*5 + i*ITEMSIZE, YPOS );
}


void
cCustomGraphicsView::_UpdateCurrentFrameItemPosition()
{
    mCurrentFrameItem->setPos( mCurrentFrame*5 + mCurrentFrame*ITEMSIZE, 1 );
}
