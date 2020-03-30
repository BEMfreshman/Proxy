#include "socksh.h"
#include "localuti.h"

void shakes_hands(node* nd, const uv_buf_t* buf,size_t buf_len)
{

    parse_socks5_mess(nd, buf, buf_len);

    if (nd->status == INVALID ) {
        free(nd->wrtreq->write_buffer.base);
        g_error("Failed to build s5 shakeshands");
    }

    uv_write(nd->wrtreq, (uv_stream_t*)nd->tcp_local, &nd->wrtreq->write_buffer, 1, after_write);
    
}

void parse_socks5_mess(node* nd, const uv_buf_t* buf, size_t buf_len)
{
    if (nd->status == REGISTER) {
        if (buf->base[0] != 0x05) {
            // unknown protocol
            nd->status = INVALID;
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

        nd->status = CONNECT;
    }
    else if (nd->status == CONNECT) {
        nd->wrtreq->write_buffer = uv_buf_init((char*)malloc(buf->len), buf->len);

        nd->wrtreq->write_buffer.base[0] = 0x05;     // VERSION of SOCK
        nd->wrtreq->write_buffer.base[1] = 0x00;     // SUCCESS CONNECT
        nd->wrtreq->write_buffer.base[2] = 0x00;
        nd->wrtreq->write_buffer.base[3] = buf->base[3];   //addr type
        
        size_t iplen = strlen(nd->pf->remotehost);
        memcpy((void*)nd->wrtreq->write_buffer.base[4], (void*)nd->pf->remotehost, iplen);
        memcpy((void*)nd->wrtreq->write_buffer.base[4 + iplen], (void*)nd->pf->remoteport, 2);
        
        nd->wrtreq->write_buffer.len = 4 + iplen + 2;
        nd->status = TRANSFER;
    }
}