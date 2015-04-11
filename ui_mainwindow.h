/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QGridLayout *gridLayoutArrange;
    QToolButton *upButton;
    QToolButton *downButton;
    QSpacerItem *verticalSpacerUp;
    QSpacerItem *verticalSpacerDown;
    QSpacerItem *verticalSpacer;
    QTreeWidget *treeWidget;
    QGridLayout *gridLayoutCommand;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QPushButton *loadButton;
    QPushButton *saveButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(400, 300);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayoutArrange = new QGridLayout();
        gridLayoutArrange->setSpacing(6);
        gridLayoutArrange->setObjectName(QStringLiteral("gridLayoutArrange"));
        upButton = new QToolButton(centralWidget);
        upButton->setObjectName(QStringLiteral("upButton"));

        gridLayoutArrange->addWidget(upButton, 1, 0, 1, 1);

        downButton = new QToolButton(centralWidget);
        downButton->setObjectName(QStringLiteral("downButton"));

        gridLayoutArrange->addWidget(downButton, 2, 0, 1, 1);

        verticalSpacerUp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayoutArrange->addItem(verticalSpacerUp, 0, 0, 1, 1);

        verticalSpacerDown = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayoutArrange->addItem(verticalSpacerDown, 3, 0, 1, 1);


        gridLayout->addLayout(gridLayoutArrange, 3, 4, 1, 1);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);

        treeWidget = new QTreeWidget(centralWidget);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setSortingEnabled(true);

        gridLayout->addWidget(treeWidget, 3, 0, 1, 4);

        gridLayoutCommand = new QGridLayout();
        gridLayoutCommand->setSpacing(6);
        gridLayoutCommand->setObjectName(QStringLiteral("gridLayoutCommand"));
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
        comboBox->setSizePolicy(sizePolicy);
        comboBox->setMinimumSize(QSize(200, 0));

        gridLayoutCommand->addWidget(comboBox, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayoutCommand->addItem(horizontalSpacer, 0, 1, 1, 1);

        loadButton = new QPushButton(centralWidget);
        loadButton->setObjectName(QStringLiteral("loadButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(loadButton->sizePolicy().hasHeightForWidth());
        loadButton->setSizePolicy(sizePolicy1);

        gridLayoutCommand->addWidget(loadButton, 0, 2, 1, 1);

        saveButton = new QPushButton(centralWidget);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        sizePolicy1.setHeightForWidth(saveButton->sizePolicy().hasHeightForWidth());
        saveButton->setSizePolicy(sizePolicy1);

        gridLayoutCommand->addWidget(saveButton, 0, 3, 1, 1);


        gridLayout->addLayout(gridLayoutCommand, 0, 0, 1, 5);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "FATOrderFile \302\251 2015 Garou @GBAtemp", 0));
        upButton->setText(QApplication::translate("MainWindow", "...", 0));
        downButton->setText(QApplication::translate("MainWindow", "...", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(7, QApplication::translate("MainWindow", "Date Modified", 0));
        ___qtreewidgetitem->setText(6, QApplication::translate("MainWindow", "Date Last Accessed", 0));
        ___qtreewidgetitem->setText(5, QApplication::translate("MainWindow", "Date Created", 0));
        ___qtreewidgetitem->setText(4, QApplication::translate("MainWindow", "Size", 0));
        ___qtreewidgetitem->setText(3, QApplication::translate("MainWindow", "Type", 0));
        ___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "Title ID", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Title Name", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "File Name", 0));
        loadButton->setText(QApplication::translate("MainWindow", "&Load", 0));
        saveButton->setText(QApplication::translate("MainWindow", "&Save", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
