#pragma once
#include <memory>
#include <thread>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <QObject>
#include "qdebug.h"

#define BUFFER_SIZE 1024*1024


class Tcp2Ssl : public QObject
{
    Q_OBJECT

public:
    Tcp2Ssl();
    ~Tcp2Ssl();

    bool Start(int local_port, const std::string& remote_host, int remote);
    void Stop();
private:
    void StartAccept();
    struct Conn;
    void StartUp(const std::shared_ptr<Conn>& conn);
    void StartDown(const std::shared_ptr<Conn>& conn);
    void Broken(const std::shared_ptr<Conn>& conn, bool is_local);
private:
    struct Conn
    {
        Conn(asio::io_service & io_service)
            : local_socket(io_service)
            , c(asio::ssl::context::sslv23)
            , remote_socket(io_service,c)
        {
        }
        ~Conn()
        {
        }
        asio::ssl::context c;
        asio::ip::tcp::socket local_socket;
        asio::ssl::stream< asio::ip::tcp::socket> remote_socket;
        uint8_t up_buffer[BUFFER_SIZE];
        uint8_t down_buffer[BUFFER_SIZE];
        bool is_stop = false;
    };

    asio::io_service io_service_;
    asio::io_service::work io_service_work_;
    asio::ip::tcp::acceptor acceptor_;

    asio::ip::tcp::endpoint remote_endpoint_;

    std::thread thread_;
};
