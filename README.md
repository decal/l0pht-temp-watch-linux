temp-watch (ver 1.2)
====================

> Various compatibility updates to mudge's old school temp-watch tool that
> was released during his time with the infamous L0pht Heavy Industries
> (the original hacker-space.)  Forward-porting modifications for Linux
> applied by Derek Callaway __<decal@ethernet.org>__ *...*

* * *

## Tested on: ##
Kali Linux 2.0 (Ubuntu 14.04.3 LTS according to `/etc/issue`)
`uname -a` `=>` `Linux ca2 3.13.0-49-generic SMP x86_64 GNU/Linux`

## Linux Compile: ##
`make linux`
(creates dynamically linked binary for Linux)

## BSD Compile: ##
`make bsd`
(creates dynamically linked binary for BSD systems)

## Linux Static Compile: ##
`make linux-static`
(compile statically linked Linux binary)

## BSD Static Compile: ##
`make bsd-static`
(compile statically linked BSD binary)

## Clean: ##
`make clean`
(removes object files and core dumps, but leaves binary)

## Clobber: ##
`make clobber`
(remove the same things as clean, and the binary as well)
