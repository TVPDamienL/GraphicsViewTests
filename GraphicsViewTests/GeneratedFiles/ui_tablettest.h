/********************************************************************************
** Form generated from reading UI file 'tablettest.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABLETTEST_H
#define UI_TABLETTEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "tester/tablettest.h"

QT_BEGIN_NAMESPACE

class Ui_TabletTest
{
public:
    QHBoxLayout *horizontalLayout;
    TabletTester *tablettest;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *logView;
    QPushButton *pushButton;

    void setupUi(QWidget *TabletTest)
    {
        if (TabletTest->objectName().isEmpty())
            TabletTest->setObjectName(QString::fromUtf8("TabletTest"));
        TabletTest->resize(730, 385);
        horizontalLayout = new QHBoxLayout(TabletTest);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tablettest = new TabletTester(TabletTest);
        tablettest->setObjectName(QString::fromUtf8("tablettest"));

        horizontalLayout->addWidget(tablettest);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        logView = new QPlainTextEdit(TabletTest);
        logView->setObjectName(QString::fromUtf8("logView"));
        logView->setReadOnly(true);

        verticalLayout->addWidget(logView);

        pushButton = new QPushButton(TabletTest);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        retranslateUi(TabletTest);
        QObject::connect(pushButton, SIGNAL(clicked()), logView, SLOT(clear()));
        QObject::connect(pushButton, SIGNAL(clicked()), tablettest, SLOT(clear()));
        QObject::connect(tablettest, SIGNAL(eventReport(QString)), logView, SLOT(appendPlainText(QString)));

        QMetaObject::connectSlotsByName(TabletTest);
    } // setupUi

    void retranslateUi(QWidget *TabletTest)
    {
        TabletTest->setWindowTitle(QApplication::translate("TabletTest", "Tablet Tester", nullptr));
        pushButton->setText(QApplication::translate("TabletTest", "Clear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TabletTest: public Ui_TabletTest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABLETTEST_H
