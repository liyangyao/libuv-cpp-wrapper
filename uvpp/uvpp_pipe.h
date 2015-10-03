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
    explicit Pipe(Loop *loop, bool ipc):
        Stream<uv_pipe_t>()
    {
        uv_pipe_init(loop->handle(), handle(), ipc);
    }

    ~Pipe()
    {

    }

    //Bind to the specified Pipe name.
    bool bind(const char *name)
    {
        return uv_pipe_bind(handle(), name)==0;
    }


    //Start connecion to the remote Pipe.
    bool connect(const char *name, const CallbackWithResult &onConnect)
    {
        m_onConnect = onConnect;
        uv_connect_t *req = new uv_connect_t;
        req->data = this;

        uv_pipe_connect(req, handle(), name, pipe_connect_cb);
        return true;
    }


private:
    DISABLE_COPY(Pipe)
    static void pipe_connect_cb(uv_connect_t* req, int status)
    {
        bool connected = status==0;
        Pipe *self = reinterpret_cast<Pipe *>(req->data);
        if (self->m_onConnect)
        {
            self->m_onConnect(connected);
        }
        delete req;
    }
    CallbackWithResult m_onConnect;
};
}

#endif // UVPP_PIPE_H
