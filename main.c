// including all the stuff i need
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  struct sockaddr_in addr; // makes struct for stuff
  char buffer[1000];       // makes buffer for stuff
  char buffer1[1000];
  char buffer2[1000];
  int abc = socket(AF_INET, SOCK_STREAM, 0); // makes socket
  addr.sin_family = AF_INET; // saves data of socket in the struct
  addr.sin_port = htons(6969);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(&(addr.sin_zero), '\0',
         8); // zeroes out all the buffers and a part of the struct
  memset(buffer, '\0', 1000);
  memset(buffer1, '\0', 1000);
  memset(buffer2, '\0', 1000);
  char random_int = bind(abc, (struct sockaddr *)&addr,
                         sizeof(addr)); // binds the stuff used random_int
                                        // because program crashed otherwise
  listen(abc, 10); // listens for stuff connect to socket abc
  int i = 1;       // makes i for infinite while loop
  const char *text_to_be_sent = "idk"; // initialises stuff to be sent
  socklen_t addr_len = sizeof(struct sockaddr);
  int array[] = {25, 50, 75, 100}; // makes array which has stuff
  int length_array = sizeof(array) / sizeof(array[0]); // finds lenght of array
  while (i == 1) {                                     // infinite while loop
    int abcd = accept(abc, (struct sockaddr *)&addr,
                      &addr_len); // accepts incoming connections
    if (abcd == -1) {
      perror("some oopsie happend"); // sends error using perror
      return 1;                      // crashes
    }
    ssize_t are_we_online_rn; // initialises a ssize_t

    while ((are_we_online_rn = recv(abcd, buffer, sizeof(buffer) - 1,
                                    0)) > // while connected online
           0) {                           // value_array_1
      if (strlen(buffer) == 13 ||
          strlen(buffer) == 14) { // checks if value sent wants data from array
        if (buffer[0] == 'v' && buffer[1] == 'a' && buffer[2] == 'l' &&
            buffer[3] == 'u' && buffer[4] == 'e' && buffer[5] == '_' &&
            buffer[6] == 'a' && buffer[7] == 'r' && buffer[8] == 'r' &&
            buffer[9] == 'a' && buffer[10] == 'y' && buffer[11] == '_') {
          int ii = buffer[12] - '0'; // converts ascii value to normal value
          if (ii < length_array) {
            sprintf(buffer2, "%d",
                    array[ii]);        // gives buffer2 value of array[ii]
            text_to_be_sent = buffer2; // set text_to_be_sent to buffer2
            send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1,
                 0); // sends text_to_be_sent to client
            printf("the data we sent from array[%i] is: %s\n", ii,
                   text_to_be_sent); // prints out data sent to client
            fflush(stdout);
          } else {
            sprintf(buffer1, "array only goes till %i\n",
                    length_array - 1); // if client asks for array out of bounds
                                       // it gives client max array length
            text_to_be_sent = buffer1;
            send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
            fflush(stdout);
          }
        } else { // if format was wrong or client wanted to send smth else to
                 // server it does this
          printf("the data you recieved is: %s\n", buffer);
          fflush(stdout);
          sprintf(buffer1,
                  "your data has been sent bitch, \n if you wanted to get a "
                  "value send 'value_array_x' where x below or equal to %i\n",
                  length_array - 1);
          text_to_be_sent = buffer1;
          send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
        }
      } else { // same here
        printf("the data you recieved is: %s\n", buffer);
        fflush(stdout);
        sprintf(buffer1,
                "your data has been sent bitch, \n if you wanted to get a "
                "value send 'value_array_x' where x below or equal to %i\n",
                length_array - 1);
        text_to_be_sent = buffer1;
        send(abcd, text_to_be_sent, strlen(text_to_be_sent) + 1, 0);
      }
      // zeroes out buffer again
      memset(buffer, '\0', 1000);
      memset(buffer1, '\0', 1000);
      memset(buffer2, '\0', 1000);
    }
    close(abcd); // closes all
  }
}