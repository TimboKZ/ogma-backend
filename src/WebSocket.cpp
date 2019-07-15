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

void WebSocket::on_open(ws::connection_hdl hdl) {
    connection_list.insert(hdl);
}

void WebSocket::on_close(ws::connection_hdl hdl) {
    connection_list.erase(hdl);
}

void WebSocket::on_message(ws::connection_hdl hdl, SocketServer::message_ptr msg) {
    vector<string> parts;

    string payload(msg->get_payload());

    auto name_split_i = -1;
    auto data_split_i = -1;
    for (auto i = 0; i < payload.length(); ++i) {
        auto c = payload.at(i);
        if (c == '#') name_split_i = i;
        else if (c == '@') {
            data_split_i = i;
            break;
        }
    }

    // Extract the data string (if any)
    string data = data_split_i != -1 ? payload.substr(data_split_i + 1) : "";

    // Extract the request ID string (if any)
    string reqId = name_split_i != -1 ? payload.substr(0, name_split_i) : "";

    // Extract the name of the action
    auto nameLength = max(data_split_i, int(payload.length())) - (name_split_i + 1);
    string name = payload.substr(name_split_i + 1, nameLength);

    cout << "Received request <" + name + "> with ID " + reqId + " and data " + data + "." << endl;

    // Prepare response and send it back
    string responseData; // TODO: Prepare response here
    string response;
    if (reqId.length() >= 1) response = reqId + "#" + name;
    else response = name;
    if (responseData.length() >= 1) response += "@" + responseData;

    socket_server.send(hdl, response, msg->get_opcode());
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

