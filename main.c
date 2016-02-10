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
}

int establishConnection(char *hostname, int portnr){
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//create socket file descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd < 0){
  		error("ERROR creating socket");
  		exit(0);
  	}
  	server = gethostbyname(hostname);
  	if (server == NULL)
  	{
  		fprintf(stderr, "ERROR, no such host");
  		exit(0);
  	}
  	serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	serv_addr.sin_port = htons(portnr);

	//connect sys call
	if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0)
  		error("ERROR connecting");
	printf("Connection Successful! \n");
	return sockfd;
}

int readF(int sckt, char recvBuff[]){
	int n;
	n = read(sckt, recvBuff, 2048);
	// printf("%s\n",recvBuff);
	if (n <0){
		fprintf(stderr, "Generic error");
	}
	recvBuff[n] = 0;
	if (gA.logfile!=NULL){
		fprintf(gA.logfile, "S->C: %s",recvBuff);
	}
	else if(gA.output == 1){
		fprintf(stdout, "S->C: %s",recvBuff);
	}
	return n;
}
void grabFile(int sckt, char *filename){
	char recvBuff[1024];
	char sendBuff[1024];
	// readF(sckt,recvBuff);
	sprintf(sendBuff,"GET %s\r\n", filename);
	writeF(sckt,sendBuff);
	readF(sckt,recvBuff);
}

void writeF(int sckt, char sendBuff[]){
	write(sckt, sendBuff, strlen(sendBuff));

	if (gA.logfile!=NULL){
		fprintf(gA.logfile, "C->S: %s",sendBuff);
	}
	else if(gA.output == 1){
		fprintf(stdout, "C->S: %s",sendBuff);
	}
}

void setActive(int socket, char sendBuff[]){

}

void setPasv(int socket){
	char recvBuff[1024];
	char sendBuff[1024];
	sprintf(sendBuff,"PASV");
	writeF(socket,sendBuff);
	readF(socket,recvBuff);
	

}

int authenticate(int sckt, char *password, char *username){
	int n= 0;
	//if(gA.logging==1)
	char recvBuff[1024];
	char sendBuff[1024];
	char *ptr;
	ptr = recvBuff;
	// bzero(recvBuff,1024);
	readF(sckt,recvBuff);
	// recv(sckt,recvBuff,1024,0);
	// printf("%s\n", );

	if(strncmp(recvBuff, "220",3)==0){
		sprintf(sendBuff,"USER %s\r\n", username);
		writeF(sckt,sendBuff);
	}

	readF(sckt,recvBuff);
	if(strncmp(recvBuff,"331",3)==0){
		sprintf(sendBuff,"PASS %s\r\n", password);
		writeF(sckt,sendBuff);
	}

	readF(sckt,recvBuff);

	while(strncmp(recvBuff,"230-",4)==0){
		readF(sckt,recvBuff);
		if (strstr(recvBuff,"230 ")){
			return 0;
		}
	}
}

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
	// int sockfd, newsockfd, clilen, n;
	// char buffer[256];
	// struct sockaddr_in serv_addr, cli_addr;

	gA.filename = NULL;
	gA.hostname = NULL;
	gA.portnr = 21;
	gA.username = "anonymous";
	gA.password = "user@localhost.localnet";
	gA.active = 0;
	gA.mode = "binary";
	gA.logfile = NULL;
	gA.logging = 0;
	gA.swarmfile = NULL;
	gA.swarming = 0;
	gA.output = 0;



	extern char *optarg;
	extern int optind,optopt;

	if (argc == 1){
		fprintf(stderr, "Error: No options given\n\n");
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
				gA.portnr = atoi(optarg);
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
				gA.active = 1;
				fprintf(stdout, "in a\n");
				break;
			case 'm':
				if (strcmp(optarg,"binary")==0){
					gA.mode = optarg;
				}
				else if (strcmp(optarg,"ASCII")==0){
					gA.mode = optarg;
				}
				else if (strcmp(optarg,"BINARY")==0){
					gA.mode = "binary";
				}
				else if (strcmp(optarg, "ascii")==0){
					gA.mode = "ASCII";
				}
				else {
					fprintf(stdout, "Error: Invalid mode\n");
					explain(stdout);
				}
				break;
			case 'l':
				if (strcmp(optarg,"-")==0){
					gA.output = 1;
				}
				else {
					gA.logfile = (FILE *)optarg;
				// gA.logging = 1;
				}
				fprintf(stdout, " in l\n");
				break;
		}
		// c = getopt_long(argc,argv,"hvf:s:p:n:P:am:l:",longOptions,&longIndex);
	}

	int sckt = establishConnection(gA.hostname, gA.portnr);
	// printf("%d\n",sckt);
	authenticate(sckt,gA.password,gA.username);



	if (gA.filename!=NULL){
		// fprintf(stdout, "about to get file\n");
		setPasv(sckt);
		// grabFile(sckt,gA.filename);
	}



	// grabFile(sckt,gA.filename);
	// return 0;
}