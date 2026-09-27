#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  char buffer[1000];
  struct sockaddr_in addr;
  int client = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6969);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(&(addr.sin_zero), '\0', 8);
  if (connect(client, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("some oopsie happend:");
    exit(1);
  }
  printf("we are in boss\n");
  int i = 1;
  while (i == 1) {
    send(client, "secret_message", strlen("secret_message") + 1, 0);
    memset(buffer, '\0', 1000);
    recv(client, buffer, sizeof(buffer), 0);
    printf("yo G the server said: %s \n", buffer);
    send(client, "value_array_1", strlen("value_array_1") + 1, 0);
    memset(buffer, '\0', 1000);
    recv(client, buffer, sizeof(buffer), 0);
    printf("yo G the server said: %s \n", buffer);
    send(client, "value_array_9", strlen("value_array_9") + 1, 0);
    memset(buffer, '\0', 1000);
    recv(client, buffer, sizeof(buffer), 0);
    printf("yo G the server said: %s \n", buffer);
    send(client, "value_array_0", strlen("value_array_0") + 1, 0);
    memset(buffer, '\0', 1000);
    recv(client, buffer, sizeof(buffer), 0);
    printf("yo G the server said: %s \n", buffer);
    i = i + 1;
  }
}