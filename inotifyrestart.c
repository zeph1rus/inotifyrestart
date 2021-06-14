/* inotifyreboot.c
short tool to reboot the system if the jvm changes 
contains (a lot) of code adapted from https://linuxhint.com/inotify_api_c_language/
and systemctl and stackoverflow and google
*/

#include<stdio.h>
#include<string.h>
#include<sys/inotify.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>
#include <sys/reboot.h>
#include<signal.h>
#include<time.h>
#include<fcntl.h>
#include "sdbus-restart.h"

#define RETRIES 3
#define SLEEP_FOR 5 // seconds to sleep before rebooting. Needs to be long enough for patching to run, say 1.5 hrs
#define MAX_EVENTS 1024  /* Maximum number of events to process*/
#define LEN_NAME 255 // max filename length in linux is 255 bytes, let's leave enough space, we aren't short on ram.
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))
/*buffer to store the data of events*/

int file_descriptor,watch_descriptor;

void signal_handler(int sig){

       // process has received a term signal, quit gracefully, close descriptors
       inotify_rm_watch( file_descriptor, watch_descriptor );
       close( file_descriptor );
       exit( 0 );

}



void sleep_then_restart_service(char* servicename) {
    for (int i = 0; i < RETRIES; i++) {
        time_t restart_time = time(NULL) + SLEEP_FOR;
        printf("Restarting %s at: %s, attempt %d ", servicename, ctime(&restart_time), i+1);
        if ( sleep(SLEEP_FOR) != 0 ) {
            //process was interrupted, break out.
            printf("Sleep caught signal before completion, cancelling scheduled restart of service\n");
            return;
        }
        int result = restart_service(servicename);
        if (result == 0) {
            printf("Service successfully sent restart command");
            break;
        }
    }
}

int main(int argc, char **argv){

       int opt;
       char *path_to_be_watched;
       char *file_to_watch;
       char *service_name;

       signal(SIGINT,signal_handler);
        /*
       path_to_be_watched = argv[1];
       file_to_watch = argv[2];
       service_name = argv[3];
    */
       /* Step 1. Initialize inotify */
       while ((opt = getopt(argc, argv, "p:f:s:h")) != -1) {
           switch(opt) {
               case 'p':
                    path_to_be_watched = optarg;
                    break;
                case 'f':
                    file_to_watch = optarg;
                    break;
                case 's':
                    service_name = optarg;
                    break;
                case '?':
                    printf("Unknown option: %c use -h for help\n", optopt);
                    break;
                case 'h':
                    printf("%s", help_string);
                    exit(0);
           }
       }
       if ( !path_to_be_watched || !file_to_watch || !service_name) {
           printf("All arguments must be provided\n%s", help_string);
           exit(1);
       }
       file_descriptor = inotify_init();


       // fnctl with the F_SETFL sets the O_NONBLOCK flag on the file descriptor, meaning we don't
       // interfere with reads
       if (fcntl(file_descriptor, F_SETFL, O_NONBLOCK) < 0)  {
           // couldn't set O_NONBLOCK so exit
           exit(2);
        }

       // We're only interested in IN_DELETE events.  Because we will usually want to monitor a symlink like /usr/lib/jvm/jre or /etc/alternatives/jre
       // we set IN_DONT_FOLLOW which treats symlinks as files rather than following them as a directory
       watch_descriptor = inotify_add_watch(file_descriptor,path_to_be_watched, IN_DELETE | IN_DONT_FOLLOW);

       if(watch_descriptor==-1){
               printf("Could not watch : %s\n",path_to_be_watched);
               // if we can't watch we should exit
               exit(-1);
       }
       else{
              printf("Watching : %s\n",path_to_be_watched);
       }


       while(1){

              int i=0,length;
              char buffer[BUF_LEN];

              /* Step 3. Read buffer*/
              length = read(file_descriptor,buffer,BUF_LEN);

              /* Step 4. Process the events which has occurred */
              while(i<length){

                struct inotify_event *event = (struct inotify_event *) &buffer[i];

                  if(event->len){
                   if ( event->mask & IN_DELETE ) {
                      if (strncmp(event->name, file_to_watch, LEN_NAME) == 0) {
                          printf( "Deletion Event for %s\\%s\n", path_to_be_watched ,event->name );
                          printf("File deletion matches watched file %s\n", file_to_watch);
                          sleep_then_restart_service(service_name);
                      }
                    }
                   i += EVENT_SIZE + event->len;
          }
    }
    }
}
