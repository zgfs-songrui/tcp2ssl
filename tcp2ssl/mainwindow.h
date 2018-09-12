#pragma once

#include <QtWidgets/QWidget>
#include <QSettings>
#include "ui_mainwindow.h"
#include "tcp2ssl.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private slots:
    void on_start_clicked();
private:
    Ui::MainWindowClass ui;
    Tcp2Ssl tcp2ssl_;
    QSettings settings_;

    int local_port_;
    QString remote_host_;
    int remote_port_;

    bool is_work = false;
};
