#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>

static int callback_echo(struct lws *wsi, 
			 enum lws_callback_reasons reason, 
			 void *user, 
			 void *in, 
			 size_t len) {
  printf("Entered Callback function\n");
  switch (reason) {
  case LWS_CALLBACK_RECEIVE:
    // Echo the received message back to the client
    printf("Rx'ed: %s\n", (char*)in);
    lws_write(wsi, in, len, LWS_WRITE_TEXT);
    break;
  default:
    printf("DEFAULT\n");
    break;
  }
  return 0;
}

static struct lws_protocols protocols[] = {
  {
    "client-protocol",
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
  info.vhost_name="127.0.0.1";
  info.port = 6969; // Port number
  info.protocols = protocols;
  info.options = LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT | LWS_SERVER_OPTION_DISABLE_IPV6;
  info.gid = -1;
  info.uid = -1;

  lws_set_log_level(LLL_DEBUG | LLL_INFO | LLL_NOTICE | LLL_WARN | LLL_ERR, NULL);
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
