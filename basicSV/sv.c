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

    nd->tcp_local = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), nd->tcp_local);

    nd->tcp_remote = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), nd->tcp_remote);

    nd->wrtreq = (uv_write_t*)malloc(sizeof(uv_write_t));

    profile *pf = (profile*)malloc(sizeof(profile));
    nd->pf = pf;
    
    nd->nd_ssl_ctx = create_ctx();

    nd->status = REGISTER;

    return nd;
}

void free_node(node* nd) {
    if (nd == NULL) return;
    uv_close((uv_handle_t*)nd->tcp_local, on_close);
    uv_close((uv_handle_t*)nd->tcp_remote, on_close);

    free(nd->wrtreq);

    free_ctx(nd->nd_ssl_ctx);
    free(nd->pf);

    free(nd);
}

int getnodeIndexbyll(GList* nodelist, uv_tcp_t* tcp_local) {
    if (nodelist == NULL || tcp_local == NULL) return -1;

    node* nd = getnodebyll(nodelist, tcp_local);
    if (nd == NULL) return -1;
    return g_list_index(nodelist, nd);
}

int getnodeIndexbyrm(GList* nodelist, uv_tcp_t* tcp_remote) {
    if (nodelist == NULL || tcp_remote == NULL) return -1;

    node* nd = getnodebyrm(nodelist, tcp_remote);
    if (nd == NULL) return -1;
    return g_list_index(nodelist, nd);
}

node* getnodebyll(GList* nodelist, uv_tcp_t* tcp_local) {
    if (nodelist == NULL) return NULL;
    GList* nd = NULL;

    if (nodelist->prev == NULL && nodelist->next == NULL) {
        nd = nodelist;
        if (((node*)nd->data)->tcp_local == tcp_local) {
            return nd->data;
        }
        else {
            return NULL;
        }
    }


    while ((nd = g_list_next(nodelist)) != NULL) {
        if (((node*)nd->data)->tcp_local == tcp_local) {
            return nd->data;
        }
    }

    return NULL;
}

node* getnodebyrm(GList* nodelist, uv_tcp_t* tcp_remote) {
    if (nodelist == NULL) return NULL;
    GList* nd = NULL;

    if (nodelist->prev == NULL && nodelist->next == NULL) {
        nd = nodelist;
        if (((node*)nd->data)->tcp_remote == tcp_remote) {
            return nd->data;
        }
        else {
            return NULL;
        }
    }

    while ((nd = g_list_next(nodelist)) != NULL) {
        if (((node*)nd->data)->tcp_remote == tcp_remote) {
            return nd->data;
        }
    }

    return NULL;
}

void add_node(GList** nodelist, node* nd) {
    *nodelist = g_list_append(*nodelist, nd);
}

node* get_node(GList* nodelist, size_t index) {
    return g_list_nth_data(nodelist, index);
}

void pop_node(GList** nodelist, int index) {
    node* nd = get_node(*nodelist, index);
    if (nd == NULL) return;
    *nodelist = g_list_remove(*nodelist, nd);
}

void pop_node2(GList** nodelist, node* nd) {

    if (nd == NULL) return;
    *nodelist = g_list_remove(*nodelist, nd);
}

void on_close(uv_handle_t* handle){
    free(handle);
}


void free_nodelist(GList* nodelist){
    if (nodelist == NULL) return;
    g_list_foreach(nodelist, free_node, NULL);
}
