#pragma once
/******************************************************************************
 *
 * A majority of this code is taken from the `swaymsg` source code:
 * https://github.com/swaywm/sway/blob/master/common/ipc-client.c
 *
 *****************************************************************************/

#include <stdint.h>

enum ipc_command_type
{
  IPC_GET_INPUTS = 100,
};

/**
 * struct ipc_response - container for sway's ipc response
 *
 * @param uint32_t size - size of the ipc response
 * @param uint32_t type - type of the ipc response
 * @param char* payload - data (if any) of the ipc response
 */
struct ipc_response
{
  uint32_t size;
  uint32_t type;
  char *payload;
};

/**
 * ipc_open_socket - Open socket via given socket path, setting a
 *                   timeout to 3 seconds.
 *
 * Exits the program with return code 1 on error.
 */
int
ipc_open_socket(const char *socket_path);

/**
 * ipc_recv_response - Read in response from the opened ipc socket.
 *
 * Returns the created and filled ipc response struct.
 */
struct ipc_response *
ipc_recv_response(int socketfd);

/**
 * ipc_free_response - Utility function to free/clear the previously
 *                     allocated `struct ipc_response`
 */
void
ipc_free_response(struct ipc_response *response);

/**
 * ipc_dispatch_command - Send typed ipc command to given socket
 *
 * Returns the payload of the ipc response if any, exits the program
 * with return code 1 on error.
 */
char *
ipc_dispatch_command(int socketfd, uint32_t type, uint32_t *len);
