/********************************************************************************
** Form generated from reading UI file 'cMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMAINWINDOW_H
#define UI_CMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "Canvas.h"
#include "ColorSwatch.h"
#include "cCustomGraphicsView.h"

QT_BEGIN_NAMESPACE

class Ui_cMainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *previewLabel;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QFormLayout *formLayout;
    QLabel *penSizeLabel;
    QSpinBox *penSizeSpinBox;
    QLabel *antiAliasLabel;
    QCheckBox *antiAliasCheckBox;
    QLabel *label;
    ColorSwatch *colorSwatch;
    QLabel *stepLabel;
    QSpinBox *stepSpinBox;
    QLabel *opacityLabel;
    QSpinBox *opacitySpinBox;
    QSpacerItem *verticalSpacer;
    QLabel *label_3;
    QGridLayout *gridLayout;
    QPushButton *buttonToolPaint;
    QPushButton *buttonToolSelect;
    QPushButton *buttonRectangle;
    cCanvas *canvas;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QDoubleSpinBox *spinZoomFactor;
    QHBoxLayout *horizontalLayout;
    QPushButton *playButton;
    QPushButton *stopButton;
    cCustomGraphicsView *graphicsView;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *cMainWindow)
    {
        if (cMainWindow->objectName().isEmpty())
            cMainWindow->setObjectName(QString::fromUtf8("cMainWindow"));
        cMainWindow->resize(955, 826);
        centralWidget = new QWidget(cMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        previewLabel = new QLabel(centralWidget);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        previewLabel->setMinimumSize(QSize(0, 0));
        previewLabel->setMaximumSize(QSize(16777215, 16777215));
        previewLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(previewLabel);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(label_2);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        penSizeLabel = new QLabel(centralWidget);
        penSizeLabel->setObjectName(QString::fromUtf8("penSizeLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, penSizeLabel);

        penSizeSpinBox = new QSpinBox(centralWidget);
        penSizeSpinBox->setObjectName(QString::fromUtf8("penSizeSpinBox"));
        penSizeSpinBox->setMaximum(9999);

        formLayout->setWidget(0, QFormLayout::FieldRole, penSizeSpinBox);

        antiAliasLabel = new QLabel(centralWidget);
        antiAliasLabel->setObjectName(QString::fromUtf8("antiAliasLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, antiAliasLabel);

        antiAliasCheckBox = new QCheckBox(centralWidget);
        antiAliasCheckBox->setObjectName(QString::fromUtf8("antiAliasCheckBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, antiAliasCheckBox);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        colorSwatch = new ColorSwatch(centralWidget);
        colorSwatch->setObjectName(QString::fromUtf8("colorSwatch"));

        formLayout->setWidget(2, QFormLayout::FieldRole, colorSwatch);

        stepLabel = new QLabel(centralWidget);
        stepLabel->setObjectName(QString::fromUtf8("stepLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, stepLabel);

        stepSpinBox = new QSpinBox(centralWidget);
        stepSpinBox->setObjectName(QString::fromUtf8("stepSpinBox"));
        stepSpinBox->setMinimum(1);
        stepSpinBox->setMaximum(99999);

        formLayout->setWidget(3, QFormLayout::FieldRole, stepSpinBox);

        opacityLabel = new QLabel(centralWidget);
        opacityLabel->setObjectName(QString::fromUtf8("opacityLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, opacityLabel);

        opacitySpinBox = new QSpinBox(centralWidget);
        opacitySpinBox->setObjectName(QString::fromUtf8("opacitySpinBox"));
        opacitySpinBox->setMaximum(100);

        formLayout->setWidget(4, QFormLayout::FieldRole, opacitySpinBox);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        buttonToolPaint = new QPushButton(centralWidget);
        buttonToolPaint->setObjectName(QString::fromUtf8("buttonToolPaint"));

        gridLayout->addWidget(buttonToolPaint, 0, 0, 1, 1);

        buttonToolSelect = new QPushButton(centralWidget);
        buttonToolSelect->setObjectName(QString::fromUtf8("buttonToolSelect"));

        gridLayout->addWidget(buttonToolSelect, 0, 1, 1, 1);

        buttonRectangle = new QPushButton(centralWidget);
        buttonRectangle->setObjectName(QString::fromUtf8("buttonRectangle"));

        gridLayout->addWidget(buttonRectangle, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        canvas = new cCanvas(centralWidget);
        canvas->setObjectName(QString::fromUtf8("canvas"));

        horizontalLayout_2->addWidget(canvas);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        spinZoomFactor = new QDoubleSpinBox(centralWidget);
        spinZoomFactor->setObjectName(QString::fromUtf8("spinZoomFactor"));

        horizontalLayout_4->addWidget(spinZoomFactor);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        playButton = new QPushButton(centralWidget);
        playButton->setObjectName(QString::fromUtf8("playButton"));

        horizontalLayout->addWidget(playButton);

        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));

        horizontalLayout->addWidget(stopButton);


        verticalLayout_2->addLayout(horizontalLayout);

        graphicsView = new cCustomGraphicsView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy1);
        graphicsView->setMaximumSize(QSize(16777215, 200));

        verticalLayout_2->addWidget(graphicsView);

        cMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(cMainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 955, 21));
        cMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(cMainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        cMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(cMainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        cMainWindow->setStatusBar(statusBar);

        retranslateUi(cMainWindow);

        QMetaObject::connectSlotsByName(cMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *cMainWindow)
    {
        cMainWindow->setWindowTitle(QApplication::translate("cMainWindow", "cMainWindow", nullptr));
        previewLabel->setText(QApplication::translate("cMainWindow", "PREVIEW", nullptr));
        label_2->setText(QApplication::translate("cMainWindow", "ToolConfig", nullptr));
        penSizeLabel->setText(QApplication::translate("cMainWindow", "PenSize", nullptr));
        antiAliasLabel->setText(QApplication::translate("cMainWindow", "AntiAlias", nullptr));
        label->setText(QApplication::translate("cMainWindow", "Color", nullptr));
        colorSwatch->setText(QString());
        stepLabel->setText(QApplication::translate("cMainWindow", "Step", nullptr));
        opacityLabel->setText(QApplication::translate("cMainWindow", "Opacity", nullptr));
        label_3->setText(QApplication::translate("cMainWindow", "Tools", nullptr));
        buttonToolPaint->setText(QApplication::translate("cMainWindow", "Paint", nullptr));
        buttonToolSelect->setText(QApplication::translate("cMainWindow", "Select", nullptr));
        buttonRectangle->setText(QApplication::translate("cMainWindow", "Rectangle", nullptr));
        playButton->setText(QApplication::translate("cMainWindow", "Play", nullptr));
        stopButton->setText(QApplication::translate("cMainWindow", "Stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cMainWindow: public Ui_cMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMAINWINDOW_H
