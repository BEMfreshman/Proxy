#pragma once
#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include "List.h"



typedef struct client_ {
    uv_tcp_t* tcp_client;
} client;

typedef struct server_ {
    uv_tcp_t* tcp_server;
    list  cli_list;
} ser;


client* create_client();
void free_client(client* cli);
void on_close(uv_handle_t* handle);
void clear_server(ser* server);
ser*  create_server();
void free_server(ser* server);
void clear_cli_list(ser* server);