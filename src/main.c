/*
 * (C) 2018 Pawel Suchanecki; Kernel Cartel 1978;
 *
 * This software is dual licensed under BSD or GPL version 2; As user you
 * may select either version 2 of the GNU General Public License ("GPL")
 * or BSD license ("BSD"). This code is targetted to BSD & GNU/Linux.
 *
 * SRC URI: https://github.com/xsub/accepted
 *
 * This program eases process of getting user console input (confirmations
 * etc.).
 *
 * It is designed to handle shell to user communication in form:
 *
 * read -p "Do you want to... ? " answer if accepted +y +Y +yes +YES -"no
 * I dont want it" +IC"yes i want it" $answer ; then #do_some_stuff; fi
 *
 * This will accept strings: "y", "Y", "yes", "YES", "yes i want it" and
 * make program exit with code 0, meaning the user input was accepted;
 *
 * Any other string including empty string (default behaviour) makes
 * program exit with 1, in example above we explicitly add "no I dont want
 * it" to negative answers.
 *
 * The +IC prefix to +IC"yes i want it" command line argument "Ignores
 * Case" for this string, meaning any combination of lower and upper case
 * letters is allowed.
 *
 * Update: the program_mode related code introduces conditional program
 * execution: if the code is run as binary named `rejected` the above
 * logic gets inversed.
 */

#include <config.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* strings.h for srtcasecmp(const char *s1, const char *s2) defined in
 * XPG4.
 */
#include <strings.h>
#include <unistd.h> // for getopt_long_only() not defined in getopt.h

/* Linux man for queue(3): "Not in POSIX.1-2001. Present on the BSDs.
 * The queue functions first appeared in 4.4BSD."
 */
#include <sys/queue.h>

/* Program validation output states */
const int ACCEPTED = EXIT_SUCCESS;
const int REJECTED = EXIT_FAILURE;

/* The operation mode (program_type) depends on binary (file) name */
// making FILENAME_MAX_LEN a const int  here renders "variably modified" on
// binary_name[]
#define FILENAME_MAX_LEN (8) // rejected/accepted is 8 letters long
char binary_name[FILENAME_MAX_LEN];
int program_type = 0x13;         // means unset
int program_inverse_type = 0x37; // means unset

/* Trivial error reporting */
int errors = 0;
#define REPORT_ERROR(msg) printf("ERROR(%3d) %s\n", errors++, msg)

/* Default accepting and rejecting anwsers (confirmations/declines) */
char *const stdyes[] = {"y", "yes", "yup", "yeah"};
char *const stdno[] = {"n", "no", "nah", "nope"};
char *const stdmin[] = {"y", "n"};

// All tokens of user input (answer/response to prompt) combined in processing.
char *user_input_tokens = NULL;

/* Answers are stored in list having this entry as node; Once registered token
 * holds pointer to a answer. */
struct entry {
  char *token;
  LIST_ENTRY(entry) entries;
} * np;

/* Definition of both lists and pointers to their heads */

LIST_HEAD(accept_head, entry)
accept_list_head = LIST_HEAD_INITIALIZER(accept_list_head);

LIST_HEAD(reject_head, entry)
reject_list_head = LIST_HEAD_INITIALIZER(reject_list_head);

struct accept_head *accept_list_head_ptr;
struct rejct_head *reject_list_head_ptr;

/* alloc_element(): This allocates and return new struct entry node */
struct entry *alloc_element(char *token) {
  struct entry *new_el_ptr = malloc(sizeof(struct entry));

  if (new_el_ptr) {
    new_el_ptr->token = token;
#if DEBUG_ON
    printf("%s: allocation for token: %s, length: %d\n", __func__, token,
           (int)0); // strlen(token));
#endif
  } else {
    REPORT_ERROR("malloc error!");
  }
  return new_el_ptr;
}

/* Program cmd line arguments, the answer tokens (+<string> &<-string>) -
 * handlers */

/* add_accepted(): register tokens for positive answer list */
void add_accepted(char *token) {
#if DEBUG_ON
  printf("%s: token: %s,\n", __func__, token);
#endif
  struct entry *new_el_ptr = alloc_element(token);
  LIST_INSERT_HEAD(&accept_list_head, new_el_ptr, entries);
}

/* add_rejected(): register tokens for positive answer list */
void add_rejected(char *token) {}

/* Option processing helpers */
void version_exit() {
  puts("This is " PACKAGE_STRING ".");
  exit(EXIT_SUCCESS);
}

void help_exit(char err) {
  printf("Usage: %s [options] [+<yes_answer>] [-<no_answer>] "
         "<user_input_at_prompt>\n",
         binary_name);
  printf(
      "\n"
      "Options:\n\n"
      "--std-yes, --stdyes, --stdy    extend the user provided answers \n"
      "                               (if any) with standard confirmations:\n"
      "                               +y +yes +yup +yeah (case "
      "insensitive).\n");
  printf("\n\n");

  // 4 state output: output 2 states but they alter between accepted & rejected
  err ? exit(program_inverse_type) : exit(program_type);
}

/* Program argument options defintion -- we use long options only */
static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"version", no_argument, 0, 'v'},
                                       //   {"stdyes", no_argument, 0, 'y'},
                                       //   {"std-yes", no_argument, 0, 'Y' },
                                       //   {"stdno", no_argument, 0, 'n' },
                                       //   {"std-no", no_argument, 0, 'N' },
                                       {0, 0, 0, 0}};

/* Parse the GNU stye long options (--like-option) with getopt_long_only fed
with on-the-fly created
argv-like array to process only one entry at the time, but still benefit fro
getopt*(). */
void parse_option_getopt_long(char **argv, int item_to_parse_idx) {
#if DEBUG_ON
  printf("%s: token to process: '%s'\n", __func__, argv[1]);
#endif

  optind = 1;
  opterr = 0; // do not print unrecognized option error
  int opt = getopt_long_only(2, argv, "", long_options, &item_to_parse_idx);
  if (opt != -1) {
    switch (opt) {
    case 'v':
      version_exit(); // never returns
    case 'h':
      help_exit(0); // same
                    /*case 'l' : length = atoi(optarg);
                       break;
                       case 'b' : breadth = atoi(optarg);
                       break; */
    default:
      // Mimic the unrecognized option error. Pretty much useless ;-)
      printf("%s: unrecognized option '%s'\n", binary_name, argv[1]);
      help_exit(1);
    }
  }
#if DEBUG_ON
  else {
    printf("%s: getopt_long_only() failed with result = %d\n", __func__, opt);
  }
#endif
}

void cleanup(void) {

#if DEBUG_ON
  puts("Cleaning up accept_list data and removing list...");
#endif
  // Cleanup lists
  while (accept_list_head.lh_first != NULL) {
// intentionally missing check if allocation was done, we ALWAYS allocate
#if DEBUG_ON
    printf("%s: accept_list: removing token: %s\n", __func__,
           accept_list_head.lh_first->token);
#endif
    free(&(accept_list_head.lh_first->token));
    LIST_REMOVE(accept_list_head.lh_first, entries);
  }

#if DEBUG_ON
  puts("Cleaning up rejct_list data and removing list...");
#endif
  // clean and remove the reject_list
  while (reject_list_head.lh_first != NULL) {
    // here for the other list, the check is in place
    if (NULL == &(reject_list_head.lh_first->token)) {
#if DEBUG_ON
      printf("%s: reject_list: removing token: %s\n", __func__,
             reject_list_head.lh_first->token);
#endif
      free(&(reject_list_head.lh_first->token));
    }
    LIST_REMOVE(reject_list_head.lh_first, entries);
  }

#if DEBUG_ON
  puts("Cleaning up temporary data...");
#endif
  // cleanup the temporary data
  if (user_input_tokens)
    free(user_input_tokens);

#if DEBUG_ON
  puts("Cleanup done.");
#endif
}

int main(int argc, char **argv) {

  /* Get the binary image filename */
  strncpy(binary_name, basename(argv[0]), FILENAME_MAX_LEN);

  /* Detect program type by checking filename */
  if (!strcmp(binary_name, "accepted")) {
    program_type = ACCEPTED;
    program_inverse_type = REJECTED;
  } else if (!strcmp(binary_name, "rejected")) {
    program_type = REJECTED;
    program_inverse_type = ACCEPTED;
  }

#if DEBUG_ON
  printf("Program type (operation mode): %s\n",
         (ACCEPTED == program_type) ? "accepted" : "rejected");
#endif

  /* Support proper exit code when no args provided, no answer can be accepting
   */
  if (argc < 2) {
    exit(program_inverse_type);
  }

  /* Init list data */
  LIST_INIT(&accept_list_head);
  LIST_INIT(&reject_list_head);

  /* Since memory related operations started, register cleanup */
  atexit(cleanup);

  /*
   * Setup: Walk through argv (the program arguments) tokens, recognize
   * "answers", "long options" and "user input" and handle each type properly.
   */
  char in_user_input_token = 0;
  short arg_id = 1;
  while (arg_id < argc) {
    char *token = argv[arg_id];

#if DEBUG_ON
    printf("%s, processing token: '%s'\n", __func__, token);
#endif

    if (!in_user_input_token) {
      if ('+' == token[0]) {
        add_accepted(token);
      } else if ('-' == token[0] && '-' != token[1]) {
        add_rejected(token);
      } else if ('-' == token[0] && '-' == token[1]) {
        // parse the '--long-option' using temporary argv
        char *const dummy_argv[2] = {binary_name, token};
        parse_option_getopt_long(dummy_argv, 0);
      } else {
        in_user_input_token = 1; // now all processing is done below only
      }
    }

    if (in_user_input_token) {
      // use input token(s) processing
      if (NULL == user_input_tokens) {
#if DEBUG_ON
        printf("User input processing mode set, token: %s\n", token);
#endif
        user_input_tokens = malloc(strlen(token) + 1);
        memcpy(user_input_tokens, token, strlen(token));
      } else {
        int old_length = strlen(user_input_tokens);
        printf("old_length=%d\n", old_length);
        user_input_tokens = user_input_tokens =
            realloc(user_input_tokens, old_length + 1 + strlen(token));
        memcpy(user_input_tokens + old_length + 1, token, strlen(token) - 1);
#if DEBUG_ON
        printf("user_input_tokens = %s\n", user_input_tokens);
#endif
      }
    }
    arg_id++;
  }

#if DEBUG_ON
  printf("final user_input_tokens = %s\n", user_input_tokens);
#endif

  /* "answers" vs "user input" matching */

  // Forward traversal of the list to match each token vs user_input_tokens
  /*
  for (np = accept_list_head.lh_first; np != NULL; np =np->entries.le_next)
  {
    #if DEBUG_ON
    printf("%s: accept_list: matching  token: %s\n", __func__,
           accept_list_head.lh_first->token);
  }
 */

  // Terminate program: dual-logic exit code
  exit(program_inverse_type);
}
