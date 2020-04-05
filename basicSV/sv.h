#pragma once
#include <uv.h>
//#include "struct_sv.h"

#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/error.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

#define MAXLINE 256

typedef struct ctx_ {

    // https session component
    uv_tcp_t* tcp_remote;

    mbedtls_net_context remote_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;

    // encryption component
    mbedtls_aes_context aes_ctx;
    mbedtls_md_context_t sha_ctx;

} ctx;

typedef struct profile_ {
    short lclport;

    char remotehost[MAXLINE];
    short remoteport;

    char encrypt_name[MAXLINE];

} profile;

typedef enum s5stat_ {
    REGISTER,
    CONNECT,
    TRANSFER,
    INVALID
} s5stat;

typedef enum sslstat_ {

} sslstat;


typedef struct node_ {
    uv_tcp_t* tcp_local;
 
    uv_write_t* wrtreq;

    s5stat s5_status;
    sslstat ssl_status;

    int read_stat;

    profile* pf;

    ctx* nd_ssl_ctx;

}node;

void on_close(uv_handle_t* handle);

static GList* g_node = NULL;

node* create_node();
GList* add_node(GList* nodelist,node* cli);
node* get_node(GList* nodelist, size_t index);
GList* pop_node(GList* nodelist, int index);
GList* pop_node2(GList* nodelist, node* cli);
void free_node(node* cli);

int getnodeIndexbyll(GList* nodelist, uv_tcp_t* tcp_local);
int getnodeIndexbyrm(GList* nodelist, uv_tcp_t* tcp_remote);
node* getnodebyll(GList* nodelist, uv_tcp_t* tcp_local);
node* getnodebyrm(GList* nodelist, uv_tcp_t* tcp_remote);

void free_nodelist(GList** nodelist);
GList* close_node_by_readstat(GList* nodelist);

ctx* create_ctx();
void free_ctx(ctx* node_ctx);