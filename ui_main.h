#ifndef UI_H
#define UI_H

#include "QMainWindow"
#include <QToolBar>
#include <QTableWidget>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>
#include <QStatusBar>

class Ui_Main {
public:
    QWidget* centralWidget;
    QToolBar* toolBar;
    QTableWidget* tableWidget;
    QBoxLayout* boxLayout;
    QStatusBar* statusBar;
    QPushButton* sendingButton;
    QPushButton* refreshButton;

    static const int _5_SECONDS_IN_MS = 5000;

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

        sendingButton = new QPushButton(toolBar);
        sendingButton->setText("Отправить");
        toolBar->addWidget(sendingButton);

        refreshButton = new QPushButton(toolBar);
        refreshButton->setText("Обновить");
        toolBar->addWidget(refreshButton);

        statusBar = new QStatusBar(mainWindow);
        statusBar->setObjectName("statusBar");
        statusBar->show();

        boxLayout->addWidget(tableWidget);

        mainWindow->setStatusBar(statusBar);
        mainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        mainWindow->setCentralWidget(centralWidget);
    }
};

#endif // UI_H
