#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

/* Heavily cribbed (ie, mostly Copied) from 
   http://0pointer.net/blog/the-new-sd-bus-api-of-systemd.html 
   other ref needed
   - https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html
   - https://www.softprayog.in/programming/d-bus-tutorial
*/

int restart_service(char* service_name) {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *m = NULL;
        sd_bus *bus = NULL;
        const char *path;
        int r;

        /* Connect to the system bus */
        r = sd_bus_open_system(&bus);
        if (r < 0) {
                fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
                goto finish;
        }

        /* Issue the method call and store the respons message in m */
        r = sd_bus_call_method(bus,
                               "org.freedesktop.systemd1",           /* service to contact */
                               "/org/freedesktop/systemd1",          /* object path */
                               "org.freedesktop.systemd1.Manager",   /* interface name */
                               "RestartUnit",                          /* method name */
                               &error,                               /* object to return error in */
                               &m,                                   /* return message on success */
                               "ss",                                 /* input signature */
                               service_name,                       /* first argument */
                               "replace");                           /* second argument */
        if (r < 0) {
                fprintf(stderr, "Failed to issue method call: %s\n", error.message);
                goto finish;
        }

        /* Parse the response message */
        r = sd_bus_message_read(m, "o", &path);
        if (r < 0) {
                fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
                goto finish;
        }


finish:
        sd_bus_error_free(&error);
        sd_bus_message_unref(m);
        sd_bus_unref(bus);

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
