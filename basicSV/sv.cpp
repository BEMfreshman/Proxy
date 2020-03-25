#include "sv.h"


client* create_client()
{
    client* cli = (client*)malloc(sizeof(client));
    cli->tcp_client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), cli->tcp_client);
    return cli;
}

void free_client(client* cli) {
    if (cli == NULL) return;
    uv_close((uv_handle_t*)cli->tcp_client, on_close);
    free(cli);
}

client* getclient(ser* server, uv_tcp_t* tcp_client) {
    if (server == NULL) return NULL;
    list<client*> lt = server->cli_list;
    for (list<client*>::iterator it = lt.begin();
        it != lt.end();
        it++) {
        if ((*it)->tcp_client == tcp_client) {
            return *it;
        }
    }

    return NULL;
}

void add_client(ser* server, client* cli) {
    server->cli_list.push_back(cli);
}

client* get_client(ser* server, size_t index) {
    list<client*> lt = server->cli_list;
    int counter = 0;
    for (list<client*>::iterator it = lt.begin();
        it != lt.end();
        it++) {
        if (counter != index) {
            counter++;
        }
        else {
            return *it;
        }
    }

    return NULL;
}

void pop_client(ser* server, int index) {
    list<client*> lt = server->cli_list;
    int counter = 0;
    for (list<client*>::iterator it = lt.begin();
        it != lt.end();
        it++) {
        if (counter != index) {
            counter++;
        }
        else {
            client* tmp = *it;
            it = lt.erase(it);
            free_client(tmp);
        }
    }
}

void pop_client2(ser* server, client* cli) {

    list<client*> lt = server->cli_list;
    int counter = 0;
    for (list<client*>::iterator it = lt.begin();
        it != lt.end();
        it++) {
        if (cli == *it) {
            client* tmp = *it;
            it = lt.erase(it);
            free_client(tmp);
        }
    }
}

void on_close(uv_handle_t* handle){
    free(handle);
}

void clear_server(ser* server) {
    if (server != NULL) {
        uv_close((uv_handle_t*)server->tcp_server, on_close);
        clear_cli_list(server);
    }
}

ser* create_server() {
    
    ser* server = (ser*)malloc(sizeof(ser));
    server->tcp_server = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), server->tcp_server);
    return server;
    
}

void free_server(ser* server){
    if (server == NULL) return;
    uv_close((uv_handle_t*)server->tcp_server, on_close);
    clear_cli_list(server);
}


void clear_cli_list(ser* server) {
    list<client*> lt = server->cli_list;
    for (list<client*>::iterator it = lt.begin();
        it != lt.end();
        it++) {
        if (*it != NULL) {
            free_client(*it);
        }
    }
    server->cli_list.clear();
}

//
//struct arg_lit *help, *version;
//struct arg_str* ip;
//struct arg_int *port;
//struct arg_end *end;


//void on_connection(uv_stream_t* server, int status);
//void on_read();
//void on_write();
//
//
//int main(int argc, char* argv[]) {
//    void *argtable[] = {
//        help = arg_litn(NULL,"help",0,1,"display the help and exit"),
//        version = arg_litn(NULL,"version",0,1,"display the version info"),
// //       ip = arg_litn(NULL,"remote ip",0,1,"set proxy server ip"),
//        ip = arg_strn(NULL,"remote ip","string",0,1,"set proxy server ip"),
//        port = arg_intn(NULL,"remote port","integer",0,1,"set proxy server port"),
//    };
//
//    int nerrors = arg_parse(argc, argv, argtable);
//
//    if (help->count > 0 || nerrors > 0 || port ->count == 0) {
//        fprintf(stderr, "Usage: Server ");
//        arg_print_syntax(stderr, argtable, "\n");
//
//        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
//        return 0;
//    }
//
//    const char* hostip = ip->sval[0];
//    short hostport = *(port->ival);
//
//    init_server();
//
//    struct sockaddr_in seraddr;
//    uv_ip4_addr(hostip, hostport, &seraddr);
//
//    uv_tcp_bind(server->tcp_server, (const struct sockaddr*)&seraddr, 0);
//    uv_listen((uv_stream_t*)server->tcp_server, 128, on_connection);
//
//    clear_server();
//    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
//    return 0;
//
//}
//
//
//