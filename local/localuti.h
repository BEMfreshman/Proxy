#pragma once
#include <uv.h>

void on_connection(uv_stream_t* tcp_server, int status);
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);

void after_shutdown(uv_shutdown_t* req, int status);

void after_write(uv_write_t* req, int status);
void free_write_buf(uv_write_t* req);