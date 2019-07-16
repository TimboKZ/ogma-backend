#include <utility>

//
// Created by euql1n on 7/15/19.
//

#include <math.h>
#include <boost/algorithm/string.hpp>

#include "WebSocket.h"

using namespace std;
using namespace Ogma;
namespace ph = ws::lib::placeholders;

WebSocket::WebSocket(Config _config) : config(std::move(_config)) {
    socket_server.init_asio();
    socket_server.set_reuse_addr(true);
    socket_server.clear_access_channels(ws::log::alevel::all ^ ws::log::alevel::fail);

    socket_server.set_open_handler(ws::lib::bind(&WebSocket::on_open, this, ph::_1));
    socket_server.set_close_handler(ws::lib::bind(&WebSocket::on_close, this, ph::_1));
    socket_server.set_message_handler(ws::lib::bind(&WebSocket::on_message, this, ph::_1, ph::_2));
}

void WebSocket::on_open(const ws::connection_hdl &handle) {
    connection_list.insert(handle);
    cout << "Added handle!" << endl;
}

void WebSocket::on_close(const ws::connection_hdl &handle) {
    connection_list.erase(handle);
    cout << "Removed handle!" << endl;
}


void WebSocket::on_message(ws::connection_hdl handle, SocketServer::message_ptr msg) {
    vector<string> parts;

    auto requestAction = json::parse(msg->get_payload());

    json responseAction;
    try {
        SocketServer::connection_ptr connection = socket_server.get_con_from_hdl(handle);
        responseAction = this->process_request(connection, requestAction);
    } catch (const std::exception &e) {
        responseAction["error"] = e.what();
    }

    if (requestAction.find("id") != requestAction.end()) {
        responseAction["id"] = requestAction["id"];
    }

    socket_server.send(std::move(handle), responseAction.dump(), msg->get_opcode());
}

json get_client_details(const SocketServer::connection_ptr &connection) {
    json details;
    details["id"] = connection->get_host();
    details["ip"] = connection->get_host();
    details["localClient"] = false; // TODO: Compare to IP of the server
    details["userAgent"] = connection->get_request_header("user-agent");
    return details;
}

json WebSocket::process_request(const SocketServer::connection_ptr &connection, json action) {
    cout << "[IPC] Received action: " << action << endl;
    if (action.find("name") == action.end()) throw runtime_error("Request action has no name specified!");
    string name = action["name"];

    json payload;

    if (name == "getClientDetails") {
        payload = get_client_details(connection);
    } else if (name == "getClientList") {
        payload = json::array();
        cout << "Connection count: " << connection_list.size() << endl;
        for (auto &clientHandle : connection_list) {
            payload.emplace_back(get_client_details(socket_server.get_con_from_hdl(clientHandle)));
        }
    } else if (name == "getSummaries") {
        payload = json::array();
    } else {
        throw runtime_error("Action " + name + " is not supported .");
    }

    json responseAction;
    responseAction["name"] = name;
    responseAction["payload"] = payload;
    return responseAction;
}

void WebSocket::start() {
    thread socket_thread([this]() {
        socket_server.listen(config.socket_server_port);
        socket_server.start_accept();
        socket_server.run();
    });
    socket_thread.join();
    this_thread::sleep_for(chrono::seconds(1));
}

WebSocket::~WebSocket() {
    socket_server.stop_listening();
    socket_server.stop();
}

