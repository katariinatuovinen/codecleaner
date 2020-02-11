#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "log.h"
#include "cleanerlib.h"


static int log_file;

static void sig_handler(int signal) {

  switch (signal) {
		case SIGINT:
    {
      printf("User pressed ctrl + c, program exiting \n");
      write_log(1, log_file, "Caught SIGINT signal from user, program exiting \n");
      exit(0);
    }
  }
}

int main(int argc, char** argv) {


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
    printf("Run the program with ./cleaner testfile \n");
    write_log(1, log_file, "Code cleaner was called without a testfile. Program exited");
    return -1;
  }

  write_log(2, log_file, "Code cleaner ran with test file ", argv[1]);

  // Create the temp file
	char *tmp_file = calloc(strlen(argv[1]) + strlen(".temp"), sizeof(char));
  strcpy(tmp_file, argv[1]);
  strcat(tmp_file, ".temp");

  if (remove_comments(argv[1], tmp_file) == -1){ // argv[1] is the input file
    write_log(2, log_file, "Removing comments from file %s failed ", argv[1]);
    return -1;
  }

  // Create the final file
  char *clean_file = calloc(strlen(argv[1]) + strlen(".clean"), sizeof(char));
  strcpy(clean_file, argv[1]);
  strcat(clean_file, ".clean");

  if (remove_empty_lines(clean_file, tmp_file) == -1){
    write_log(2, log_file, "Removing empty lines from file %s failed", argv[1]);
    return -1;
  }

  // Remove temp file
  int ret = remove(tmp_file);
  if(ret == 0) {
    write_log(1, log_file, "Temporary file deleted successfully");
  } else {
    write_log(1, log_file, "Error: unable to delete the temporary file");
  }

  // Remove temp & final file arrays
  free(tmp_file);
  free(clean_file);

  return 0;

}
