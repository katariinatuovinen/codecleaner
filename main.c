#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "log.h"

static int log_file;

void sig_handler(int signal) {
	int pid;
	switch (signal) {
		case SIGINT:
    {
      write_log(2, log_file, "Caught SIGINT signal in %d, program exiting", getpid());

			// Send SIGTERM to all the processses with the same group id 0
			kill(0, SIGTERM);

			// Brute force way to kill the remaining processes
			// WNOHANG: return immediately if no child has exited.
			while ((pid = waitpid(-1, 0, WNOHANG)) != -1) {
        write_log(2, log_file, "Killing process with PID = %d", pid);
		    kill(pid, SIGKILL);
      }
	    write_log(1, log_file, "All child processes killed.");
	   }
  }

	exit(0);

}




int main(int argc, char** argv) {

  // removing previous log file
  remove("code_cleaner.log");

	log_file = open("code_cleaner.log", O_RDWR | O_APPEND | O_CREAT | O_NONBLOCK, S_IRWXU);
  if (!log_file) {
    write_log(1, log_file, "Error while creating code_cleaner.log");
  }

  write_log(1, log_file, "Initial commit, logfile created");

  // Handle signal processes
  struct sigaction sig;

  sigemptyset(&sig.sa_mask);
  sig.sa_flags = 0;
  sig.sa_handler = sig_handler;

  // Handle only SIGINT signal
  sigaction(SIGINT, &sig, NULL);

  // Check that the program was called with right parameters (./cleaner testifile)
  if(argc < 2) {
    printf("Run the program with: %s testfile", argv[0]);
    write_log(1, log_file, "Code cleaner was called without a testfile. Program exited");
    return -1;
  }

  write_log(2, log_file, "Code cleaner ran with test file named ", argv[1]);

  // Process indentification
  pid_t pid;

  // Fork children for every child process, execute one program for every child
  for(int i = 1; i < argc; i++) {

      pid = fork();
      if(pid == -1) {
        write_log(1, log_file, "Failed forking a new child process \n");
        return -1;
      }

      // If a child process exists, call cleaner-functions
      if(pid == 0) {
				write_log(2, log_file, "Opening a new cleaner instance with filename: ", argv[i]);
        char* args[] = {"./cleaner", argv[i], NULL};
        execvp(args[0], args);  // execute ./cleaner with the wanted file
        return 0;
      }
  }


  // Wait for all the children to return before exiting the parent program
  // WNOHANG: return immediately if no child has exited.
  while (waitpid(-1, 0, WNOHANG) != -1) { }

	write_log(1, log_file, "All the cleaners returned. Exiting main program.");

	return 0;

}
