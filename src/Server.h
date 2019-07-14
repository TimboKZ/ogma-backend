//
// Created by euql1n on 7/14/19.
//

#ifndef OGMA_BACKEND_SERVER_H
#define OGMA_BACKEND_SERVER_H

#include <server_http.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "Config.h"

namespace Ogma {

    namespace ws = websocketpp;

    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
    using SocketServer = ws::server<ws::config::asio>;

    class Server {

        private:
            Config config;
            HttpServer web_server;
            SocketServer socket_server;

            void on_message(websocketpp::connection_hdl hdl, SocketServer::message_ptr msg);

        public:
            explicit Server(Config config);
            void start();

    };

}

#endif //OGMA_BACKEND_SERVER_H
