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
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * strings.h for srtcasecmp(const char *s1, const char *s2) defined in
 * XPG4.
 */
#include <strings.h>
/*
 * Linux man for queue(3): "Not in POSIX.1-2001. Present on the BSDs. The
 * queue functions first appeared in 4.4BSD."
 */
#include <sys/queue.h>

const int ACCEPTED = EXIT_SUCCESS;
const int REJECTED = EXIT_FAILURE;
const char *delims = " ";
const int FILENAME_MAX_LEN = 32;

int program_type = 0x1337;	// means unset

int errors = 0;
#define REPORT_ERROR(msg) printf("ERROR(%3d) %s\n", errors++, msg)

struct entry
{
  char *token;
    LIST_ENTRY (entry) entries;
} *np;

LIST_HEAD (accept_head, entry) accept_list_head;
LIST_HEAD (reject_head, entry) reject_list_head;

     struct accept_head *accept_list_head_ptr;
     struct rejct_head *reject_list_head_ptr;


     struct entry *alloc_element (char *token)
{
  struct entry *new_el_ptr = malloc (sizeof (struct entry));

  if (new_el_ptr)
    {
      np->token = token;
    }
  else
    {
      REPORT_ERROR ("malloc error");
    }
  return new_el_ptr;
}

void
add_accepted (char *token)
{
  struct entry *new_el_ptr = alloc_element (token);
  LIST_INSERT_HEAD (accept_list_head_ptr, new_el_ptr, entries);
}

void
add_rejected (char *token)
{
}

void
parse_option (char *t)
{
  // print version
  if (!strcmp (t, "--version"))
    {
      puts ("This is " PACKAGE_STRING ".");
      exit (EXIT_SUCCESS);
    }

  if (!strcmp (t, "--help"))
    {
      puts ("Usage: WIP");
      exit (EXIT_SUCCESS);
    }
}

void
cleanup (void)
{
  while (accept_list_head.lh_first != NULL)
    LIST_REMOVE (accept_list_head.lh_first, entries);
  while (reject_list_head.lh_first != NULL)
    LIST_REMOVE (reject_list_head.lh_first, entries);
#if DEBUG_ON
  puts ("Cleanup done.");
#endif
}

int
main (int argc, char **argv)
{

  // The operation mode (program_type) depends on binary name: check the
  //
  // actual file name
  char binary_name[FILENAME_MAX_LEN];
  strncpy (binary_name, basename (argv[0]), FILENAME_MAX_LEN);

  if (!strcmp (binary_name, "accepted"))
    {
      program_type = ACCEPTED;
    }
  else if (!strcmp (binary_name, "rejected"))
    {
      program_type = REJECTED;
    }

#if DEBUG_ON
  printf ("Program type (operation mode): %s\n",
	  (ACCEPTED == program_type) ? "accepted" : "rejected");
#endif

  // no args provided, no answer can be accepting
  if (argc < 2)
    {
      if (ACCEPTED == program_type)
	{
	  exit (REJECTED);
	}
      else
	{
	  exit (ACCEPTED);
	}
    }


  /* Init list data */
  LIST_INIT (&accept_list_head);
  LIST_INIT (&reject_list_head);
  /* register cleanup */
  atexit (cleanup);


  /*
   * walk through tokens
   */
  short arg_id = 1;
  while (arg_id < argc)
    {
      char *token = argv[arg_id++];

#if DEBUG_ON
      printf ("Processing token: '%s'\n", token);
#endif

      if ('+' == token[0])
	{
	  add_accepted (argv[arg_id]);
	}
      else if ('-' == token[0] && '-' != token[1])
	{
	  add_rejected (argv[arg_id]);
	}
      else
	{
	  parse_option (token);
	}
    }

cleanup:
  /*
   * Delete lists
   */

  exit (REJECTED);
}

/*
 * //Forward traversal for (np = accept_list_.lh_first; np != NULL; np =
 * np->entries.le_next) np-> ...
 */
