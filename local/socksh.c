#include "socksh.h"
#include "localuti.h"

void s5_shakes_hands(node* nd, const uv_buf_t* buf,size_t buf_len)
{

    parse_socks5_mess(nd, buf, buf_len);

    if (nd->s5_status == INVALID ) {
        free(nd->wrtreq->write_buffer.base);
        g_error("Failed to build s5 shakeshands");
    }

    uv_write(nd->wrtreq, (uv_stream_t*)nd->tcp_local, &nd->wrtreq->write_buffer, 1, after_write);
    
}

void parse_socks5_mess(node* nd, const uv_buf_t* buf, size_t buf_len)
{
    if (nd->s5_status == REGISTER) {
        if (buf->base[0] != 0x05) {
            // unknown protocol
            nd->s5_status = INVALID;
            return;
        }

        char methods[2] = "";
        methods[0] = buf->base[1];
        int size = (int)buf->base[1] + 1;

        nd->wrtreq->write_buffer = uv_buf_init((char*)malloc(buf->len), buf->len);

        nd->wrtreq->write_buffer.base[0] = 0x05;     // VERSION of SOCK
        nd->wrtreq->write_buffer.base[1] = 0xff;     // UNACCEPTED METHOD

        // if there is any valid method, overwrite it;
        for (int i = 1; i < size; i++) {
            nd->wrtreq->write_buffer.base[i] = buf->base[i + 1];
        }

        nd->wrtreq->write_buffer.len = size;

        nd->s5_status = CONNECT;
    }
    else if (nd->s5_status == CONNECT) {
        nd->wrtreq->write_buffer = uv_buf_init((char*)malloc(buf->len), buf->len);

        nd->wrtreq->write_buffer.base[0] = 0x05;     // VERSION of SOCK
        nd->wrtreq->write_buffer.base[1] = 0x00;     // SUCCESS CONNECT
        nd->wrtreq->write_buffer.base[2] = 0x00;
        nd->wrtreq->write_buffer.base[3] = buf->base[3];   //addr type

        for (int i = 4; i < 10; i++) {
            nd->wrtreq->write_buffer.base[i] = 0x00;
        }
        
        nd->wrtreq->write_buffer.len = 10;
        nd->s5_status = TRANSFER;
    }
}

int ssl_shakes_handes(node* nd)
{
    ctx* sslsession = nd->nd_ssl_ctx;
    profile* pf = nd->pf;

    int ret = 0;

    const char* rmhost = pf->remotehost;
    short rmport = pf->remoteport;

    char rmport_str[256] = "";
#ifdef WIN32
    _itoa(rmport, rmport_str, 10);
#else
    itoa(rmport, rmport_str, 10);
#endif

    // 1. certificates

    // 2. start connection
    if ((ret = mbedtls_net_connect(&sslsession->remote_fd, 
         rmhost, rmport_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
        g_message("ssl connect to proxy server failed");
        return EXIT_FAILURE;
    }

    uv_tcp_open(sslsession->tcp_remote, sslsession->remote_fd.fd);
    uv_tcp_init(uv_default_loop(), sslsession->tcp_remote);

    if ((ret = mbedtls_ssl_config_defaults(&sslsession->conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        g_message("ssl_config_defaults failed");
        return EXIT_FAILURE;
    }

    // 3. setup ssl
    if ((ret = mbedtls_ssl_setup(&sslsession->ssl, &sslsession->conf)) != 0)
    {
        g_message("ssl_setup failed");
        return EXIT_FAILURE;    
    }

    if ((ret = mbedtls_ssl_set_hostname(&sslsession->ssl, rmhost)) != 0)
    {
        g_message("ssl_set_hostname failed");
        return EXIT_FAILURE;
    }

    // 4. set bio

}