#include "sv.h"
//#include <argtable2.h>


client* create_client()
{
    client* cli = (client*)malloc(sizeof(client));
    cli->tcp_client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), cli->tcp_client);
    return cli;
}

void free_client(client* cli) {
    uv_close((uv_handle_t*)cli->tcp_client, on_close);
    free(cli);
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
    server->cli_list = NULL;
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
    /*list<client*>::iterator it;
    for (it = server->cli_list.begin(); it != server->cli_list.end(); it++) {
        if (*it != NULL) {
            freeclient(*it);
        }
    }
    server->cli_list.clear();*/
    /*qlist_obj_t* p_obj = NULL;
    while (server->cli_list->getnext(server->cli_list, p_obj, false) == true) {
        if (p_obj != NULL) {
            freeclient((client*)p_obj);
        }
    }*/

    node* nd = NULL;
    while (server->cli_list->getnext(server->cli_list, nd)) {
        client* cli = (client*)nd->data;
        uv_close((uv_handle_t*)cli->tcp_client,on_close);
        free(cli);
        nd->data = NULL;
    }
    freelist(server->cli_list);
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
//void on_connection(uv_stream_t* tcp_server, int status)
//{
//    // build connection;
//    if (status == -1) {
//        fprintf(stderr, "Failed to build new connection");
//        return;
//    }
//
//
//    
//    
//    client* cli = newclient();
//    if (uv_accept(tcp_server, (uv_stream_t*)cli->tcp_client) == 0) {
//        // connect to new client has been built
//        server->cli_list->addfirst(server->cli_list,cli,sizeof(client));
//
//        // Todo:read data 
//    }
//    else {
//        freeclient(cli);
//    }
//    
//    
//
//}