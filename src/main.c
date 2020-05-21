#include "mgos.h"
#include "mgos_wifi.h"
#include "mgos_rpc.h"

static void sum_cb(struct mg_rpc_request_info *ri, void *cb_arg,
                   struct mg_rpc_frame_info *fi, struct mg_str args)
{
    LOG(LL_INFO, ("executing timer code"));
    double a = 0, b = 0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &a, &b) == 2)
    {
        mg_rpc_send_responsef(ri, "%.2lf", a + b);
    }
    else
    {
        mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"a\":N1,\"b\":N2}");
    }
    (void)cb_arg;
    (void)fi;
}

static void my_timer_cb(void *arg)
{
    LOG(LL_INFO, ("Inside timer function."));
    struct mg_rpc_call_opts opts = {.dst = mg_mk_str("ws://192.168.0.104:5000")};
    mg_rpc_callf(mgos_rpc_get_global(), mg_mk_str("ping"), NULL, NULL, &opts,
                 NULL);
    (void)arg;
}

enum mgos_app_init_result mgos_app_init(void)
{
    // Register JSON RPC Callback
    mg_rpc_add_handler(mgos_rpc_get_global(), "Sum", "{a: %lf, b: %lf}", sum_cb, NULL);

    // Register callback for a timer to send information to the server
    mgos_set_timer(5000, MGOS_TIMER_REPEAT, my_timer_cb, NULL);

    return MGOS_APP_INIT_SUCCESS;
}
