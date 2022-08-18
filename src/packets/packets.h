#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PACKET_DATA_SIZE 256

enum PacketCode {
  CODE_SEND_MSG
};

struct Packet {
  enum PacketCode code;
  time_t time;
  char user[PACKET_DATA_SIZE];
  char data[PACKET_DATA_SIZE];
};

// Turn a Packet struct into a buffer that can be sent through sockets
void serialize(struct Packet* packet, char* buffer);

// Turn a buffer into a usable Packet struct
void deserialize(char* buffer, struct Packet* packet);

#endif