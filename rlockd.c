#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <libnotify/notify.h>
#include <systemd/sd-bus.h>

#define PORT ""
#define TOKENLEN_RNDMAX 999999999999
#define TOKENLEN_RNDMIN 99999999999

char recvdata[2048];
char token[16];
int sockfd;
struct addrinfo hints, *result, *output;

void notify(const char *message, const int millisecs) {
  notify_init("Hello world!");
  NotifyNotification *notif = notify_notification_new ("remotelockd", message, NULL);
  notify_notification_set_timeout(notif, millisecs);
  notify_notification_show (notif, NULL);
  g_object_unref(G_OBJECT(notif));
  notify_uninit();
}

void error(const char *message) {
  notify(message, 3000);
  exit(1);
}

int suspend() {
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus *bus = NULL;
  int retval;
  char errstr[384];

  retval = sd_bus_open_system(&bus);
  if (retval < 0) {
    snprintf(errstr, sizeof(errstr), "failed to connect to system bus: %s\n", strerror(-retval));
    notify(errstr, 3000);
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
    snprintf(errstr, sizeof(errstr), "failed to suspend: %s\n", error.message);
    notify(errstr, 3000);
    goto finish;
  }

finish:
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  return retval < 0 ? 1 : 0;
}

void gentoken() {
  srand(time(NULL)*getpid());
  unsigned long numtoken = rand() % (TOKENLEN_RNDMAX - TOKENLEN_RNDMIN) + 1;
  snprintf(token, sizeof(token), "%ld", numtoken);
}

void bootstrap() {
  int incoming_fd;
  int yes = 1;
  socklen_t addr_size;
  struct sockaddr_storage their_addr;

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
  }

  if (listen(sockfd, 10) == -1) {
    error("listen() failed");
  }

  gentoken();
  char textwtoken[128];
  snprintf(textwtoken, sizeof(textwtoken), "daemon is running. token is %s", token);
  notify(textwtoken, 10000);

  while(1) {
    addr_size = sizeof(their_addr);
    incoming_fd = accept(sockfd, (struct sockaddr *) &their_addr, &addr_size);
    if(incoming_fd == -1) {
      close(incoming_fd);
      continue;
    }

    if(recv(incoming_fd, recvdata, sizeof(recvdata), 0) == -1) {
      close(incoming_fd);
      continue;
    }

    if(strncmp(recvdata, token, sizeof(recvdata)) == 0) {
      close(incoming_fd);
      close(sockfd);
      if(suspend() == 0) {
        notify("suspending", 1500);
        exit(0);
      } else {
        error("couldn't suspend()");
        continue;
      }
    } else {
      close(incoming_fd);
      continue;
    }
  }
}

void printhelp() {
  printf("remotelockd cli options:\n -s start the program\n -h print help\n");
}

int main(int argc, char *argv[]) {
  int option;

  if(argc != 2) {
    printhelp();
    exit(0);
  }

  while ((option = getopt(argc, argv, "hs")) != -1) {
    switch(option) {
      case 'h':
        printhelp();
        break;
      case 's':
        bootstrap();
        break;
      default:
        printhelp();
        break;
    }
  }
  return 0;
}
