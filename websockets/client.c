#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>

#define PORT 6969
#define MESSAGE_SIZE 128

static int callback_client(struct lws *wsi, 
                           enum lws_callback_reasons reason, 
                           void *user, 
                           void *in, 
                           size_t len) {
  //  printf("Entered callback function\n");
  switch (reason) {
  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    // Send a message once the connection is established
    printf("SENDING....\n");
    /*
      LWS_WRITE_TEXT, LWS_WRITE_BINARY, LWS_WRITE_CONTINUATION, LWS_WRITE_PING, LWS_WRITE_PONG
      the send buffer has to have LWS_PRE bytes valid BEFORE the buffer pointer you pass to lws_write().
    */
    unsigned char buf[LWS_PRE + MESSAGE_SIZE];
    memset((void*)buf, 0, LWS_PRE + MESSAGE_SIZE);
    strcpy(&buf[LWS_PRE], "Sup, Server...1");
    lws_write(wsi, &buf[LWS_PRE], strlen("Sup, Server...1"), LWS_WRITE_TEXT);
    break;
  case LWS_CALLBACK_CLIENT_RECEIVE:
    // Print the received message
    printf("Received: %s\n", (char *)in);
    break;
  default:
    break;
  }
  return 0;
}

static struct lws_protocols protocols[] = {
  {
    "client-protocol",
    callback_client,
    0,
    MESSAGE_SIZE,
  },
  { NULL, NULL, 0, 0 } // terminator
};

void sendMessage( unsigned char *message, size_t length )
{
  unsigned char buf[LWS_PRE + MESSAGE_SIZE];
  memset((void*)buf, 0, LWS_PRE + MESSAGE_SIZE);
  strncpy(&buf[LWS_PRE], message, length);
  lws_write(wsi, &buf[LWS_PRE], length, LWS_WRITE_TEXT);
}

int main() {
  struct lws_context_creation_info info;
  struct lws_context *context;

  memset(&info, 0, sizeof(info));
  info.port = CONTEXT_PORT_NO_LISTEN; // No listening port for client
  info.protocols = protocols;

  context = lws_create_context(&info);
  if (!context) {
    fprintf(stderr, "lws_create_context failed\n");
    return -1;
  }

  // Connect to the server
  struct lws_client_connect_info ccinfo = {0};
  ccinfo.context = context;
  ccinfo.address = "localhost";
  ccinfo.port = PORT;
  ccinfo.path = "/";
  ccinfo.host = lws_canonical_hostname(context);
  ccinfo.origin = "localhost";
  ccinfo.protocol = protocols[0].name;

  lws_client_connect_via_info(&ccinfo);

  // Event loop
  while (1) {
    lws_service(context, 0);
  }

  lws_context_destroy(context);
  return 0;
}
