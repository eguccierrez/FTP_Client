#include "mftp.h"

void explain(FILE *out){
	fprintf(out, "Run./mftp [OPTIONS] \n");
	fprintf(out, "OPTIONS: \n");

	fprintf(out, " -h, --help                   Displays this help information \n");
	fprintf(out, " -v, --version                Prints name of application, version number and author \n");
	fprintf(out, " -f, --file [file]            Specifies file to download \n");
	fprintf(out, " -s, --server [hostname]      Specifies the server to download the file from \n");
	fprintf(out, " -p, --port [port]            Specifies the port to be used when contacting the server (default:21) \n");
	fprintf(out, " -n, --username [user]        Uses the username user when logging into the FTP server (default:anonnymous) \n");
	fprintf(out, " -P, --password [password]    Uses the password password when loggin into the FTP server (default:user@localhost.loalnet) \n");
	fprintf(out, " -a, --active                 Forces active behaviour (the server opens the data connection to the client)(default:passive behaviour) \n");
	fprintf(out, " -m, --mode [mode]            Specifies the mode to be used for the transfer (ASCII or binary)(default:binary) \n");
	fprintf(out, " -l, --log [logfile]          Logs all the FTP commands exchanged with the server and the corresponding replies to file logfile.\n \t\t\t      If the filename is '-' then the commands are printed to stdout \n");
	exit(0);
};

static const struct option longOptions[] = {
	{"help",no_argument,NULL,'h'},
	{"version",no_argument,NULL,'v'},
	{"file",required_argument,NULL,'f'},
	{"server",required_argument,NULL,'s'},
	{"port",required_argument,NULL,'p'},
	{"username",required_argument,NULL,'n'},
	{"password",required_argument,NULL,'P'},
	{"active",no_argument,NULL,'a'},
	{"mode",required_argument,NULL,'m'},
	{"log",required_argument,NULL,'l'},
	{"swarm",required_argument,NULL,'w'},
	{NULL,no_argument,NULL,0}
};

int main(int argc, char **argv){
	int c;
	int longIndex;

	gA.filename = NULL;
	gA.hostname = NULL;
	gA.portnr = 21;
	gA.username = "anonnymous";
	gA.password = "user@localhost.localnet";
	gA.active = 0;
	gA.mode = "binary";
	gA.logfile = NULL;
	gA.logging = 0;
	gA.swarmfile = NULL;
	gA.swarming = 0;



	extern char *optarg;
	extern int optind,optopt;

	if (argc == 1){
		fprintf(stderr, "Error: No options given\n");
		explain(stdout);
	}

	while((c = getopt_long(argc,argv,"hvf:s:p:n:P:am:l:",longOptions,&longIndex)) != -1){
		switch(c) {
			case 'h':
				explain(stdout);
				break;
			case 'v':
				fprintf(stdout, "mftp 0.1\nAuthor:Enrique Gutierrez");
				break;
			case 'f':
				fprintf(stdout, " in f\n");
				gA.filename = optarg;
				break;
			case 's':
				gA.hostname = optarg;
				fprintf(stdout, " in s\n");
				break;
			case 'p':
				gA.portnr = optarg;
				fprintf(stdout, " in p\n");
				break;
			case 'n':
				gA.username = optarg;
				fprintf(stdout, " in n\n");
				break;
			case 'P':
				gA.password = optarg;
				fprintf(stdout, " in P\n");
				break;
			case 'a':
				// activeB();
				fprintf(stdout, "in a\n");
				break;
			case 'm':
				gA.mode = optarg;
				fprintf(stdout, " in m\n");
				break;
			case 'l':
				gA.logfile = optarg;
				fprintf(stdout, " in l\n");
				break;
		}
		c = getopt_long(argc,argv,"hvf:s:p:n:P:am:l:",longOptions,&longIndex);
	}
	// return 0;
}