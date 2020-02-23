#ifndef UI_H
#define UI_H

#include "QMainWindow"
#include <QToolBar>
#include <QTableWidget>
#include <QMenuBar>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>

class Ui_Main {
public:
    QWidget* centralWidget;
    QToolBar* toolBar;
    QMenuBar* menuBar;
    QTableWidget* tableWidget;
    QBoxLayout* boxLayout;

    void setupUi(QMainWindow* mainWindow) {
        if (mainWindow->objectName().isEmpty()) {
            mainWindow->setObjectName("MainWindow");
        }
        mainWindow->resize(400, 300);
        centralWidget = new QWidget(mainWindow);
        centralWidget->setObjectName("centralWidget");

        boxLayout = new QBoxLayout(QBoxLayout::TopToBottom, centralWidget);

        tableWidget = new QTableWidget(centralWidget);
        tableWidget->setObjectName("tableWidget");

        toolBar = new QToolBar(centralWidget);
        toolBar->setObjectName("toolBar");
        toolBar->setMovable(false);

        QPushButton* testButton = new QPushButton();
        toolBar->addWidget(testButton);

        menuBar = new QMenuBar(mainWindow);
        menuBar->setObjectName("menuBar");
//        menuBar->setGeometry(0, 0, 400, 20);
        menuBar->addMenu("Test menu");

        boxLayout->addWidget(tableWidget);

        mainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        mainWindow->setMenuBar(menuBar);
        mainWindow->setCentralWidget(centralWidget);
    }
};

#endif // UI_H
