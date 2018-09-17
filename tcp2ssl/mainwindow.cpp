#include "mainwindow.h"
#include <QVariant>
#include <qdebug.h>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    , settings_("tcp2ssl.conf", QSettings::IniFormat)
{
    ui.setupUi(this);
    setFixedSize(size());

    local_port_ = settings_.value("tcp2ssl/local_port").toInt();
    ui.localPort->setText(QString().setNum(local_port_));

    remote_host_ = settings_.value("tcp2ssl/remote_host").toString();
    ui.remoteHost->setText(remote_host_);

    remote_port_ = settings_.value("tcp2ssl/remote_port").toInt();
    ui.remotePort->setText(QString().setNum(remote_port_));

    show_tray_ = settings_.value("tcp2ssl/show_tray", true).toBool();
    if (show_tray_)
    {
        ui.showTray->setChecked(true);
        on_showTray_clicked(true);
    }
}

MainWindow::~MainWindow()
{
    if (show_tray_ != ui.showTray->isChecked())
        settings_.setValue("tcp2ssl/show_tray", ui.showTray->isChecked());
    if (is_work)
        tcp2ssl_.Stop();
}

void MainWindow::on_showTray_clicked(bool checked)
{
    if (checked)
    {
        if (!system_tray_icon_)
        {
            system_tray_icon_ = new QSystemTrayIcon(QIcon(":/MainWindow/icon"), this);
            connect(system_tray_icon_, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger)
                {
                    showNormal();
                    activateWindow();
                }
            });

            QMenu* tray_menu = new QMenu(this);
            tray_opt_action = tray_menu->addAction(QString::fromWCharArray(is_work ? L"停止" : L"开始"), [&]() {on_start_clicked(); });
            tray_menu->addSeparator();
            tray_menu->addAction(QString::fromWCharArray(L"退出"), [&]() {close(); });
            system_tray_icon_->setContextMenu(tray_menu);
        }
        system_tray_icon_->show();
    }
    else if (system_tray_icon_)
    {
        system_tray_icon_->hide();
    }
}

void MainWindow::changeEvent(QEvent * event)
{
    if (event->type() == QEvent::WindowStateChange && windowState() == Qt::WindowMinimized && ui.showTray->isChecked())
        hide();

    QWidget::changeEvent(event);
}

void MainWindow::on_start_clicked()
{
    if (is_work)
    {
        tcp2ssl_.Stop();
        is_work = false;
        ui.settingPanel->setEnabled(true);
        ui.start->setText(QString::fromWCharArray(L"开始"));
    }
    else
    {
        int local_port = ui.localPort->text().toInt();
        QString remote_host = ui.remoteHost->text();
        int remote_port = ui.remotePort->text().toInt();

        if (tcp2ssl_.Start(local_port, remote_host.toStdString(), remote_port))
        {
            is_work = true;
            if (local_port != local_port_)
            {
                local_port_ = local_port;
                settings_.setValue("tcp2ssl/local_port", local_port_);
            }
            if (remote_host_ != remote_host)
            {
                remote_host_ = remote_host;
                settings_.setValue("tcp2ssl/remote_host", remote_host_);
            }
            if (remote_port != remote_port_)
            {
                remote_port_ = remote_port;
                settings_.setValue("tcp2ssl/remote_port", remote_port_);
            }
            ui.start->setText(QString::fromWCharArray(L"停止"));
            ui.settingPanel->setEnabled(false);
        }
    }
    if (tray_opt_action)
        tray_opt_action->setText(QString::fromWCharArray(is_work?L"停止":L"开始"));
}

