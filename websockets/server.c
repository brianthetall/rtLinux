#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>

static int callback_echo(struct lws *wsi, 
                          enum lws_callback_reasons reason, 
                          void *user, 
                          void *in, 
                          size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE:
            // Echo the received message back to the client
            lws_write(wsi, in, len, LWS_WRITE_TEXT);
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_echo,
        0,
        128,
    },
    { NULL, NULL, 0, 0 } // terminator
};

int main() {
    struct lws_context_creation_info info;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = 8080; // Port number
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws_create_context failed\n");
        return -1;
    }

    // Event loop
    while (1) {
        lws_service(context, 0);
    }

    lws_context_destroy(context);
    return 0;
}
