 #pragma once

#include "connection.hpp"
#include "dmp_sender.hpp"
#include "dmp_receiver.hpp"

#include "ui_interface.hpp"
#include "dmp_client_interface.hpp"

class DmpClient : public DmpClientInterface
{
    std::string name;
    dmp::Connection connection;
    message::Ping last_sent_ping;
    dmp_library::Library lib;

    std::shared_ptr<UiInterface> ui;

    DmpSender sender;
    DmpReceiver receiver;

    message::DmpCallbacks callbacks;

public:

    DmpClient(std::string name, dmp::Connection&& conn);
    DmpClient(DmpClient&&) = default;

    void set_ui(std::shared_ptr<UiInterface> new_ui)
    {
        ui = new_ui;
    }

    void handle_request(message::Type t);
    void listen_requests();

    void stop() final;
    void run() final;
    void search(std::string query) final;
    void index(std::string path) final;
    message::DmpCallbacks& get_callbacks() final;
    void send_bye() final;

    void handle_ping(message::Ping ping);
    void handle_name_request(message::NameRequest name_req);
    void handle_pong(message::Pong pong);
    void handle_search_request(message::SearchRequest search_req);
    void handle_search_response(message::SearchResponse search_res);
};