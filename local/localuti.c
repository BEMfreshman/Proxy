#include "localuti.h"
#include "sv.h"
#include "socksh.h"

extern GList* lcllist;

void on_connection(uv_stream_t* tcp_lcl_ser, int status)
{

    // build connection;
    if (status == -1) {
        fprintf(stderr, "Failed to build new connection");
        return;
    }

    node* local = create_node();
//    uv_stream_t* tcp_lcl_cli = malloc(sizeof(uv_stream_t));

    if (uv_accept(tcp_lcl_ser, (uv_stream_t*)local->tcp_local) == 0) {
        // connect to new client has been built

        add_node(&lcllist, local);
        
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
    memset(tmp, 0, suggested_size);
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
        g_error("Error message: %s", uv_strerror(nread));
        free(buf->base);

        uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        uv_shutdown(req, tcp_local, after_shutdown);
    }

    node* nd = getnodebyll(lcllist, (uv_tcp_t*)tcp_local);
    if (nd == NULL) {
        free(buf->base);
    }

    // sock5 shakehands
    if (nd->status != TRANSFER) {
        shakeshands(nd, buf);
    }
    else {
        // shakeshands over
        // do something
    }

    uv_read_start((uv_stream_t*)nd->tcp_local, alloc_buffer, on_read);
}

void after_write(uv_write_t* req, int status) {
    free_write_buf(req);
}

void free_write_buf(uv_write_t* req) {
    free(req->write_buffer.base);
    req->write_buffer.len = 0;
}

void after_shutdown(uv_shutdown_t* req, int status)
{
    int index = getnodeIndexbyll(lcllist, (uv_tcp_t*)req->handle);
    pop_node(&lcllist, index);
}