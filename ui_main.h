#ifndef UI_H
#define UI_H

#include "QMainWindow"
#include <QToolBar>
#include <QTableWidget>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>
#include <QStatusBar>
#include <QHeaderView>

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
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->horizontalHeader()->setVisible(false);

        toolBar = new QToolBar(centralWidget);
        toolBar->setObjectName("toolBar");
        toolBar->setMovable(false);

        QSize buttonSize(25, 30);

        sendingButton = new QPushButton(toolBar);
        sendingButton->setText("Отправить");
        sendingButton->setIcon(QIcon(":/images/send_64px.png"));
        sendingButton->setIconSize(buttonSize);
        toolBar->addWidget(sendingButton);

        toolBar->addSeparator();

        refreshButton = new QPushButton(toolBar);
        refreshButton->setText("Обновить");
        refreshButton->setIcon(QIcon(":/images/refresh_64px.png"));
        refreshButton->setIconSize(buttonSize);
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
