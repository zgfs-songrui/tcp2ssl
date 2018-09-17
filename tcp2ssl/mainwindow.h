#pragma once

#include <QtWidgets/QWidget>
#include <QSettings>
#include <QSystemTrayIcon>
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
    void on_showTray_clicked(bool checked);
private:
    void changeEvent(QEvent *event);
private:
    Ui::MainWindowClass ui;
    Tcp2Ssl tcp2ssl_;
    QSettings settings_;

    int local_port_;
    QString remote_host_;
    int remote_port_;
    bool show_tray_;

    QSystemTrayIcon* system_tray_icon_ = nullptr;
    QAction* tray_opt_action = nullptr;
    bool is_work = false;
};
