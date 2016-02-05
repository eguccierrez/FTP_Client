
#include "unistd.h"  //standard symbolic constants and types
#include "stdio.h"  //prinf
#include "stdlib.h"  //exit
#include "getopt.h"  //automate some of the chore involved in parsing typical unix command line options
#include "sys/socket.h" //IP Family. makes available a type, socklen_t, which is an unsigned opaque integral type of length of at least 32 bits
#include "netinet/in.h" //IP Family. in_port_t:An unsigned integral type of exactly 16 bits.
                        //in_addr_t:An unsigned integral type of exactly 32 bits.
#include "string.h"
#include <regex.h> //regular-expression-matching types.
                   //defines the structures and symbolic constants used by the regcomp(), regexec(), regerror() and regfree() functions.
#include "pthread.h"  //threads


#include <errno.h>  //system error numbers
#include <netdb.h>  //definitions for network database operations
#include <arpa/inet.h> //definitions for internet operations
#define _MULTI_THREADED

struct globalArgs {
	char *filename;
	char *hostname;
	int portnr;
	char *username;
	char *password;
	int active;
	char *mode;
	char *logfile;
	int logging;
	int swarming;
	char *swarmfile;
} gA;

struct ftpArgs {
	int portnr;
	int tid;
	char *filename;
	char *hostname;
	char *username;
	char *password;
};

