#pragma once
#include <uv.h>
//#include "struct_sv.h"

#include <uv.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

#define MAXLINE 256

typedef struct ctx_ {
    SSL* ssl;
    SSL_CTX* ssl_ctx;
    BIO* bio_read;
    BIO* bio_write;
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


typedef struct node_ {
    uv_tcp_t* tcp_local;
    uv_tcp_t* tcp_remote;

    uv_write_t* wrtreq;

    s5stat status;
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