
#include "sv.h"
#include <stdlib.h>
#include <stdio.h>
#include <argtable3.h>

#include "localuti.h"
#include "sv.h"

struct arg_lit *help, *version;
struct arg_str* ip;
struct arg_str* encryname;
struct arg_int *port;
struct arg_end *end;

GList* lcllist = NULL;
profile* pf = NULL;
uv_tcp_t* lcltcp = NULL;

FILE* log = NULL;


uv_tcp_t* create_lcltcp() {
    uv_tcp_t* tcp = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), tcp);
    return tcp;
}

void free_lcltcp(uv_tcp_t* lcltcp) {
    uv_close((uv_handle_t*)lcltcp, on_close);
    lcltcp = NULL;
}

static void LogToFile(const gchar* message) {
    if (log != NULL) {
        GTimeVal* gtv = malloc(sizeof(GTimeVal));
        g_get_current_time(gtv);
        fprintf(log, "[%s]:%s", g_time_val_to_iso8601(gtv), message);
        free(gtv);
    }
}

static void log_handler(const gchar* log_domain, GLogLevelFlags log_level,
    const gchar* message, gpointer user_data)
{
    g_set_print_handler(LogToFile);
}


int main(int argc, char* argv[]) {

    void *argtable[] = {
        help = arg_litn(NULL,"help",0,1,"display the help and exit"),
        version = arg_litn(NULL,"version",0,1,"display the version info"),
        ip = arg_strn(NULL,"remote ip","string",0,1,"set proxy server ip"),
        encryname = art_strn(NULL,"encryption method name","string",0,1,"set encryption method"),
        port = arg_intn(NULL,"remote port","integer",0,1,"set proxy server port"),
    };

    int nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0 || nerrors > 0 || port->count == 0 || encryname->count == 0) {
        fprintf(stderr, "Usage: Server ");
        arg_print_syntax(stderr, argtable, "\n");

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 0;
    }

    pf = malloc(sizeof(profile));
    log = fopen("local.log","wr");

    const char* hostip = ip->sval[0];
    short hostport = *(port->ival);
    const encry = encryname->sval[0];

    strcpy(pf->encrypt_name, encry);
    strcpy(pf->remotehost, hostip);
    pf->remoteport = hostport;

    struct sockaddr_in seraddr;
    uv_ip4_addr(hostip, hostport, &seraddr);

    lcltcp = create_lcltcp();

    uv_tcp_bind(lcltcp, (const struct sockaddr*)&seraddr, 0);
    uv_listen((uv_stream_t*)lcltcp, 1280, on_connection);


    fclose(log);
    free(pf);
    free_lcltcp(lcltcp);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;

}

