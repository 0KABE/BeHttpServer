#include <arpa/inet.h>
#include <cstring>
#include <map>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

const char *LOCAL_HOST = "127.0.0.1";

/**********************************************************************
 * Print out an error message with perror()(for system
 * errors; base on value of errno, which indicates system call errors)
 * and exit the program indicating an error.
**********************************************************************/
void error_exit(const char *str) {
  perror(str);
  exit(1);
}

/**********************************************************************
 * Start up the http server
 * Start an process to listen http connections on a specified port.
 * If the port is 0, it will allocate a port and modify the original
 * port variable to reflect the actual prot.
**********************************************************************/
int start_up(u_short &port) {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)
    error_exit("socket");
  struct sockaddr_in serv_addr {
    AF_INET, htons(port), { inet_addr(LOCAL_HOST) }
  };

  if (bind(serv_sock, (sockaddr *)&serv_addr, sizeof(serv_addr)))
    error_exit("bind");

  if (!port) {
    socklen_t serv_addr_len = sizeof(serv_addr);
    if (getsockname(serv_sock, (sockaddr *)&serv_addr, &serv_addr_len))
      error_exit("getsockname");
    port = ntohs(serv_addr.sin_port);
  }

  if (listen(serv_sock, 20))
    error_exit("listen");

  return serv_sock;
}

/**********************************************************************
 * Read a line from a file descriptor fd, and store it in char *buff
 * with size buff_size
 * Return: the size read
**********************************************************************/
ssize_t read_line(const int fd, char *buff, const size_t buff_size) {
  ssize_t size = 0;
  while (size < buff_size - 1) {
    if (read(fd, &buff[size], 1) == -1)
      error_exit("read");
    if (buff[size++] == '\n')
      break;
  }
  buff[size++] = '\0';
  return size;
}

/**********************************************************************
 * 
**********************************************************************/
void unimplement_method(int client) {}

void *accept_request(void *pclient) {
  int client = *(int *)pclient;
  char buff[1024];
  ssize_t buff_len;
  buff_len = read_line(client, buff, sizeof(buff));

  char method[256];
  char url[256];
  char path[256];
  sscanf(buff, "%s%s", method, url);
  sprintf(path, "htdocs%s", url);
  if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
    unimplement_method(client);
    return NULL;
  }
  bool cgi = false;
  if (path[strlen(path) - 1] == '/')
    strcat(path, "index.html");
  struct stat st;
  if (stat(path, &st) == -1) {

  } else {

  }

  close(client);
  return NULL;
}

int main() {
  u_short port = 1234;
  int server_sock = start_up(port);
  printf("BeHttpServer running on port: %u\n", port);
  struct sockaddr_in client_sockaddr;
  socklen_t client_sock_len;
  pthread_t pthread;
  int client_sock;
  while (1) {
    if ((client_sock = accept(server_sock, (sockaddr *)&client_sockaddr,
                              &client_sock_len)) == -1)
      error_exit("accept");
    if (pthread_create(&pthread, NULL, accept_request, (void *)&client_sock) ==
        -1)
      error_exit("pthread_create");
  }
}