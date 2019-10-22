/* Student Name: Sanchita Kanade
 * Class: CS570
 * Due Date: 11/28/2018
 * File Name:p2.c
 * Assignment: program4
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "getword.h"
#include "CHK.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#define MAXITEM 100
#define OUTPUT_MODE 0700

int tildeFlag;
int doubleLessthan;
int ip_fd;
int out_fd;

int backslashFlag;




