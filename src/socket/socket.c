/* socket.c */

#include "socket.h"

int initializeServer(int port) {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  if(serverSocket == 0) {
    perror("socket failed");
    return -1;
  }

  int opt = 1;
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    return -1;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    return -1;
  }

  if (listen(serverSocket, 3) < 0) {
    perror("listen");
    return -1;
  }

  int addressLength = sizeof(address);
  if (accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addressLength) < 0) {
    perror("accept");
    return -1;
  }

  return 0;
}
