#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <libnotify/notify.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <systemd/sd-bus.h>

#define PORT "50143"

struct addrinfo hints, *result, *output;
char recvdata[4096];
int sockfd;

void error(const char *message) {
  fprintf(stderr, "%s\n", message);
}

char *netsetup() {
  int yes = 1;
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  int incoming_fd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, PORT, &hints, &output) != 0) {
    error("getaddrinfo() failed");
  }

  for(result = output; result != NULL; result->ai_next) {
    if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
      error("socket() failed");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      error("setsockopt() failed");
      exit(1);
    }

    if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
      close(sockfd);
      error("bind() failed");
      continue;
    }
    break;
  }

  freeaddrinfo(output);

  if (result == NULL) {
    error("failed to bind");
    exit(1);
  }

  if (listen(sockfd, 10) == -1) {
    error("listen() failed");
    exit(1);
  }

  while(1) {
    addr_size = sizeof(their_addr);
    incoming_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);
    if(incoming_fd == -1) {
      error("accept() failed");
      continue;
    }

    if(recv(incoming_fd, recvdata, sizeof(recvdata), 0) == -1) {
      error("recv() failed");
    }
    printf("%s\n", recvdata);
    close(incoming_fd);
  }
  close(sockfd);
  return recvdata;
}

int notify(const char *message) {
  notify_init ("Hello world!");
  NotifyNotification * notif = notify_notification_new ("rlockd", message);
  notify_notification_show (notif, NULL);
  g_object_unref(G_OBJECT(notif));
  notify_uninit();
  return 0;
}

int suspend() {
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus *bus = NULL;
  int retval;

  retval = sd_bus_open_system(&bus);
  if (retval < 0) {
    fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-retval));
    goto finish;
  }

  retval = sd_bus_call_method(bus,
    "org.freedesktop.login1",           /* service to contact */
    "/org/freedesktop/login1",          /* object path */
    "org.freedesktop.login1.Manager",   /* interface name */
    "Suspend",                          /* method name */
    &error,                             /* object to return error in */
    NULL,                               /* method doesn't return anything */
    "b",                                /* input signature (bool) */
    0);                                 /* argument */

  if (retval < 0) {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    goto finish;
  }

finish:
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  return retval < 0 ? 1 : 0;
}


int main() {
  notify("listening on port 50143");
  netsetup();
  return 0;
}

