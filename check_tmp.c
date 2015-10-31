/* a quick and dirty hack to work across systems and watch particular
   directories. Ostensibly to start hammering on how bad people are on
   tmp droppings following links, etc. etc. .mudge 8.20.98 */

#include "check_tmp.h"

void usage(char *progname);
struct listStruct *addnode(struct listStruct *, char *, char *);
struct listStruct *prunelist(struct listStruct *);
int checknode(struct listStruct *, char *, char *);
int copyfile(char *src, char *dst);
int checkdir(char *);
#ifdef DEBUG
void walklist(struct listStruct *);
#endif
int become_daemon(void);

int main(int argc, char **argv){

  int c, copyflag=0, destflag=0, replaceflag=0, replacedone=0;
  int regcopyflag=0, replacewatchflag=0, regreplacewatchflag=0; 
#if defined (REGCOMP_3C)
  int ret;
#endif
  extern char *optarg;
  struct dirent *dp;
  DIR *dirp;
  char *watchdir = "/tmp", *copyname = NULL, tmp1[MAXNAMLEN];
  char *destdir = watchdir, *ptr;
  char *replacefile = NULL;
  char *replacewatch = NULL;
#if defined (REGEXPR_3G)
  char *expbuf;
#else 
#if defined (REGCOMP_3C)
  regex_t reg;
  char error_buffer[256];
#endif
#endif

#if (__FreeBSD_version >= 500011) || (__FreeBSD_version >= 410000 && __FreeBSD_version < 500000)
  struct kevent ev;
  int fd;
  struct timespec ts = { 0, 0 };
  int kq = -1;
#endif

  struct listStruct *list = NULL;

  syslogflag=0; /* initialize */

#ifdef REGEX
  while ((c = getopt(argc, argv, "Sd:c:C:D:hR:x:X:")) != EOF){
#else
  while ((c = getopt(argc, argv, "Sd:c:D:hR:x:")) != EOF){
#endif
    switch (c){
      case 'd':
        watchdir = optarg;
        break;
      case 'D':
        destdir = optarg;
        if (!checkdir(destdir)) {
          printf("invalid dest dir: %s\n", destdir);
          exit(1);
        }
        destflag=1;
        break;
      case 'c':
        copyname = optarg;
        ptr = (char *)strrchr(copyname, '/');
        if (ptr){
          copyname = (char *)(++ptr);
          printf("path not used for copyfile - only filename [%s]\n", copyname);
        }
        copyflag = 1;
        break;
#ifdef REGEX
      case 'C':
        copyname = optarg;
        ptr = (char *)strrchr(copyname, '/');
        if (ptr){
          copyname = (char *)(++ptr);
          printf("path not used for copyfile - only filename [%s]\n", copyname);
        }
#if defined (REGEXPR_3G)
        if( (expbuf = compile(copyname, NULL, NULL)) == NULL){
          printf("failed to compile regexp : %s\n", copyname);
          exit(1);
        }
#else 
#if defined (REGCOMP_3C)
        ret = regcomp(&reg, copyname, REG_EXTENDED);
        if (ret){
          regerror(ret, &reg, error_buffer, sizeof(error_buffer));
          printf("%s\n",error_buffer);
        }
#endif
#endif
        regcopyflag = 1;
        break;
#endif
      case 'S':
        syslogflag++;
        break;
      case 'R':
        replaceflag++;
        replacefile = optarg;
        break;
      case 'x':
        replacewatch = optarg;
        ptr = (char *)strrchr(replacewatch , '/');
        if (ptr){
          replacewatch = (char *)(++ptr);
          printf("path not used for copyfile - only filename [%s]\n", 
                  replacewatch);
        }
        replacewatchflag = 1;
        break;
#ifdef REGEX
      case 'X':
        replacewatch = optarg;
        ptr = (char *)strrchr(replacewatch, '/');
        if (ptr){
          replacewatch = (char *)(++ptr);
          printf("path not used for copyfile - only filename [%s]\n", 
                  replacewatch);
        }
#if defined (REGEXPR_3G)
        if( (expbuf = compile(replacewatch, NULL, NULL)) == NULL){
          printf("failed to compile regexp : %s\n", replacewatch);
          exit(1);
        }
#else
#if defined (REGCOMP_3C)
        ret = regcomp(&reg, replacewatch, REG_EXTENDED);
        if (ret){
          regerror(ret, &reg, error_buffer, sizeof(error_buffer));
          printf("%s\n",error_buffer);
        }
#endif
#endif
        regreplacewatchflag = 1;
        break;
#endif
      case 'h':
      case '?':
        usage(argv[0]);
        break;
    }
  }

  if (syslogflag){
    if (become_daemon()){
      fprintf(stderr, "failed to become daemon...exiting\n");
      exit(1);
    }
    openlog("l0pht-watch", LOG_PID, LOG_USER);
  }

  if (replacewatchflag && (copyflag || regcopyflag)){
    printf("either watch and replace or copy and replace. Not both.\n");
    exit(1);
  }

  if (replacewatchflag && regreplacewatchflag){
    printf("cannot specify both replacewatch flags\n");
    exit(1);
  }

  if (regcopyflag && copyflag){
    printf("cannot specify both copy flags\n");
    exit(1);
  }

  if (destflag && ! (copyflag || regcopyflag)){
    printf("must specify copy if specifying dest dir\n");
    exit(1);
  }

  dirp = opendir(watchdir);
  if (!dirp){
    printf("could not open %s\n", watchdir);
    exit(1);
  }

#if (__FreeBSD_version >= 500011) || (__FreeBSD_version >= 410000 && __FreeBSD_version < 500000)
  /* Set up the KQ on the target directory */
  fd = dirfd(dirp);

  kq = kqueue();
  if (kq < 0)
    err(1, "kqueue");

  ev.ident = fd;
  ev.filter = EVFILT_VNODE;
  ev.flags = EV_ADD | EV_ENABLE | EV_CLEAR;
  ev.fflags = NOTE_WRITE;
  kevent(kq, &ev, 1, NULL, 0, &ts);
#endif

  /* steup the first element of the list */
  while ((dp = readdir(dirp)) != NULL){
    if (!list) { /* first time */
      if (replacewatchflag){
        if (!replacedone){
          if (strncmp(dp->d_name, replacewatch, strlen(replacewatch)) == 0) {
            strncpy(tmp1, watchdir, MAXNAMLEN - 1);
            strcat(tmp1, "/");
            strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
            
            if(unlink(tmp1))
              perror("unlink");

            if(symlink(replacefile, tmp1))
              perror("symlink");

            replacedone = 1;
          }
        }
      }
    } else if (regreplacewatchflag){
        if (!replacedone){
#if defined (REGEXPR_3G)
          if(advance(dp->d_name, expbuf)){
#else
#if defined (REGCOMP_3C)
          if(regexec(&reg, dp->d_name, 0, NULL, 0) == 0){
#endif
#endif
            strncpy(tmp1, watchdir, MAXNAMLEN - 1);
            strcat(tmp1, "/");
            strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));

            if(unlink(tmp1))
              perror("unlink");

            if(symlink(replacefile, tmp1))
              perror("symlink");
          }
        }
      }

      if (copyflag){
        if ( strncmp(dp->d_name, copyname, strlen(copyname)) == 0) { 
          strncpy(tmp1, watchdir, MAXNAMLEN - 1);
          strcat(tmp1, "/");
          strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
          if ( copyfile(tmp1, destdir) == 0){
            if (syslogflag){
              syslog(LOG_ERR, "did not copy %s (problem?)\n", tmp1);
            } else {
              printf("did not copy %s (problem?)\n", tmp1);
            }
          }
          if (replaceflag){
            if (!replacedone){
              
              if(unlink(tmp1))
                perror("unlink");

              if(symlink(replacefile, tmp1))
                perror("symlink");

              replacedone = 1;
            }
          }
        }
      }
      else if (regcopyflag){
#if defined (REGEXPR_3G)
        if(advance(dp->d_name, expbuf)){
#else 
#if defined (REGCOMP_3C)
        if(regexec(&reg, dp->d_name, 0, NULL, 0) == 0){
#endif
#endif
          strncpy(tmp1, watchdir, MAXNAMLEN - 1);
          strcat(tmp1, "/");
          strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
          if ( copyfile(tmp1, destdir) == 0){
            if (syslogflag){
              syslog(LOG_ERR, "did not copy %s (problem?)\n", tmp1);
            } else {
              printf("did not copy %s (problem?)\n", tmp1);
            }
          }
          if (replaceflag){
            if (!replacedone){
              if(unlink(tmp1))
                perror("unlink");
              
              if(symlink(replacefile, tmp1))
                perror("symlink");

              replacedone = 1;
            }
          }
        }
      }
      list = addnode(list, watchdir, dp->d_name);  
      break;
  }

  rewinddir(dirp);

  while (1){

#if (__FreeBSD_version >= 500011) || (__FreeBSD_version >= 410000 && __FreeBSD_version < 500000)
    if (ev.fflags & NOTE_WRITE) {
#endif
    while ((dp = readdir(dirp)) != NULL){
      if (!(checknode(list, watchdir, dp->d_name))){
        if (replacewatchflag){
          if (!replacedone){
            if (strncmp(dp->d_name, replacewatch, strlen(replacewatch)) == 0) {
              strncpy(tmp1, watchdir, MAXNAMLEN - 1);
              strcat(tmp1, "/");
              strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
              
              if(unlink(tmp1))
                perror("unlink");

              if(symlink(replacefile, tmp1))
                perror("symlink");

              replacedone = 1;
            }
          }
        }
        else if (regreplacewatchflag){
          if (!replacedone){
#if defined (REGEXPR_3G)
            if(advance(dp->d_name, expbuf)){
#else
#if defined (REGCOMP_3C)
            if(regexec(&reg, dp->d_name, 0, NULL, 0) == 0){
#endif
#endif
              strncpy(tmp1, watchdir, MAXNAMLEN - 1);
              strcat(tmp1, "/");
              strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
              
              if(unlink(tmp1))
                perror("unlink");

              if(symlink(replacefile, tmp1))
                perror("symlink");

              replacedone = 1;
            }
          }
        }
        if (copyflag){
          if (!strncmp(dp->d_name, copyname, strlen(copyname))){ 
            strncpy(tmp1, watchdir, MAXNAMLEN - 1);
            strcat(tmp1, "/");
            strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
            if ( copyfile(tmp1, destdir) == 0){
              if (syslogflag){
                syslog(LOG_ERR, "did not copy %s (too many copies already?)\n",
                       tmp1);
              } else {
                printf("did not copy %s (too many copies already?)\n", tmp1);
              }
            }
            if (replaceflag){
              if (!replacedone){
                if(unlink(tmp1))
                  perror("unlink");

                if(symlink(replacefile, tmp1))
                  perror("symlink");

                replacedone = 1;
              }
            }
          }
        }
        else if (regcopyflag){
#if defined (REGEXPR_3G)
          if (advance(dp->d_name, expbuf)){
#else 
#if defined (REGCOMP_3C)
          if(regexec(&reg, dp->d_name, 0, NULL, 0) == 0){
#endif
#endif
            strncpy(tmp1, watchdir, MAXNAMLEN - 1);
            strcat(tmp1, "/");
            strncat(tmp1, dp->d_name, (MAXNAMLEN - 1) - strlen(tmp1));
            if ( copyfile(tmp1, destdir) == 0){
              if (syslogflag){
                syslog(LOG_ERR, "did not copy %s (too many copies already?)\n",
                       tmp1);
              } else {
                printf("did not copy %s (too many copies already?)\n", tmp1);
              }
            }
            if (replaceflag){
              if (!replacedone){
                
                if(unlink(tmp1))
                  perror("unlink");
                
                if(symlink(replacefile, tmp1))
                  perror("symlink");

                replacedone = 1;
              }
            }
          }
        }
        addnode(list, watchdir, dp->d_name);
      /* closedir(dirp); */
      }
    }
#if 0
    walklist(list);
#endif
    list = prunelist(list); 
    rewinddir(dirp);

#if (__FreeBSD_version >= 500011) || (__FreeBSD_version >= 410000 && __FreeBSD_version < 500000)
    /* Reset event so we don't trigger on the rewinddir */
    kevent(kq, &ev, 1, NULL, 0, &ts);

    /* Block until the directory changes */
    if (kevent(kq, NULL, 0, &ev, 1, NULL) < 0)
      err(1, "kevent");

  }
#endif
  }
/*   closedir(dirp); */

  return(1);
}

