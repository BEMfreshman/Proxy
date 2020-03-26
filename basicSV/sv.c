#include "sv.h"

ctx* create_ctx()
{
    ctx* re = malloc(sizeof(ctx));

    re->ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    re->ssl = SSL_new(re->ssl_ctx);

    re->bio_read = BIO_new(BIO_s_mem());
    re->bio_write = BIO_new(BIO_s_mem());

    SSL_set_bio(re->ssl, re->bio_read, re->bio_write);

    return re;
}

void free_ctx(ctx* node_ctx) {
    
    if (node_ctx == NULL) return;

    SSL_free(node_ctx->ssl);
    SSL_CTX_free(node_ctx->ssl_ctx);

    BIO_free(node_ctx->bio_read);
    BIO_free(node_ctx->bio_write);
    free(node_ctx);
}


node* create_node()
{
    node* nd = (node*)malloc(sizeof(node));
    nd->tcp_node = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), nd->tcp_node);

    nd->nd_ssl_ctx = create_ctx();

    return nd;
}

void free_node(node* nd) {
    if (nd == NULL) return;
    uv_close((uv_handle_t*)nd->tcp_node, on_close);

    free_ctx(nd->nd_ssl_ctx);

    free(nd);
}

int getnodeIndex(GList* nodelist, uv_tcp_t* tcp_node) {
    if (nodelist == NULL || tcp_node == NULL) return -1;

    node* nd = getnode(nodelist, tcp_node);
    if (nd == NULL) return -1;
    return g_list_index(nodelist, nd);
}

node* getnode(GList* nodelist, uv_tcp_t* tcp_node) {
    if (nodelist == NULL) return NULL;
    GList* nd = NULL;
    while ((nd = g_list_next(nodelist)) != NULL) {
        if (((node*)nd->data)->tcp_node == tcp_node) {
            return nd->data;
        }
    }

    return NULL;
}

void add_node(GList* nodelist, node* nd) {
    g_list_append(nodelist, nd);
}

node* get_node(GList* nodelist, size_t index) {
    return g_list_nth_data(nodelist, index);
}

void pop_node(GList* nodelist, int index) {
    node* nd = get_node(nodelist, index);
    if (nd == NULL) return;
    g_list_remove(nodelist, nd);
}

void pop_node2(GList* nodelist, node* nd) {

    if (nd == NULL) return;
    g_list_remove(nodelist, nd);
}

void on_close(uv_handle_t* handle){
    free(handle);
}


void free_nodelist(GList* nodelist){
    if (nodelist == NULL) return;
    clear_cli_list(nodelist);
}


void clear_cli_list(GList* nodelist) {
    g_list_foreach(nodelist, free_node, NULL);
}