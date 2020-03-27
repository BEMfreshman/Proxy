#include "localuti.h"
#include "sv.h"

extern GList* lcllist;

void on_connection(uv_stream_t* tcp_server, int status)
{

    // build connection;
    if (status == -1) {
        fprintf(stderr, "Failed to build new connection");
        return;
    }

    node* local = create_node();
    if (uv_accept(tcp_server, (uv_stream_t*)local->tcp_local) == 0) {
        // connect to new client has been built

        add_node(lcllist, local);
        
        uv_read_start((uv_stream_t*)local->tcp_local, alloc_buffer, on_read);
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

void on_read(uv_stream_t* tcp_local, ssize_t nread, const uv_buf_t* buf)
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
        uv_shutdown(req, tcp_local, after_shutdown);
    }

    node* nd = getnodebyll(lcllist, tcp_local);
    if (nd == NULL) {
        free(buf->base);
    }

    if (nread == UV_EOF) {
        // read all data
    }
    else {
        if (!SSL_is_init_finished(nd->nd_ssl_ctx->ssl)) {
            //SSL shakeshands has not been finished
            SSL_connect(nd->nd_ssl_ctx->ssl);
            free(buf->base);
        }
    }

}

void after_shutdown(uv_shutdown_t* req, int status)
{
    int index = getnodeIndex(lcllist, (uv_tcp_t*)req->handle);
    pop_node(lcllist, index);
}