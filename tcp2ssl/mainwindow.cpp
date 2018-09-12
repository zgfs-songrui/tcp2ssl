#include "mainwindow.h"
#include <QVariant>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , settings_("tcp2ssl.conf", QSettings::IniFormat)
{
    ui.setupUi(this);

    local_port_ = settings_.value("tcp2ssl.local_port").toInt();
    ui.localPort->setText(QString().setNum(local_port_));

    remote_host_ = settings_.value("tcp2ssl.remote_host").toString();
    ui.remoteHost->setText(remote_host_);

    remote_port_ = settings_.value("tcp2ssl.remote_port").toInt();
    ui.remotePort->setText(QString().setNum(remote_port_));
}

MainWindow::~MainWindow()
{
    if (is_work)
        tcp2ssl_.Stop();
}

void MainWindow::on_start_clicked()
{
    if (is_work)
    {
        tcp2ssl_.Stop();
        is_work = false;
        ui.settingPanel->setEnabled(true);
        ui.start->setText(QString::fromWCharArray(L"¿ªÊ¼"));
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
                settings_.setValue("tcp2ssl.local_port", local_port_);
            }
            if (remote_host_ != remote_host)
            {
                remote_host_ = remote_host;
                settings_.setValue("tcp2ssl.remote_host", remote_host_);
            }
            if (remote_port != remote_port_)
            {
                remote_port_ = remote_port;
                settings_.setValue("tcp2ssl.remote_port", remote_port_);
            }
            ui.start->setText(QString::fromWCharArray(L"Í£Ö¹"));
            ui.settingPanel->setEnabled(false);
        }
    }
}

