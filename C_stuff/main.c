// including all the stuff i need
#include "cJSON.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 32768 // buffer got way bigger because the internet sends
                           // stuff in many tiny chunks instead of all at once
                           // like localhost does

// reads the whole request instead of trusting a single recv call, over the
// network one recv can hold half the headers or cut a message in half
static ssize_t read_request(int fd, char *buf, size_t size) {
  size_t total = 0;

  while (total + 1 < size) {
    ssize_t bytes_got = recv(fd, buf + total, size - 1 - total, 0);
    if (bytes_got <= 0) { // client hung up or errored, use whatever we got
      break;
    }
    total += bytes_got;

    char *headers_end = strstr(buf, "\r\n\r\n");
    if (headers_end != NULL) {
      size_t headers_size = (size_t)(headers_end - buf) + 4;
      long body_size = 0;
      char *length_header = strstr(buf, "Content-Length:");
      if (length_header != NULL && length_header < headers_end) {
        body_size = atol(length_header + 15); // 15 is strlen of the header name
      }
      if (total >= headers_size + (size_t)body_size) {
        break; // we already have the headers and the whole json body
      }
    }
  }

  buf[total] = '\0';
  return (ssize_t)total;
}

int main(void) {
  struct sockaddr_in addr;
  struct sockaddr_in client_addr;
  char buffer[BUFFER_SIZE];                  // makes buffer for stuff
  char buffer1[BUFFER_SIZE];                 // makes struct for stuff
  char buffer2[BUFFER_SIZE];                 // makes struct for stuff
  int abc = socket(AF_INET, SOCK_STREAM, 0); // makes socket

  int reuse = 1; // lets the server restart instantly instead of complaining
                  // that the port is still in use (nest restarts it a lot)
  setsockopt(abc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  const char *port_from_env = getenv("PORT"); // nest hands us a port to use
  int port_to_listen_on =
      (port_from_env != NULL) ? atoi(port_from_env)
                              : 6969; // falls back to the old port when unset

  const char *db_path = getenv("OCHAT_DB"); // lets us find the json files even
  if (db_path == NULL) {                    // when nest starts us from another
    db_path = "entire_chat.json";           // working folder
  }
  const char *swears_path = getenv("OCHAT_SWEARS"); // same idea for swears
  if (swears_path == NULL) {
    swears_path = "swears.json";
  }

  addr.sin_family = AF_INET; // saves data of socket in the struct
  addr.sin_port = htons((uint16_t)port_to_listen_on);
  addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 so the whole internet can knock
  memset(&(addr.sin_zero), '\0',
         sizeof(addr.sin_zero)); // zeroes out all the buffers and a part of the struct
  memset(buffer, '\0', sizeof(buffer));
  memset(buffer1, '\0', sizeof(buffer1));
  memset(buffer2, '\0', sizeof(buffer2));

  printf("oChat.c server listening on 0.0.0.0:%d (db: %s)\n", port_to_listen_on,
         db_path);

  char random_int = bind(abc, (struct sockaddr *)&addr,
                         sizeof(addr)); // binds the stuff used random_int
                                        // because program crashed otherwise
  listen(abc, 64); // listens for stuff connect to socket abc
  int i = 1;       // makes i for infinite while loop
  const char *text_to_be_sent = "idk"; // initialises stuff to be sent
  if (random_int < 0) {
    perror("bind");
    return 1;
  }
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

    struct timeval give_up_waiting = {
      1, 0}; // browsers leave sockets open without saying anything (they
             // preconnect and keep them warm), and this server only handles
             // one client at a time so one lazy socket used to freeze the
             // whole chat until the browser decided to close it
    setsockopt(abcd, SOL_SOCKET, SO_RCVTIMEO, &give_up_waiting,
               sizeof(give_up_waiting));

    char *client_ip =
        inet_ntoa(client_addr.sin_addr); // takes client's ip to store

    ssize_t are_we_online_rn; // initialises a ssize_t

    are_we_online_rn = read_request(abcd, buffer,
                                   sizeof(buffer)); // while connected online

    if (are_we_online_rn > 0) {

      // zeroes out buffer value
      buffer[are_we_online_rn] = '\0';

      char method[16], path[256];
      sscanf(buffer, "%s %s", method, path);

      char *query = strpbrk(path, "?#"); // drops ?query stuff off the path
      if (query != NULL) {
        *query = '\0';
      }

      if (strcmp(method, "OPTIONS") ==
          0) { // checks if method is equal to OPTIONS
        const char *options_response =
            "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: *\r\n" // this helps connect to
                                                 // frontend, it was generated
                                                 // using llm
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Access-Control-Max-Age: 86400\r\n" // browsers remember this for a
                                                   // day so they stop asking
                                                   // before every single message
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
                "*\r\nConnection: close\r\n\r\nInvalid Text Message/JSON Error "
                "lol idk :)";
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

              int is_clean = 1;

              FILE *swear = fopen(swears_path, "r");
              if (swear != NULL) {
                char line[1024];
                while (fgets(line, sizeof(line), swear)) {
                  cJSON *json = cJSON_Parse(line);
                  if (json) {
                    cJSON *item =
                        cJSON_GetObjectItemCaseSensitive(json, "word");
                    if (cJSON_IsString(item) && item->valuestring) {
                      // saves each swear word in the string
                      if (strstr(msg_field->valuestring, item->valuestring) !=
                              NULL ||
                          strstr(username->valuestring, item->valuestring) !=
                              NULL) {
                        is_clean = 0;
                        cJSON_Delete(json);
                        break;
                      }
                    }
                    cJSON_Delete(json);
                  }
                }
                fclose(swear);
              }

              if (is_clean == 0) {
                // sends message to website that user entered no no stuff
                const char *close = "HTTP/1.1 200 OK\r\n"
                                    "Access-Control-Allow-Origin: *\r\n"
                                    "Content-Type: application/json\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "{\"msg\": \"reload_website\"}";
                send(abcd, close, strlen(close), 0);
              } else {
                cJSON *entry = cJSON_CreateObject();

                cJSON_AddStringToObject(entry, "msg", msg_field->valuestring);
                cJSON_AddStringToObject(entry, "nickname",
                                        username->valuestring);
                cJSON_AddStringToObject(entry, "IP", client_ip);

                char *json_file_to_save = cJSON_PrintUnformatted(entry);
                FILE *db = fopen(db_path,
                                 "r+"); // opens the "database" to add data to

                if (db != NULL) {
                  fseek(db, -1, SEEK_END);
                  if (fgetc(db) == ']') { // checks if last value is "[" or not
                    long current_size =
                        ftell(db);           // stores old size of json file
                    int raw_fd = fileno(db); // gets file id
                    ftruncate(
                        raw_fd,
                        current_size -
                            1); // reduces file by 1 byte which deletes the "["
                  }
                  fseek(db, 0, SEEK_END);
                  fprintf(db, ",\n%s\n]",
                          json_file_to_save); // if db does exist
                  fclose(db);
                }
                free(json_file_to_save);
                cJSON_Delete(entry);
                const char *success =
                    "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: "
                    "*\r\nConnection: close\r\n\r\nmessage_recorded :)";
                send(abcd, success, strlen(success), 0);
              }
            }
            cJSON_Delete(text_coming);
          }
        }
      } else if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
        // a tiny hello so you can check the server is alive from a browser
        const char *hello =
            "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"msg\": \"oChat.c server is alive :)\"}";
        send(abcd, hello, strlen(hello), 0);
      } else if (strcmp(method, "GET") == 0 &&
                 strcmp(path,
                        "/db") == 0) { // checks if method = GET and path = /db
        FILE *db = fopen(db_path, "r");
        if (db == NULL) {
          const char *nothing =
              "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n"
              "Content-Type: application/json\r\n\r\n[]"; // sends this if db is
                                                          // nothing/empty
          send(abcd, nothing, strlen(nothing), 0);
        } else {
          const char *stupid_http_header = "HTTP/1.1 200 OK\r\n"
                                           "Access-Control-Allow-Origin: *\r\n"
                                           "Content-Type: application/json\r\n"
                                           "Connection: close\r\n"
                                           "\r\n";
          send(abcd, stupid_http_header, strlen(stupid_http_header),
               0); // sends the stupid headers to frontend
          fseek(db, 0, SEEK_END);
          long db_size = ftell(db); // gets db_size
          rewind(db);
          char *buffer_for_db =
              malloc(db_size); // makes a buffer same size as db
          fread(buffer_for_db, 1, db_size, db);  // loads entire db into ram
          send(abcd, buffer_for_db, db_size, 0); // sends the db to the frontend
          free(buffer_for_db); // so that server does not crash
          fclose(db);
        }
      }
      // zeroes buffer
      memset(buffer, '\0', sizeof(buffer));
      memset(buffer1, '\0', sizeof(buffer1));
      memset(buffer2, '\0', sizeof(buffer2));
    }
    close(abcd); // closes *everything*
  }
}