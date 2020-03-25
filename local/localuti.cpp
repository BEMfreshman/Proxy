#include "localuti.h"
#include "sv.h"
void on_connection(uv_stream_t* tcp_server, int status)
{
    extern ser* localss;
    // build connection;
    if (status == -1) {
        fprintf(stderr, "Failed to build new connection");
        return;
    }

    client* cli = create_client();
    if (uv_accept(tcp_server, (uv_stream_t*)cli->tcp_client) == 0) {
        // connect to new client has been built
        localss->cli_list.push_back(cli);
        // Todo:read data
        uv_read_start((uv_stream_t*)cli->tcp_client, alloc_buffer, on_read);
    }
    else {
        free_client(cli);
    }
}

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    //prep buffer to use
    char* tmp = (char*)malloc(suggested_size);
    *buf = uv_buf_init(tmp, suggested_size);
}

void on_read(uv_stream_t* tcp_client, ssize_t nread, const uv_buf_t* buf)
{
    extern ser* server;
    if (nread == 0) {
        // reading busy, continue and don't do anything
        free(buf->base);
        return;
    }

    if (nread < 0) {
        //some error take place
        fprintf(stderr, "Error message: %s", uv_strerror(nread));
        free(buf->base);

        uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        uv_shutdown(req, tcp_client, after_shutdown);
    }
}

void after_shutdown(uv_shutdown_t* req, int status)
{
    uv_close((uv_handle_t*)req->handle, on_close);
}