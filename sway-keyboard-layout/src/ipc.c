/******************************************************************************
 *
 * A majority of this code is taken from the `swaymsg` source code:
 * https://github.com/swaywm/sway/blob/master/common/ipc-client.c
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc.h"

/* define sway (i.e. i3-compatible) ipc header data */
static const char ipc_magic[] = { 'i', '3', '-', 'i', 'p', 'c' };
#define IPC_HEADER_SIZE (sizeof(ipc_magic) + 8)

int
ipc_open_socket(const char *socket_path)
{
  struct sockaddr_un addr;

  /* create socket */
  int socketfd;
  if (-1 == (socketfd = socket(AF_UNIX, SOCK_STREAM, 0))) {
    fprintf(stderr, "%s\n", "Unable to open socket");
    exit(1);
  }

  /* setup socket */
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
  addr.sun_path[sizeof(addr.sun_path) - 1] = 0;
  int l = sizeof(struct sockaddr_un);
  if (-1 == connect(socketfd, (struct sockaddr *) &addr, l)) {
    fprintf(stderr, "%s %s\n", "Unable to connect to", socket_path);
    exit(1);
  }

  /* set ipc timeout */
  struct timeval timeout = { .tv_sec = 3, .tv_usec = 0 };
  setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  return socketfd;
}

struct ipc_response *
ipc_recv_response(int socketfd)
{
  char data[IPC_HEADER_SIZE];
  uint32_t *data32 = (uint32_t *) (data + sizeof(ipc_magic));

  size_t total = 0;
  while (total < IPC_HEADER_SIZE) {
    ssize_t received = recv(socketfd, data + total, IPC_HEADER_SIZE - total, 0);
    if (received <= 0) {
      fprintf(stderr, "%s\n", "Unable to receive IPC response");
      exit(1);
    }
    total += received;
  }

  struct ipc_response *response = malloc(sizeof(struct ipc_response));
  if (!response) {
    fprintf(stderr, "%s\n", "Unable to allocate memory for IPC response");
    exit(1);
  }

  total = 0;
  memcpy(&response->size, &data32[0], sizeof(data32[0]));
  memcpy(&response->type, &data32[1], sizeof(data32[1]));

  char *payload = malloc(response->size + 1);
  if (!payload) {
    free(response);
    fprintf(stderr, "%s\n", "Unable to allocate memory for IPC response");
    exit(1);
  }

  while (total < response->size) {
    ssize_t received =
      recv(socketfd, payload + total, response->size - total, 0);
    if (received < 0) {
      fprintf(stderr, "%s\n", "Unable to receive IPC response");
      exit(1);
    }
    total += received;
  }

  payload[response->size] = '\0';
  response->payload = payload;

  return response;
}

void
ipc_free_response(struct ipc_response *response)
{
  free(response->payload);
  free(response);
}

char *
ipc_dispatch_command(int socketfd, uint32_t type, uint32_t *len)
{
  char data[IPC_HEADER_SIZE];
  uint32_t *data32 = (uint32_t *) (data + sizeof(ipc_magic));

  memcpy(data, ipc_magic, sizeof(ipc_magic));
  memcpy(&data32[0], len, sizeof(*len));
  memcpy(&data32[1], &type, sizeof(type));

  if (-1 == write(socketfd, data, IPC_HEADER_SIZE)) {
    fprintf(stderr, "%s\n", "Unable to send IPC header");
    exit(1);
  }

  struct ipc_response *ipc_response = ipc_recv_response(socketfd);
  char *res = strdup(ipc_response->payload);
  *len = ipc_response->size;
  ipc_free_response(ipc_response);

  return res;
}
