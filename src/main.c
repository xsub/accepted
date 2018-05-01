/* 

(C) 2018 Pawel Suchanecki; Kernel Cartel 1978;

This software is dual licensed under BSD or GPL version 2;
As user you may select either version 2 of the GNU General Public License ("GPL") or BSD license ("BSD"). 

SRC URI: https://github.com/xsub/accepted

This program eases process of getting user console input (confirmations etc.).

It is designed to handle shell to user communication in form:

read -p "Do you want to... ? " answer
if accepted +y +Y +yes +YES -"no I dont want it" +IC"yes i want it" $answer ; then #do_some_stuff; fi

This will accept strings: "y", "Y", "yes", "YES", "yes i want it" and make program exit with code 0,
meaning the user input was accepted; Any other string including empty string (default behaviour) 
makes program exit with 1, in example above we explicitly add "no I dont want it" to negative answers.
The +IC prefix to +IC"yes i want it" command line argument "Ignores Case" for this string, meaning any
combination of lower and upper case letters is allowed.

*/

#include <config.h>
#include <libgen.h>,
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Linux man for queue(3): "Not in POSIX.1-2001. Present on the BSDs. The queue functions first appeared in 4.4BSD."*/
#include <sys/queue.h> 
 

const int ACCEPTED = EXIT_SUCCESS;
const int REJECTED = EXIT_FAILURE;
const char *delims = " ";
const int FILENAME_MAX_LEN = 32;

int program_type = 0x1337; // means unset

struct entry {
    char *str_ptr;
    LIST_ENTRY(entry) entries;
} *np;


void add_accepted (char *t) { 
}

void add_rejected (char *t) { 
}

void parse_option (char *t) {
}

int main (int argc, char **argv)
{

  // The operation mode (program_type) depends on binary name: check the actual file name
  char binary_name[FILENAME_MAX_LEN];
  strncpy(binary_name, basename(argv[0]), FILENAME_MAX_LEN);

  if (!strcmp(binary_name, "accepted")) { program_type = ACCEPTED; }
  else if (!strcmp(binary_name, "rejected")) { program_type = REJECTED; } 

  printf ("Program type (operation mode): %s\n", (ACCEPTED == program_type) ? "accepted" : "rejected");

  // no args provided, no answer can be accepting
  if (argc < 2) {
    exit (REJECTED);
  }

  // print version
  if (!strcmp (argv[1], "--version")) {
    puts ("This is " PACKAGE_STRING ".");
    exit (EXIT_SUCCESS);
  } 
    
  LIST_HEAD(a_head, entry) accept_list_head;
  LIST_HEAD(r_head, entry) reject_list_head;

  struct accept_list_head *accept_list_head_ptr;
  struct reject_list_head *reject_list_head_ptr;

  LIST_INIT(&accept_list_head);
  LIST_INIT(&reject_list_head);

  np = malloc(sizeof(struct entry));
  /* Insert at the head. */
  LIST_INSERT_HEAD(&accept_list_head, np, entries);

  /* walk through tokens */
  short arg_id = 1;
  while(arg_id < argc){
     char *token = argv[arg_id++];

     printf("Processing token: '%s'\n", token);

     if ('+' == token[0]) {
        add_accepted(token);
     } else if ('-' == token[0]){
        add_rejected(token);
     }
     else {
        parse_option(token);
     }
  }

cleanup:
  /* Delete lists*/
  while (accept_list_head.lh_first != NULL) LIST_REMOVE(accept_list_head.lh_first, entries);
  while (reject_list_head.lh_first != NULL) LIST_REMOVE(reject_list_head.lh_first, entries);

  exit (REJECTED);
}

/*
//Forward traversal 
for (np = accept_list_.lh_first; np != NULL; np = np->entries.le_next)
    np-> ...
  */
