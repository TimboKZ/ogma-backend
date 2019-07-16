//
// Created by euql1n on 7/15/19.
//

#ifndef OGMA_BACKEND_WEBSOCKET_H
#define OGMA_BACKEND_WEBSOCKET_H

#include <set>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <nlohmann/json.hpp>

#include "Config.h"

namespace Ogma {
    namespace ws = websocketpp;
    using SocketServer = ws::server<ws::config::asio>;
    using json = nlohmann::json;

    class WebSocket {
        private:
            Config config;
            SocketServer socket_server;
            std::set<ws::connection_hdl, std::owner_less<ws::connection_hdl>> connection_list;

            void on_open(const ws::connection_hdl& handle);
            void on_close(const ws::connection_hdl& handle);
            json process_request(const SocketServer::connection_ptr& connection, json action);
            void on_message(ws::connection_hdl handle, SocketServer::message_ptr msg);

        public:
            explicit WebSocket(Config config);
            virtual ~WebSocket();
            void start();

    };

}

#endif //OGMA_BACKEND_WEBSOCKET_H
