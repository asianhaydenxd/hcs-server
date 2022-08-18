/* socket.c */

#include "socket.h"

void startServing(int serverSocket, struct sockaddr_in address) {
  int addressLength = sizeof(address);

  fd_set fds;
  int clientSockets[MAX_CLIENTS] = {0};

  char buffer[1024];

  while(1) {
    FD_ZERO(&fds);
    FD_SET(serverSocket, &fds);

    int maxSocket = serverSocket;

    for(int i = 0; i < MAX_CLIENTS; i++) {
      if(clientSockets[i] > 0) FD_SET(clientSockets[i], &fds);
      if(clientSockets[i] > maxSocket) maxSocket = clientSockets[i];
    }

    select(maxSocket + 1, &fds, NULL, NULL, NULL);

    if(FD_ISSET(serverSocket, &fds)) {
      int newSocket = accept(serverSocket, (struct sockaddr*)&address, (socklen_t*)&addressLength);

      printf("User %s:%d socket %d connected\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), newSocket);

      send(newSocket, "Hello from the server!\n", 24, 0);

      for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clientSockets[i] == 0) {
          clientSockets[i] = newSocket;
          break;
        }
      }
    }

    for(int i = 0; i < MAX_CLIENTS; i++) {
      int sd = clientSockets[i];

      if(FD_ISSET(sd, &fds)) {
        int valread = read(sd, buffer, 1024);

        if(valread <= 0) {
          getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addressLength);
          printf("User %s:%d disconnected\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
          close(sd);
          clientSockets[i] = 0;
        } else {
          printf("Received smthn\n");
          buffer[valread] = '\0';
          send(sd, buffer, strlen(buffer), 0);
        }
      }
    }
  }
}

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

  startServing(serverSocket, address);

  return 0;
}
