#include "ImageDebugger.h"

#include <QGraphicsItem>
#include <QDebug>

cImageDebugger::cImageDebugger( QWidget * parent ) :
    QDialog( parent )
{
    mView = new QGraphicsView( this );

    QGraphicsScene* scene = new QGraphicsScene( mView );
    mView->setScene( scene );
    mView->setAlignment( Qt::AlignCenter );
    setGeometry( 0, 0, 500, 500 );
    mView->setGeometry( 0, 0, 500, 500 );
    hide();
}


//static
cImageDebugger*
cImageDebugger::Instance( QWidget * parent )
{
    static cImageDebugger* gimg = 0;
    if( !gimg )
        gimg = new cImageDebugger( parent );

    return  gimg;
}


void
cImageDebugger::resizeEvent( QResizeEvent * iEvent )
{
    mView->setGeometry( geometry() );
}


void
cImageDebugger::ShowImage( const QImage * image )
{
    mView->scene()->clear();

    auto item = new QGraphicsPixmapItem();
    item->setPixmap( QPixmap::fromImage( *image ) );
    mView->scene()->addItem( item );
    show();
}


void
cImageDebugger::ShowImages( QVector<const QImage*> images )
{
    mView->scene()->clear();

    int x = 0;
    for( auto image : images )
    {
        auto item = new QGraphicsPixmapItem();
        item->setPixmap( QPixmap::fromImage( *image ) );
        item->setPos( x, 0 );
        mView->scene()->addItem( item );
        x += 5 + image->width();
    }

    show();
}


void
cImageDebugger::ShowImages( QVector<QImage*> images )
{
    mView->scene()->clear();

    int x = 0;
    for( auto image : images )
    {
        auto item = new QGraphicsPixmapItem();
        item->setPixmap( QPixmap::fromImage( *image ) );
        item->setPos( x, 0 );
        mView->scene()->addItem( item );
        x += 5 + image->width();
    }

    show();
}


void
cImageDebugger::ShowImage( const float * image, int width, int height )
{
    QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
    uchar* data = img->bits();

    const int bpl = width * 4;

    for( int x = 0; x < bpl; ++x )
        for( int y = 0; y < height; ++y )
        {
            int index = y * bpl + x;
            data[ index ] = uchar( image[ index ] );
    }

    ShowImage( img );
}


void
cImageDebugger::ShowImage( const float * image, int width, int height, const QRect& iArea )
{
    QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
    img->fill( Qt::transparent );
    uchar* data = img->bits();

    const int bpl = width * 4;

    for( int y = iArea.top(); y <= iArea.bottom(); ++y )
        for( int x = iArea.left(); x <= iArea.right(); ++x )
        {
            int index = y * bpl + x * 4;
            data[ index+0 ] = uchar( image[ index+0 ] );
            data[ index+1 ] = uchar( image[ index+1 ] );
            data[ index+2 ] = uchar( image[ index+2 ] );
            data[ index+3 ] = uchar( image[ index+3 ] );
        }

    ShowImage( img );
}


void
cImageDebugger::ShowImageGray( const float * image, int width, int height )
{
    QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
    uchar* data = img->bits();

    const int bpl = img->bytesPerLine();

    for( int x = 0; x < width; ++x )
        for( int y = 0; y < height; ++y )
        {
            int indexRGBA = y * bpl + x * 4;
            int indexGray = indexRGBA/4;

            data[ indexRGBA+0 ] = 0;
            data[ indexRGBA+1 ] = 0;
            data[ indexRGBA+2 ] = 0;
            data[ indexRGBA+3 ] = uchar( image[ indexGray ] );
        }

    ShowImage( img );
}


void
cImageDebugger::ShowImages( QVector<const float*> images, QVector<int> widths, QVector<int> heights )
{
    QVector< const QImage* > outputs;

    for( int i = 0; i < images.size(); ++i )
    {
        const float* image = images[ i ];
        const int width = widths[ i ];
        const int height = heights[ i ];
        QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
        uchar* data = img->bits();

        const int bpl = width * 4;

        for( int x = 0; x < bpl; ++x )
            for( int y = 0; y < height; ++y )
            {
                int index = y * bpl + x;
                data[ index ] = image[ index ];
            }

        outputs.push_back( img );
    }

    ShowImages( outputs );
}


void
cImageDebugger::ShowImagesGray( QVector<const float*> images, QVector<int> widths, QVector<int> heights )
{
    QVector< const QImage* > outputs;

    for( int i = 0; i < images.size(); ++i )
    {
        const float* image = images[ i ];
        const int width = widths[ i ];
        const int height = heights[ i ];

        QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
        uchar* data = img->bits();

        const int bpl = img->bytesPerLine();

        for( int x = 0; x < width; ++x )
            for( int y = 0; y < height; ++y )
            {
                int indexRGBA = y * bpl + x * 4;
                int indexGray = indexRGBA/4;

                data[ indexRGBA+0 ] = 0;
                data[ indexRGBA+1 ] = 0;
                data[ indexRGBA+2 ] = 0;
                data[ indexRGBA+3 ] = uchar( image[ indexGray ] );
            }

        outputs.push_back( img );
    }

    ShowImages( outputs );
}


void
cImageDebugger::Hide()
{
    hide();
}


void
cImageDebugger::AddImage( const QImage * iImage )
{
    mImages.push_back( iImage );
}


void
cImageDebugger::AddImage( const float * image, int width, int height )
{
    QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
    uchar* data = img->bits();

    const int bpl = width * 4;

    for( int x = 0; x < bpl; ++x )
        for( int y = 0; y < height; ++y )
        {
            int index = y * bpl + x;
            data[ index ] = uchar( image[ index ] );
        }

    AddImage( img );
}


void
cImageDebugger::AddImage( const float * image, int width, int height, const QRect & iArea )
{
    QImage* img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
    img->fill( Qt::transparent );
    uchar* data = img->bits();

    const int bpl = width * 4;

    for( int y = iArea.top(); y <= iArea.bottom(); ++y )
        for( int x = iArea.left(); x <= iArea.right(); ++x )
        {
            int index = y * bpl + x * 4;
            data[ index+0 ] = uchar( image[ index+0 ] );
            data[ index+1 ] = uchar( image[ index+1 ] );
            data[ index+2 ] = uchar( image[ index+2 ] );
            data[ index+3 ] = uchar( image[ index+3 ] );
        }

    AddImage( img );
}


void
cImageDebugger::ShowNextImage()
{
    if( mImages.size() > 0 )
    {
        auto img = mImages.front();
        ShowImage( img );
        mImages.erase( mImages.begin() );
        delete img;
    }
}


void
cImageDebugger::ClearImages()
{
    for( auto img : mImages )
        delete  img;

    mImages.clear();
}
