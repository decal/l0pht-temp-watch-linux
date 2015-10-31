#include "check_tmp.h"

/* taken from Stevens APUE */
int become_daemon(void){
 
  pid_t pid = 0x0;

  if ((pid = fork()) < 0)
    return -1;
  else if (pid != 0)
    exit(EXIT_SUCCESS); /* death to the parental unit */

  setsid();

  /* we're going to fork again for annoying SYSVr4 to insure that
     we have no controlling terminal */

  if((pid = fork()) < 0)

    return -1;
  else if (pid != 0) 
    exit(EXIT_SUCCESS); /* death to the parental unit */

  if(chdir("/"))
    perror("chdir");

  umask(0);

  return 0;
}
