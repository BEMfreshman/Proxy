#include "socksh.h"
#include "localuti.h"

void shakeshands(node* nd, const uv_buf_t* buf)
{
    if (nd->status == REGISTER) {
        if (strlen(buf->base) != 2 || buf->base[0] != 0x05) {
            // unknown protocol
            nd->status = INVALID;
        }

        size_t size = 2;
        nd->wrtreq->write_buffer = uv_buf_init((char*)malloc(size), size);
        memcpy(nd->wrtreq->write_buffer.base, "\x05\x00", size);

        uv_write(nd->wrtreq, (uv_stream_t*)nd->tcp_local, &nd->wrtreq->write_buffer, size, after_write);

        nd->status = CONNECT;

    }
    else if (nd->status == CONNECT){
        nd->status = TRANSFER;
    }

    if (nd->status == INVALID) {
        g_error("Failed to build s5 shakeshands");
        
    }
}