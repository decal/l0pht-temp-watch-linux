#include "check_tmp.h"

void usage(char *progname){
  char *c, *proggie;

  c = (char *)strrchr(progname, '/');
  if (c)
    proggie = (c + 1);
  else
    proggie = progname;

  printf("Usage: %s (ver 1.1) -dcCDRxXh\n", proggie);
  printf("Author: mudge@l0pht.com\n\n");
  printf("     -d directory     Directory to monitor (defaults to /tmp)\n");
  printf("     -c filename      File to make a copy of upon seeing in\n");
  printf("                       monitor.\n");
  printf("     -C filename      File to make a copy of upon seeing in\n");
  printf("                       monitor. filename in this case must be a\n");
  printf("                       regular expression with specific start and\n");
  printf("                       end markers [ ie '^' and '$' ]\n");
  printf("     -D  directory    Directory to store copies in (defaults to\n");
  printf("                       /tmp\n");
  printf("     -S               Enable sending of syslog messages\n");
  printf("                         errors will be logged under LOG_ERR\n");
  printf("                         links showing up in the directory being "
                                   "watched\n");
  printf("                           will be logged under LOG_ALERT\n");
  printf("     -R filename      Replace with a symbolic link to filename\n");
  printf("     -x filename      filename to watch for and replace\n");
  printf("     -X filename      regex filename to watch for and replace\n");
  printf("                       note -- -R can be used with -C|c to copy\n");
  printf("                       and replace or with -x|X to simply replace\n");
  printf("     -h               help - this screen\n\n");
  exit(1);
}

