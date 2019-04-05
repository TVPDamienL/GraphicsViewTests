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
    void ShowImageGray( const float* image, int width, int height );
    void ShowImages( QVector< const float* > images, QVector< int > widths, QVector< int > heights );
    void ShowImagesGray( QVector< const float* > images, QVector< int > widths, QVector< int > heights );
    void Hide();

private:
    QGraphicsView* mView;
};

