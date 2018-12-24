/********************************************************************************
** Form generated from reading UI file 'cMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMAINWINDOW_H
#define UI_CMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
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
    cCanvas *canvas;
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
            cMainWindow->setObjectName(QStringLiteral("cMainWindow"));
        cMainWindow->resize(649, 589);
        centralWidget = new QWidget(cMainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        previewLabel = new QLabel(centralWidget);
        previewLabel->setObjectName(QStringLiteral("previewLabel"));
        previewLabel->setMinimumSize(QSize(0, 0));
        previewLabel->setMaximumSize(QSize(16777215, 16777215));
        previewLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(previewLabel);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(label_2);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        penSizeLabel = new QLabel(centralWidget);
        penSizeLabel->setObjectName(QStringLiteral("penSizeLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, penSizeLabel);

        penSizeSpinBox = new QSpinBox(centralWidget);
        penSizeSpinBox->setObjectName(QStringLiteral("penSizeSpinBox"));
        penSizeSpinBox->setMaximum(9999);

        formLayout->setWidget(0, QFormLayout::FieldRole, penSizeSpinBox);

        antiAliasLabel = new QLabel(centralWidget);
        antiAliasLabel->setObjectName(QStringLiteral("antiAliasLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, antiAliasLabel);

        antiAliasCheckBox = new QCheckBox(centralWidget);
        antiAliasCheckBox->setObjectName(QStringLiteral("antiAliasCheckBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, antiAliasCheckBox);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        colorSwatch = new ColorSwatch(centralWidget);
        colorSwatch->setObjectName(QStringLiteral("colorSwatch"));

        formLayout->setWidget(2, QFormLayout::FieldRole, colorSwatch);

        stepLabel = new QLabel(centralWidget);
        stepLabel->setObjectName(QStringLiteral("stepLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, stepLabel);

        stepSpinBox = new QSpinBox(centralWidget);
        stepSpinBox->setObjectName(QStringLiteral("stepSpinBox"));
        stepSpinBox->setMinimum(1);
        stepSpinBox->setMaximum(99999);

        formLayout->setWidget(3, QFormLayout::FieldRole, stepSpinBox);


        verticalLayout->addLayout(formLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        canvas = new cCanvas(centralWidget);
        canvas->setObjectName(QStringLiteral("canvas"));

        horizontalLayout_2->addWidget(canvas);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        playButton = new QPushButton(centralWidget);
        playButton->setObjectName(QStringLiteral("playButton"));

        horizontalLayout->addWidget(playButton);

        stopButton = new QPushButton(centralWidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));

        horizontalLayout->addWidget(stopButton);


        verticalLayout_2->addLayout(horizontalLayout);

        graphicsView = new cCustomGraphicsView(centralWidget);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy1);
        graphicsView->setMaximumSize(QSize(16777215, 200));

        verticalLayout_2->addWidget(graphicsView);

        cMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(cMainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 649, 21));
        cMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(cMainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        cMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(cMainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
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
        playButton->setText(QApplication::translate("cMainWindow", "Play", nullptr));
        stopButton->setText(QApplication::translate("cMainWindow", "Stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class cMainWindow: public Ui_cMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMAINWINDOW_H
