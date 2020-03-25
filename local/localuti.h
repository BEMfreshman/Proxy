#pragma once
#include <uv.h>

void on_connection(uv_stream_t* tcp_server, int status);
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);

void after_shutdown(uv_shutdown_t* req, int status);