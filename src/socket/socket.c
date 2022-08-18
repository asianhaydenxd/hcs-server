/* socket.c */

#include "socket.h"

void startServing(int serverSocket, struct sockaddr_in serverAddress) {
  int addressLength = sizeof(serverAddress);

  fd_set allSockets; // Contains sockets to server and all clients
  int clientDescriptors[MAX_CLIENTS] = {0}; // Field of possible socket descriptors

  char buffer[BUFFER_SIZE];

  while(1) {
    // Refresh allSockets container
    FD_ZERO(&allSockets);
    // Add serverSocket to allSockets
    FD_SET(serverSocket, &allSockets);

    // Check clientDescriptors slots
    int maxDescriptor = serverSocket;
    for(int i = 0; i < MAX_CLIENTS; i++) {
      // Add socket descriptor to allSockets if valid
      if(clientDescriptors[i] > 0) FD_SET(clientDescriptors[i], &allSockets);
      // Find length required to encompass all sockets
      if(clientDescriptors[i] > maxDescriptor) maxDescriptor = clientDescriptors[i];
    }

    // Wait until info is received from a socket in allSockets
    select(maxDescriptor + 1, &allSockets, NULL, NULL, NULL);

    // Info received from serverSocket, meaning a socket was opened to it
    if(FD_ISSET(serverSocket, &allSockets)) {
      int newDescriptor = accept(serverSocket, (struct sockaddr*)&serverAddress, (socklen_t*)&addressLength);

      printf("User %s:%d socket %d connected\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port), newDescriptor);

      send(newDescriptor, "Hello from the server!\n", 24, 0);

      // Find the first unoccupied slot in clientDescriptors
      for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clientDescriptors[i] == 0) {
          clientDescriptors[i] = newDescriptor;
          break;
        }
      }
    }

    // Check for info received from sockets
    for(int i = 0; i < MAX_CLIENTS; i++) {
      int currentDescriptor = clientDescriptors[i];

      // Check if info was received from this socket
      if(FD_ISSET(currentDescriptor, &allSockets)) {
        int packetValue = read(currentDescriptor, buffer, BUFFER_SIZE);

        if(packetValue <= 0) {
          // Disconnect socket
          getpeername(currentDescriptor, (struct sockaddr*)&serverAddress, (socklen_t*)&addressLength);
          printf("User %s:%d disconnected\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
          close(currentDescriptor);
          clientDescriptors[i] = 0;
        } else {
          // Interpret received packet
          printf("Received smthn\n");
          buffer[packetValue] = '\0';
          send(currentDescriptor, buffer, strlen(buffer), 0);
        }
      }
    }
  }
}

int initializeServer(int port) {
  // Open a socket and its descriptor
  int serverDescriptor = socket(AF_INET, SOCK_STREAM, 0);

  if(serverDescriptor == 0) {
    perror("socket failed");
    return -1;
  }

  // Set socket to allow multiple connections
  int opt = 1;
  int sockOptResult = setsockopt(serverDescriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
  if(sockOptResult < 0) {
    perror("setsockopt");
    return -1;
  }

  // Initialize address and port of server
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);

  // Bind server to address and port
  if(bind(serverDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
    perror("bind failed");
    return -1;
  }

  // Prepare to accept connections on serverDescriptor
  // Up to 3 connections will pend at a time
  if(listen(serverDescriptor, 3) < 0) {
    perror("listen");
    return -1;
  }

  // Start accepting connections and packets
  startServing(serverDescriptor, serverAddress);

  return 0;
}
