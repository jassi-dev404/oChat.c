#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  struct sockaddr_in addr;
  char buffer[1000];
  char buffer1[1000];
  char buffer2[1000];
  int abc = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6969);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(&(addr.sin_zero), '\0', 8);
  memset(buffer, '\0', 1000);
  memset(buffer1, '\0', 1000);
  memset(buffer2, '\0', 1000);
  char random_int = bind(abc, (struct sockaddr *)&addr, sizeof(addr));
  listen(abc, 10);
  int i = 1;
  const char *text_to_be_sent = "idk";
  socklen_t addr_len = sizeof(struct sockaddr);
  int array[] = {25, 50, 75, 100};
  int length_array = sizeof(array) / sizeof(array[0]);
  while (i == 1) {
    int abcd = accept(abc, (struct sockaddr *)&addr, &addr_len);
    if (abcd == -1) {
      perror("some oopsie happend");
      return 1;
    }
    ssize_t are_we_online_rn;

    while ((are_we_online_rn = recv(abcd, buffer, sizeof(buffer) - 1, 0)) >
           0) { // value_array_1
      if (strlen(buffer) == 13 || strlen(buffer) == 14) {
        if (buffer[0] == 'v' && buffer[1] == 'a' && buffer[2] == 'l' &&
            buffer[3] == 'u' && buffer[4] == 'e' && buffer[5] == '_' &&
            buffer[6] == 'a' && buffer[7] == 'r' && buffer[8] == 'r' &&
            buffer[9] == 'a' && buffer[10] == 'y' && buffer[11] == '_') {
          int ii = buffer[12] - '0';
          if (ii < length_array) {
            sprintf(buffer2, "%d", array[ii]);
            text_to_be_sent = buffer2;
            send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
            printf("the data we sent from array[%i] is: %s\n", ii,
                   text_to_be_sent);
            fflush(stdout);
          } else {
            sprintf(buffer1, "array only goes till %i\n", length_array - 1);
            text_to_be_sent = buffer1;
            send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
            fflush(stdout);
          }
        } else {
          printf("the data you recieved is: %s\n", buffer);
          fflush(stdout);
          sprintf(buffer1,
                  "your data has been sent bitch, \n if you wanted to get a "
                  "value send 'value_array_x' where x below or equal to %i\n",
                  length_array - 1);
          text_to_be_sent = buffer1;
          send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
        }
      } else {
        printf("the data you recieved is: %s\n", buffer);
        fflush(stdout);
        sprintf(buffer1,
                "your data has been sent bitch, \n if you wanted to get a "
                "value send 'value_array_x' where x below or equal to %i\n",
                length_array - 1);
        text_to_be_sent = buffer1;
        send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
      }

      memset(buffer, '\0', 1000);
      memset(buffer1, '\0', 1000);
      memset(buffer2, '\0', 1000);
    }
    close(abcd);
  }
}