#include "socksh.h"
#include "localuti.h"

void shakeshands(node* nd, const uv_buf_t* buf)
{
    if (nd->status == REGISTER) {
        if (buf->base[0] != 0x05) {
            // unknown protocol
            nd->status = INVALID;
        }

        char methods[2] = "";
        methods[0] = buf->base[1];
//        int size = (int)strtol(methods, NULL, 16) + 1;
        int size = (int)buf->base[1] + 1;

        nd->wrtreq->write_buffer = uv_buf_init((char*)malloc(buf->len), buf->len);
        
        nd->wrtreq->write_buffer.base[0] = 0x05;     // VERSION of SOCK
        nd->wrtreq->write_buffer.base[1] = 0xff;     // UNACCEPTED METHOD
        
        // if there is any valid method, overwrite it;
        for (int i = 1; i < size; i++) {
            nd->wrtreq->write_buffer.base[i] = buf->base[i + 1];
        }

        nd->wrtreq->write_buffer.len = size;

        uv_write(nd->wrtreq, (uv_stream_t*)nd->tcp_local, &nd->wrtreq->write_buffer, 1, after_write);

        nd->status = CONNECT;

    }
    else if (nd->status == CONNECT){
        nd->status = TRANSFER;
    }

    if (nd->status == INVALID) {
        g_error("Failed to build s5 shakeshands");
        
    }
}