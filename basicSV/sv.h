#pragma once
#include <uv.h>
//#include "struct_sv.h"

#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>

using std::list;

struct client {
    uv_tcp_t* tcp_client;

};

struct ser {
    uv_tcp_t* tcp_server;
    list<client*>  cli_list;
};

void on_close(uv_handle_t* handle);

client* create_client();
void add_client(ser* server,client* cli);
client* get_client(ser* server, size_t index);
void pop_client(ser* server, int index);
void pop_client2(ser* server, client* cli);
void free_client(client* cli);

client* getclient(ser* server, uv_tcp_t* tcp_client);


void clear_server(ser* server);
ser*  create_server();
void free_server(ser* server);
void clear_cli_list(ser* server);