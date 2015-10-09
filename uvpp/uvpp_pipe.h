/****************************************************************************

Creator: liyangyao@gmail.com
Date: 2015/8/18

****************************************************************************/

#ifndef UVPP_PIPE_H
#define UVPP_PIPE_H

#include "uvpp_define.h"
#include "uvpp_handle.h"
#include "uvpp_loop.h"
#include "uvpp_stream.h"

namespace uv{

class Pipe: public Stream<uv_pipe_t>
{
public:
    explicit Pipe(Loop *loop, bool ipc = true):
        Stream<uv_pipe_t>()
    {
        uv_pipe_init(loop->handle(), handle(), ipc);
    }

    explicit Pipe(uv_loop_t* loop = uv_default_loop(), bool ipc = true):
        Stream<uv_pipe_t>()
    {
        uv_pipe_init(loop, handle(), ipc);
    }


    ~Pipe()
    {

    }

    //Bind to the specified Pipe name.
    bool bind(const char *name)
    {
        return uv_pipe_bind(handle(), name)==0;
    }


    struct pipe_connect_ctx
    {
        uv_connect_t req;
        CallbackWithResult callback;
    };

    //Start connecion to the remote Pipe.
    bool connect(const char *name, const CallbackWithResult &connectCallback)
    {
        pipe_connect_ctx *ctx = new pipe_connect_ctx;
        ctx->callback = connectCallback;
        uv_pipe_connect(&ctx->req, handle(), name, pipe_connect_cb);
        return true;
    }


private:
    static void pipe_connect_cb(uv_connect_t* req, int status)
    {
        pipe_connect_ctx *ctx = UV_CONTAINER_OF(req, pipe_connect_ctx, req);
        if (ctx->callback)
        {
            ctx->callback(status);
        }
        delete ctx;
    }
    DISABLE_COPY(Pipe)
};
}

#endif // UVPP_PIPE_H
