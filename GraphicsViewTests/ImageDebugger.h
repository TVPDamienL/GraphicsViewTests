#pragma once


#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>

#define IMAGEDEBUG cImageDebugger::Instance( 0 )


class cImageDebugger :
    public QDialog
{
    Q_OBJECT

public:
    cImageDebugger( QWidget *parent = nullptr );

public:
    static cImageDebugger* Instance( QWidget * parent );

public:
    void resizeEvent( QResizeEvent* iEvent ) override;

public:
    void ShowImage( const QImage* image );
    void ShowImages( QVector< const QImage* > images );
    void ShowImages( QVector< QImage* > images );

    void ShowImage( const float* image, int width, int height );
    void ShowImage( const float* image, int width, int height, const QRect& iArea );
    void ShowImageGray( const float* image, int width, int height );
    void ShowImages( QVector< const float* > images, QVector< int > widths, QVector< int > heights );
    void ShowImagesGray( QVector< const float* > images, QVector< int > widths, QVector< int > heights );
    void Hide();

public:
    void AddImage( const QImage* iImage );
    void AddImage( const float* image, int width, int height );
    void AddImage( const float* image, int width, int height, const QRect& iArea );
    void ShowNextImage();
    void ClearImages();

private:
    QGraphicsView* mView;
    std::vector< const QImage* > mImages;
};

