#include "sv.h"

ctx* create_ctx()
{
    ctx* re = malloc(sizeof(ctx));

    re->tcp_remote = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), re->tcp_remote);

    mbedtls_net_init(&re->remote_fd);
    mbedtls_ssl_init(&re->ssl);
    mbedtls_ssl_config_init(&re->conf);
    mbedtls_x509_crt_init(&re->cacert);
    mbedtls_ctr_drbg_init(&re->ctr_drbg);
    mbedtls_entropy_init(&re->entropy);

    mbedtls_aes_init(&re->aes_ctx);
    mbedtls_md_init(&re->sha_ctx);

    return re;
}

void free_ctx(ctx* node_ctx) {
    
    if (node_ctx == NULL) return;

    uv_close((uv_handle_t*)node_ctx->tcp_remote, on_close);

    mbedtls_net_free(&node_ctx->remote_fd);
    mbedtls_x509_crt_free(&node_ctx->cacert);
    mbedtls_ssl_free(&node_ctx->ssl);
    mbedtls_ssl_config_free(&node_ctx->conf);
    mbedtls_ctr_drbg_free(&node_ctx->ctr_drbg);
    mbedtls_entropy_free(&node_ctx->entropy);

    mbedtls_aes_free(&node_ctx->aes_ctx);
    mbedtls_md_free(&node_ctx->sha_ctx);

    free(node_ctx);
}


node* create_node()
{
    node* nd = (node*)malloc(sizeof(node));

    nd->tcp_local = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), nd->tcp_local);

    nd->wrtreq = (uv_write_t*)malloc(sizeof(uv_write_t));
    nd->read_stat = 0;

    profile *pf = (profile*)malloc(sizeof(profile));
    nd->pf = pf;
    
    nd->nd_ssl_ctx = create_ctx();

    nd->s5_status = REGISTER;

    return nd;
}

void free_node(node* nd) {
    if (nd == NULL) return;
    uv_close((uv_handle_t*)nd->tcp_local, on_close);

    free(nd->wrtreq);

    if (nd->nd_ssl_ctx == NULL) {
        free_ctx(nd->nd_ssl_ctx);
        nd->nd_ssl_ctx = NULL;
    }
    
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

    nd = g_list_first(nodelist);

    while ((nd = g_list_next(nd)) != NULL) {
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
        if (((node*)nd->data)->nd_ssl_ctx->tcp_remote == tcp_remote) {
            return nd->data;
        }
        else {
            return NULL;
        }
    }

    while ((nd = g_list_next(nodelist)) != NULL) {
        if (((node*)nd->data)->nd_ssl_ctx->tcp_remote == tcp_remote) {
            return nd->data;
        }
    }

    return NULL;
}

GList* add_node(GList* nodelist, node* nd) {
    return g_list_append(nodelist, nd);
}

node* get_node(GList* nodelist, size_t index) {
    return g_list_nth_data(nodelist, index);
}

GList* pop_node(GList* nodelist, int index) {
    node* nd = get_node(nodelist, index);
    if (nd == NULL) return nodelist;
    return g_list_remove(nodelist, nd);
}

GList* pop_node2(GList* nodelist, node* nd) {
    if (nd == NULL) return nodelist;
    return g_list_remove(nodelist, nd);
}

void on_close(uv_handle_t* handle){
    free(handle);
}


void free_nodelist(GList** nodelist){
    if (*nodelist == NULL) return;
    g_list_foreach(*nodelist, (GFunc)free_node, NULL);
}


GList* close_node_by_readstat(GList* nodelist)
{
    if (nodelist == NULL) return nodelist;
    for (GList* p = nodelist; p != NULL; p = p->next) {
        node* nd = (node*)p->data;
        if (nd->read_stat == UV_EOF) {
            GList* prev = p->prev;
            GList* next = p->next;
            if (prev != NULL) {
                p->prev->next = p->next;
            }
            if (next != NULL) {
                p->next->prev = p->prev;
            }
            free_node(nd);
            p = prev;
        }
    }
    return nodelist;
}
