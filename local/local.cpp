
#include "sv.h"
#include <stdlib.h>
#include <stdio.h>
#include <argtable3.h>

#include "localuti.h"
#include "sv.h"

struct arg_lit *help, *version;
struct arg_str* ip;
struct arg_int *port;
struct arg_end *end;

ser* localss = NULL;

int main(int argc, char* argv[]) {

    void *argtable[] = {
        help = arg_litn(NULL,"help",0,1,"display the help and exit"),
        version = arg_litn(NULL,"version",0,1,"display the version info"),
        //       ip = arg_litn(NULL,"remote ip",0,1,"set proxy server ip"),
               ip = arg_strn(NULL,"remote ip","string",0,1,"set proxy server ip"),
               port = arg_intn(NULL,"remote port","integer",0,1,"set proxy server port"),
    };

    int nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0 || nerrors > 0 || port->count == 0) {
        fprintf(stderr, "Usage: Server ");
        arg_print_syntax(stderr, argtable, "\n");

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 0;
    }

    const char* hostip = ip->sval[0];
    short hostport = *(port->ival);

    localss = create_server();

    struct sockaddr_in seraddr;
    uv_ip4_addr(hostip, hostport, &seraddr);

    uv_tcp_bind(localss->tcp_server, (const struct sockaddr*)&seraddr, 0);
    uv_listen((uv_stream_t*)localss->tcp_server, 128, on_connection);

    clear_server(localss);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;

}

