#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "log.h"

// Function to remove comments e.g '/*' or '//' from a file
// Creates the uncommented temp file 'temporary' with empty lines
// Returns 0 if removing comments succeeded, otherwise -1
int remove_comments(char *filename, char *temporary) {

    FILE *read_file = fopen(filename, "r");

    if (!read_file) {
      return -1;
    }

    FILE *tmp_file = fopen(temporary, "w");
    if(!tmp_file) {
        return -1;
    }

    char ch_1 = fgetc(read_file);
    char ch_2;

    while(ch_1 != EOF) {
    	ch_2 = fgetc(read_file);
    	if (ch_1 == '/' && ch_2 == '/') { // single line comment
        while (ch_1 != '\n') {
          ch_1 = fgetc(read_file);
        }
        ch_1 = fgetc(read_file);
      } else if (ch_1 == '/' && ch_2 == '*') {  // multi line comment
        while (!(ch_1 == '*' && ch_2 == '/')) {
          ch_1 = ch_2;
          ch_2 = fgetc(read_file);
        }
        ch_1 = fgetc(read_file);
      } else {  // otherwise write character to file
          fputc(ch_1, tmp_file);
          ch_1 = ch_2;
        }
    }

    fclose(read_file);
    fclose(tmp_file);

    return 0;

}

// Function to remove empty lines from an uncommented file
// Creates the final clean file 'clean_file'
// Returns 0 if succeeded, otherwise -1
int remove_empty_lines(char *final, char *tmp) {

    FILE *clean_file = fopen(final, "w");

    if(!clean_file) {
        return -1;
    }

    FILE *tmp_file = fopen(tmp, "r");

    if(!tmp_file) {
        return -1;
    }

    char buffer[100];
    int i = 0;
    int found_char = 0;

    while (!feof(tmp_file)) {
      fgets(buffer, sizeof(buffer), tmp_file);
      while (buffer[i] != '\0') {
        if (isspace(buffer[i]) == 0) {  // Exit loop if we found even one character
          found_char = 1;
          break;
        }
        i++;
      }

      if (found_char == 1){ // If character write to clean file
        fputs(buffer, clean_file);
      }

      buffer[0] = '\0';
      found_char = 0;
      i = 0;

    }


    fclose(tmp_file);
    fclose(clean_file);

    return 0;

}
