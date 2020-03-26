#include "localuti.h"
#include "sv.h"
void on_connection(uv_stream_t* tcp_server, int status)
{
    extern GList* lcllist;
    // build connection;
    if (status == -1) {
        fprintf(stderr, "Failed to build new connection");
        return;
    }

    node* local = create_node();
    if (uv_accept(tcp_server, (uv_stream_t*)local->tcp_node) == 0) {
        // connect to new client has been built

        add_node(lcllist, local);
        
        uv_read_start((uv_stream_t*)local->tcp_node, alloc_buffer, on_read);
    }
    else {
        free_node(local);
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

    //
}

void after_shutdown(uv_shutdown_t* req, int status)
{
    extern GList* lcllist;
    int index = getnodeIndex(lcllist, (uv_tcp_t*)req->handle);
    pop_node(lcllist, index);
}