#pragma once

#include "colorPicker.h"

#include "PaintToolBase.h"

#include  <QFrame>
#include  <QVBoxLayout>
#include  <QSlider>


class colorPickerDialog :
    public  QFrame
{

public:
    ~colorPickerDialog();
    colorPickerDialog( cPaintToolBase* model, QWidget* iParent = nullptr );

    void  openAtPosition( QPointF iPos );

    QSize  size() const;

    void  sliderChanged( double iValue );
    void  slider2Changed( double iValue );
    void  colorPicked( const QColor& color );

private:
    colorPicker*    _colorPicker;
    QVBoxLayout*    _mainLayout;

    QHBoxLayout*    _horlayout;

    QWidget*        _container;

    QSlider*        _slider;
    QSlider*        _slider2;

    cPaintToolBase*     _ToolModel;
};

