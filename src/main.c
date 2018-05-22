/*
 * (C) 2018 Pawel Suchanecki; Kernel Cartel 1978;
 *
 * This software is multi-licensed under BSD or GPL version 2 or MIT;
 * You may select either version 2 of the GNU General Public License ("GPL")
 * or BSD license ("BSD") or MIT license ("MIT") depending on your needs.
 * This code is targetting BSD, GNU/Linux and/or POSIX-compatible systems.
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
#define REPORT_ERROR(msg)                                                      \
  { printf("ERROR(%s:%d)#%d: %s\n", __func__, __LINE__, errors++, msg); }
#define FATAL_ERROR(msg)                                                       \
  {                                                                            \
    REPORT_ERROR(msg);                                                         \
    exit(1);                                                                   \
  }

/* Default accepting and rejecting anwsers (confirmations/declines) */
char *const stdyes[] = {"y", "yes", "yup", "yeah"};
char *const stdno[] = {"n", "no", "nah", "nope"};
char *const stdmin[] = {"y", "n"};

// All tokens of user input (answer/response to prompt) combined in processing.
char *user_input_tokens = NULL;

// Accepting/rejecting answers list types (ids)
enum { ACCEPT_LIST, REJECT_LIST };

// Accepting/rejecting answers list: single list entry
typedef struct token_list_entry token_list_entry_t;
struct token_list_entry {
  int arg_id;
  SLIST_ENTRY(token_list_entry) entries;
};

// Accepting/rejecting answers list single list declaration
SLIST_HEAD(a_slisthead, token_list_entry) accept_head;
SLIST_HEAD(r_slisthead, token_list_entry) reject_head;

void print_list(void *list_head_p, char list_type) {
  int i = 0;
  token_list_entry_t *entry_p = NULL;

  SLIST_FOREACH(entry_p, &accept_head, entries) {
    printf("arg_id: %d\n", entry_p->arg_id);
  }
  printf("\n");
}

/* Program cmd line arguments, the answer tokens (+<string> &<-string>) -
 * handlers */

/* add_accepted(): register tokens for positive answer list */
void add_element_list(int list_id, int token_id, char **argv) {
#if DEBUG_ON
  printf("id %s: token: (%d) '%s',\n", __func__, token_id, argv[token_id]);
#endif
  token_list_entry_t *new_el_ptr = malloc(sizeof(token_list_entry_t));
  new_el_ptr->arg_id = token_id;
  switch (list_id) {
  case ACCEPT_LIST:
    SLIST_INSERT_HEAD(&accept_head, new_el_ptr, entries);
    break;
  case REJECT_LIST:
    SLIST_INSERT_HEAD(&reject_head, new_el_ptr, entries);
    break;
  default:
    FATAL_ERROR("Unkown list id!");
  }
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

  // 4 state output: output 2 states but they alter between accepted &
  // rejected
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

void cleanup(void);

/* Helper */
int user_input_token_length = 0;
int user_input_token_start = 0;
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

  /* Support proper exit code when no args provided, no answer can be
   * accepting
   */
  if (argc < 2) {
    exit(program_inverse_type);
  }

  /* Init list data */
  SLIST_INIT(&accept_head);
  SLIST_INIT(&reject_head);

  /* Since memory related operations started, register cleanup */
  // atexit(cleanup);

  /*
   * Setup: Walk through argv (the program arguments) tokens, recognize
   * "answers", "long options" and "user input" and handle each type properly.
   */
  int user_input_tokens_count = 0;
  char in_user_input_token = 0;
  short arg_id = 1;

  while (arg_id < argc) {
    char *token = argv[arg_id];

#if DEBUG_ON
    printf("%s, processing token: '%s'\n", __func__, token);
#endif

#define token_non_zero_length(t) (strlen(t) > 1)
#define token_char_is_minus(t, pos) ('-' == token[pos])
#define token_char_is_plus(t, pos) ('+' == token[pos])

    if (!in_user_input_token) {

      if (token_char_is_plus(token, 0) && token_non_zero_length(token)) {

        add_element_list(ACCEPT_LIST, arg_id, argv);

      } else if (token_char_is_minus(token, 0) &&
                 !token_char_is_minus(token, 1)) {

        add_element_list(REJECT_LIST, arg_id, argv);

      } else if (token_char_is_minus(token, 0) &&
                 token_char_is_minus(token, 1)) {

        // parse the '--long-option' using temporary argv
        // Note: can pass "token prev" and skip this dummy
        char *const dummy_argv[2] = {binary_name, token};
        parse_option_getopt_long(dummy_argv, 0);

      } else {
        // Switch to processing user answer tokens (user input to prompt).
        // Once
        // this is set all processing of command line arguments is done below.
        in_user_input_token = 1;
        user_input_token_start = arg_id;
      }
    }

    if (in_user_input_token) {
// use input token(s) processing
#if DEBUG_ON
      printf("In user input processing mode, token: '%s'\n", token);
#endif
      user_input_tokens_count++;
      user_input_token_length += strlen(token) + 1;
    }

    // move to next argument
    arg_id++;

  } // while (arg_id < argc)

#if DEBUG_ON
  printf("user_input_tokens_count: %d\n", user_input_tokens_count);
  printf("user_input_token_length: %d\n", user_input_token_length);
#endif

  int i;
  if (user_input_token_length) {

    user_input_tokens = malloc(user_input_token_length);

    memset(user_input_tokens, '.', user_input_token_length);
    user_input_tokens[user_input_token_length - 1] = '\0';

    if (!user_input_tokens)
      FATAL_ERROR("malloc error");

    char *token = argv[user_input_token_start];

    memcpy(user_input_tokens, token, strlen(token));

    int offset = strlen(token) + 1;

    for (i = 1; i < argc - user_input_token_start; i++) {

      token = argv[user_input_token_start + i];

      printf("Current user_input_tokens: '%s'\n", user_input_tokens);

      memcpy(&(user_input_tokens[offset]), token, strlen(token));

      printf("Updated user_input_tokens: '%s'\n", user_input_tokens);

      offset += strlen(token) + 1;
    }
  }

#if DEBUG_ON
  printf("final user_input_tokens = %s\n", user_input_tokens);
  printf("byte by byte verification: ");
  i = 0;
  char *user_input_tokens_bb =
      user_input_tokens; // dont modify the user_input_tokens, free needs it
  while (user_input_token_length-- > 1) {
    printf("%d: %d, (%c), ", i++, *user_input_tokens_bb,
           *user_input_tokens_bb++);
  }
  printf("\n");
#endif

  // Clean up

  // Clean lists

  while (!SLIST_EMPTY(&accept_head)) {

#if DEBUG_ON
    puts("Cleaning up accept_list data and removing list...");
#endif

    token_list_entry_t *entry = SLIST_FIRST(&accept_head);
    printf("Token (id): (%d)\n", entry->arg_id);
    printf("Token body: (%d) = '%s'\n", entry->arg_id, argv[entry->arg_id]);
    SLIST_REMOVE_HEAD(&accept_head, entries);
    free(entry);
  }

  while (!SLIST_EMPTY(&reject_head)) {

#if DEBUG_ON
    puts("Cleaning up reject_list data and removing list...");
#endif

    token_list_entry_t *entry = SLIST_FIRST(&reject_head);
    printf("Token (id): (%d)\n", entry->arg_id);
    printf("Token body: (%d) = '%s'\n", entry->arg_id, argv[entry->arg_id]);
    SLIST_REMOVE_HEAD(&reject_head, entries);
    free(entry);
  }

#if DEBUG_ON
  puts("Cleaning up temporary data...");
#endif
  // cleanup the temporary data
  if (user_input_tokens) {
    // user_input_tokens[user_input_token_length]='\0';
    printf("user_input_tokens = %s\n", user_input_tokens);

    free(user_input_tokens);
  }

#if DEBUG_ON
  puts("Cleanup done.");
#endif

  // Terminate program: dual-logic exit code
  exit(program_inverse_type);
}
