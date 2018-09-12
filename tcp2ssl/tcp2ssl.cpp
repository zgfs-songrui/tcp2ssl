#include "tcp2ssl.h"
Tcp2Ssl::Tcp2Ssl()
    : io_service_work_(io_service_)
    , acceptor_(io_service_)
{
}

Tcp2Ssl::~Tcp2Ssl()
{

}

bool Tcp2Ssl::Start(int local_port, const std::string & remote_host, int remote_port)
{
    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), local_port);
        remote_endpoint_ = asio::ip::tcp::endpoint(
            asio::ip::address::from_string(remote_host), remote_port);
        acceptor_.open(endpoint.protocol());
        acceptor_.bind(endpoint);
        acceptor_.listen();
    }
    catch (...)
    {
        return false;
    }
    StartAccept();
    io_service_.reset();
    thread_ = std::thread([=]() {io_service_.run(); });
    return true;
}

void Tcp2Ssl::Stop()
{
    acceptor_.close();
    io_service_.stop();
    thread_.join();
}

void Tcp2Ssl::StartAccept()
{
    auto con = std::make_shared<Conn>(io_service_);
    acceptor_.async_accept(con->local_socket, [=](const asio::error_code& ec) {
        if (!ec)
        {
            con->remote_socket.lowest_layer().async_connect(remote_endpoint_, [=](const asio::error_code& ec) {
                if (!ec)
                {
                    con->remote_socket.handshake(asio::ssl::stream< asio::ip::tcp::socket>::client);
                    StartUp(con);
                    StartDown(con);
                }
                else
                {
                    qDebug() << "remote connect failed" << ec.message().c_str();
                }
            });
            StartAccept();
        }
        else
        {
        }
    });
}

void Tcp2Ssl::StartUp(const std::shared_ptr<Conn>& conn)
{
    conn->local_socket.async_read_some(asio::buffer(conn->up_buffer, BUFFER_SIZE),
        [=](const asio::error_code& ec, std::size_t size) {
        if (ec)
        {
            qDebug() << "local recv failed" << ec.message().c_str();
            Broken(conn, true);
            return;
        }
        else if (conn->is_stop)
        {
            qDebug() << "local close";
            asio::error_code ec;
            conn->local_socket.close(ec);
            return;
        }
        asio::async_write(conn->remote_socket, asio::buffer(conn->up_buffer, size),
            [=](const asio::error_code& ec, std::size_t size) {
            if (ec)
            {
                qDebug() << "remote send failed" << ec.message().c_str();
                Broken(conn, false);
                return;
            }
            else if (conn->is_stop)
            {
                qDebug() << "remote close";
                conn->remote_socket.async_shutdown(
                    [=](const asio::error_code&) {
                    asio::error_code ec; conn->remote_socket.lowest_layer().close(ec); });
                return;
            }
            StartUp(conn);
        });
    });
}

void Tcp2Ssl::StartDown(const std::shared_ptr<Conn>& conn)
{
    conn->remote_socket.async_read_some(asio::buffer(conn->down_buffer, BUFFER_SIZE),
        [=](const asio::error_code& ec, std::size_t size) {
        if (ec)
        {
            qDebug() << "remote recv failed" << ec.message().c_str();
            Broken(conn, false);
            return;
        }

        else if (conn->is_stop)
        {
            qDebug() << "remote close";
            conn->remote_socket.async_shutdown(
                [=](const asio::error_code&) {
                asio::error_code ec; conn->remote_socket.lowest_layer().close(ec); });
            return;
        }
        asio::async_write(conn->local_socket, asio::buffer(conn->down_buffer, size),
            [=](const asio::error_code& ec, std::size_t size) {
            if (ec)
            {
                qDebug() << "loacl recv failed" << ec.message().c_str();
                Broken(conn, true);
                return;
            }
            else if (conn->is_stop)
            {
                qDebug() << "loacl close";
                asio::error_code ec;
                conn->local_socket.close(ec);
                return;
            }
            StartDown(conn);
        });
    });
}

void Tcp2Ssl::Broken(const std::shared_ptr<Conn>& conn, bool is_local)
{
    if (conn->is_stop)
        return;
    conn->is_stop = true;

    asio::error_code ec;
    if (is_local)
        conn->local_socket.close(ec);
    else
        conn->remote_socket.async_shutdown(
            [=](const asio::error_code&) {
        asio::error_code ec; conn->remote_socket.lowest_layer().close(ec); });

}
