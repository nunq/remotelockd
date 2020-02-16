#include <stdio.h>
#include <systemd/sd-bus.h>

int main() {
// WIP
  if ( (port = netsetup()) == 1) {
    notify("waiting for network");
  } else {
    printf("listening on port %d", port);
  }

  return 0;
}


int panic() {
  //notify and die
}

int netsetup() {
  //bind to shuffled port, listenloop all the time
  //ret 0: port num (50000 < port < 60000)
  //ret 1: waiting for net
}

int notify(const char *message) {
  //libnotify send msg, w prog name, etc
}

int suspend() {
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus *bus = NULL;
  int retval;

  retval = sd_bus_open_system(&bus);
  if (retval < 0) {
    fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
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

  if (r < 0) {
    fprintf(stderr, "Failed to issue method call: %s\n", error.message);
    goto finish;
  }

finish:
  sd_bus_error_free(&error);
  sd_bus_unref(bus);

  return r < 0 ? 1 : 0;
}
