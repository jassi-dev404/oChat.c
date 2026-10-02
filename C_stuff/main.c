// including all the stuff i need
#include "cJSON.h"
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
  struct sockaddr_in addr;
  char buffer[1000];                         // makes buffer for stuff
  char buffer1[1000];                        // makes struct for stuff
  char buffer2[1000];                        // makes struct for stuff
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

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  int array[] = {25, 50, 75, 100}; // makes array which has stuff
  int length_array = sizeof(array) / sizeof(array[0]); // finds lenght of array

  while (i == 1) { // infinite while loop

    int abcd = accept(abc, (struct sockaddr *)&client_addr,
                      &client_len); // accepts incoming connections
    if (abcd == -1) {
      perror("some oopsie happend"); // sends error using perror
      return 1;                      // crashes
    }

    char *client_ip =
        inet_ntoa(client_addr.sin_addr); // takes client's ip to store

    ssize_t are_we_online_rn; // initialises a ssize_t

    if ((are_we_online_rn = recv(abcd, buffer, sizeof(buffer) - 1,
                                 0)) > // while connected online
        0) {

      // zeroes out buffer value
      buffer[are_we_online_rn] = '\0';

      char method[16], path[256];
      sscanf(buffer, "%s %s", method, path);

      if (strcmp(method, "OPTIONS") ==
          0) { // checks if method is equal to OPTIONS
        const char *options_response =
            "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: *\r\n" // this helps connect to
                                                 // frontend, it was generated
                                                 // using llm
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";
        send(abcd, options_response, strlen(options_response),
             0); // sends that to front end so that sending json to front end
                 // works
      }

      else if (strcmp(method, "POST") == 0 &&
               strcmp(path, "/send") ==
                   0) { // checks if method = POST and path = /send
        char *body =
            strstr(buffer, "\r\n\r\n"); // in http 2 blank lines mark the split
                                        // between headers and json data
        if (body != NULL) {
          body += 4; // this is so that we skip over the "\r\n\r\n"
          cJSON *text_coming = cJSON_Parse(body);
          if (text_coming == NULL) {
            const char *error =
                "HTTP/1.1 400 Bad Request\r\nAccess-Control-Allow-Origin: "
                "*\r\n\r\nInvalid Text Message/JSON Error lol idk :)";
            send(abcd, error, strlen(error),
                 0); // sends error if json has some issue
          } else {
            cJSON *msg_field = cJSON_GetObjectItem(
                text_coming, "msg"); // takes the message and the nickname from
                                     // the json given by the front end
            cJSON *username = cJSON_GetObjectItem(text_coming, "nickname");

            if (cJSON_IsString(msg_field) && (msg_field->valuestring != NULL) &&
                cJSON_IsString(username) &&
                (username->valuestring !=
                 NULL)) { // checks if msg and nickname exist and are not null

              cJSON *entry = cJSON_CreateObject();

              cJSON_AddStringToObject(entry, "msg", msg_field->valuestring);
              cJSON_AddStringToObject(entry, "nickname", username->valuestring);
              cJSON_AddStringToObject(
                  entry, "IP",
                  client_ip); // appends msg, nickname, ip and their respective
                              // values to a new line in json file

              char *json_file_to_save = cJSON_PrintUnformatted(entry);
              FILE *db = fopen("entire_chat.json",
                               "r+"); // opens the "database" to add data to

              if (db != NULL) {
                fseek(db, -1, SEEK_END);
                if (fgetc(db) == ']') { //checks if last value is "[" or not
                  long current_size = ftell(db);  //stores old size of json file
                  int raw_fd = fileno(db);  //gets file id 
                  ftruncate(raw_fd, current_size - 1); //reduces file by 1 byte which deletes the "["
                }
                fseek(db, 0, SEEK_END);
                fprintf(db, ",\n%s\n]", json_file_to_save); // if db does exist
                fclose(db);
              }
              free(json_file_to_save);
              cJSON_Delete(entry);
              const char *success =
                  "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: "
                  "*\r\n\r\nmessage_recorded :)";
              send(abcd, success, strlen(success), 0);
            }
            cJSON_Delete(text_coming);
          }
        }
      }
      // zeroes buffer
      memset(buffer, '\0', 1000);
      memset(buffer1, '\0', 1000);
      memset(buffer2, '\0', 1000);
    }
    close(abcd); // closes *everything*
  }
}