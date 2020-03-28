
#include "sv.h"
#include <stdlib.h>
#include <stdio.h>
#include <argtable3.h>

#include "localuti.h"

struct arg_lit *help, *version;
struct arg_str* ip;
struct arg_str* encryname;
struct arg_int *port;
struct arg_int *lport;
struct arg_end *end;


FILE* logfile = NULL;
GList* lcllist = NULL;
profile* pf = NULL;
uv_tcp_t* lcltcp = NULL;


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
    if (logf != NULL) {
        GTimeVal gtv;
        g_get_current_time(&gtv);
        fprintf(logfile, "[%s]:%s", g_time_val_to_iso8601(&gtv), message);
    }
}

static void log_handler(const gchar* log_domain, GLogLevelFlags log_level,
    const gchar* message, gpointer user_data)
{
    g_set_print_handler(LogToFile);
}


int main(int argc, char* argv[]) {
    logfile = fopen("local.log", "w+");


    void *argtable[] = {
        help = arg_litn(NULL,"help",0,1,"display the help and exit"),
        version = arg_litn(NULL,"version",0,1,"display the version info"),
        ip = arg_strn(NULL,"rmip","string",0,1,"set proxy server ip"),
        encryname = arg_strn(NULL,"encry","string",0,1,"set encryption method"),
        port = arg_intn(NULL,"rmport","integer",0,1,"set proxy server port"),
        lport = arg_intn(NULL,"localport","integer",0,1,"set local port"),
        end = arg_end(20)
    };

    int nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0 || nerrors > 0 || port->count == 0 || lport->count == 0) {
        fprintf(stderr, "Usage: local ");
        arg_print_syntax(stderr, argtable, "\n");

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

        g_message("Arguments Parsering Failed");
        fclose(logfile);

        return 0;
    }

    pf = malloc(sizeof(profile));
    

    const char* hostip = ip->sval[0];
    short hostport = *(port->ival);
    short localport = (*lport->ival);
 //   const char *encry = encryname->sval[0];

 //   strcpy(pf->encrypt_name, encry);
    strcpy(pf->remotehost, hostip);
    pf->remoteport = hostport;
    pf->lclport = localport;

    struct sockaddr_in localaddr;
    uv_ip4_addr("0.0.0.0", localport, &localaddr);

    lcltcp = create_lcltcp();

    uv_tcp_bind(lcltcp, (const struct sockaddr*)&localaddr, 0);
    uv_listen((uv_stream_t*)lcltcp, 1280, on_connection);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);


    fclose(logfile);
    free(pf);
    free_lcltcp(lcltcp);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;

}

