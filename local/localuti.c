#include "localuti.h"
#include "sv.h"
#include "socksh.h"

extern GList* lcllist;
extern profile* pf;

void close_connection_by_timer(uv_timer_t* handle)
{
    lcllist = close_node_by_readstat(lcllist);
}

void on_connection(uv_stream_t* tcp_lcl_ser, int status)
{

    // build connection;
    if (status == -1) {
        fprintf(stderr, "Failed to build new connection");
        return;
    }

    node* local = create_node();
    memcpy(local->pf, pf,sizeof(*pf));

    if (uv_accept(tcp_lcl_ser, (uv_stream_t*)local->tcp_local) == 0) {
        // connect to new client has been built

        lcllist = add_node(lcllist, local);
        
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
    node* nd = getnodebyll(lcllist, (uv_tcp_t*)tcp_local);
    if (nd == NULL || nd->read_stat == UV_EOF) {
        free(buf->base);
        return;
    }

    nd->read_stat = nread;

    if (nread == 0) {
        // reading busy, continue and don't do anything
        free(buf->base);
        return;
    }

    if (nread == UV_EOF) {
        g_message("recv eof, tcp connection will be closed");
        free(buf->base);
        return;
    }

    if (nread < 0) {
        //some error take place
        
        free(buf->base);

        uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
        uv_shutdown(req, tcp_local, after_shutdown);
        g_error("Error message: %s", uv_strerror(nread));
        return;
    }

    // sock5 shakehands
    if (nd->status != TRANSFER) {
        shakes_hands(nd, buf,nread);
        free(buf->base);
    }
    else {
        printf("Good\n");
        // do something
    }

//    uv_read_start((uv_stream_t*)nd->tcp_local, alloc_buffer, on_read);
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
    lcllist = pop_node(lcllist, index);
}