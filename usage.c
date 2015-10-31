#include "check_tmp.h"

void usage(char *progname) {
  char *c = strrchr(progname, '/');
  char *proggie = c ? ++c : progname;

  printf("Usage: %s (ver 1.2) -dcCDRxXh\n", proggie);
  puts("Author: mudge@l0pht.com\tUpdate: decal@ethernet.org\n");
  puts("     -d directory     Directory to monitor (defaults to /tmp)");
  puts("     -c filename      File to make a copy of upon seeing in");
  puts("                       monitor.");
  puts("     -C filename      File to make a copy of upon seeing in");
  puts("                       monitor. filename in this case must be a");
  puts("                       regular expression with specific start and");
  puts("                       end markers [ ie '^' and '$' ]");
  puts("     -D  directory    Directory for storing copies (defaults to /tmp)");
  puts("     -S               Enable sending of syslog messages");
  puts("                         errors will be logged under LOG_ERR");
  puts("                         links showing up in the directory being watched");
  puts("                           will be logged under LOG_ALERT");
  puts("     -R filename      Replace with a symbolic link to filename");
  puts("     -x filename      filename to watch for and replace");
  puts("     -X filename      regex filename to watch for and replace");
  puts("                       note -- -R can be used with -C|c to copy");
  puts("                       and replace or with -x|X to simply replace");
  puts("     -h               help - this screen\n");

  exit(EXIT_FAILURE);
}
