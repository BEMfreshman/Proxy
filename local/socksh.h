#pragma once

#include <uv.h>
#include <sv.h>

void s5_shakes_hands(node* nd, const uv_buf_t* buf,size_t buf_len);
void parse_socks5_mess(node* nd, const uv_buf_t* buf, size_t buf_len);

int ssl_shakes_handes(node* nd);