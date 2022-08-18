/* packets.c */

#include <stdio.h>

#include "packets.h"

void serialize(struct Packet* packet, char* buffer) {
  int *q = (int*)buffer;
  *q = packet->code; q++;
  *q = packet->time; q++;

  char *p = (char*)q;
  for(int i = 0; i < PACKET_DATA_SIZE; i++, p++) {
    *p = packet->user[i];
  }

  for(int i = 0; i < PACKET_DATA_SIZE; i++, p++) {
    *p = packet->data[i];
  }
}

void deserialize(char* buffer, struct Packet* packet) {
  int* q = (int*)buffer;
  packet->code = *q; q++;
  packet->time = *q; q++;

  char *p = (char*)q;
  for(int i = 0; i < PACKET_DATA_SIZE; i++, p++) {
    packet->user[i] = *p;
  }

  for(int i = 0; i < PACKET_DATA_SIZE; i++, p++) {
    packet->data[i] = *p;
  }
}
