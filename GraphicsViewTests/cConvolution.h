#pragma once

#include <QGenericMatrix>
#include <QImage>


class cConvolution
{
public:
    ~cConvolution();
    cConvolution();


public:
    void SetKernel( const QGenericMatrix< 3, 3, int >& iKernel );

public:
    QImage* GetEdgeDectionFromImage( const QImage* iImage );

private:
    QGenericMatrix< 3, 3, int > mKernel;
};

