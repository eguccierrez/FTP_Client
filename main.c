#include "mftp.h"
pthread_mutex_t readLock;
pthread_mutex_t writeLock;

void explain(FILE *out){
	fprintf(out, "Run./mftp [OPTIONS] \n");
	fprintf(out, "OPTIONS: \n");

	fprintf(out, " -h, --help                       Displays this help information \n");
	fprintf(out, " -v, --version                    Prints name of application, version number and author \n");
	fprintf(out, " -f, --file [file]                Specifies file to download \n");
	fprintf(out, " -s, --server [hostname]          Specifies the server to download the file from \n");
	fprintf(out, " -p, --port [port]                Specifies the port to be used when contacting the server (default:21) \n");
	fprintf(out, " -n, --username [user]            Uses the username user when logging into the FTP server (default:anonnymous) \n");
	fprintf(out, " -P, --password [password]        Uses the password password when loggin into the FTP server (default:user@localhost.loalnet) \n");
	fprintf(out, " -a, --active                     Forces active behaviour (the server opens the data connection to the client)(default:passive behaviour) \n");
	fprintf(out, " -m, --mode [mode]                Specifies the mode to be used for the transfer (ASCII or binary)(default:binary) \n");
	fprintf(out, " -l, --log [logfile]              Logs all the FTP commands exchanged with the server and the corresponding replies to file logfile.\n \t\t\t      If the filename is '-' then the commands are printed to stdout \n");
	fprintf(out, " -w, --swarm [swarm-config-file]  Swarming version of ftp download. Swarm config file must be of format [ftp://username:password@servername/file-path]");
	fprintf(out, " -b [num-bytes]                   Forces threads to download in segments of size num-bytes");
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
  	// fprintf(stdout, "%s\n", hostname);
  	server = gethostbyname(hostname);
  	if (server == NULL)
  	{
  		fprintf(stderr, "ERROR, no such host\n");
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
	// printf("Connection Successful! \n");
	return sockfd;
}

void *Part1(void *pass){
	int size;
	struct sArgs *yee = (struct sArgs *)pass;
	printf("eeeey\n");
	printf("%s\n",(char *)yee->hostname);
	int sckt = establishConnection(yee->hostname, yee->portnr);
		// printf("%d\n",sckt);
	authenticateS(sckt,yee->password,yee->username,yee->tid);

	if (yee->filename!=NULL){
		// fprintf(stdout, "about to get file\n");
		
		int portNum = setPasvS(sckt,yee->tid);
		int dataSocket = establishConnection(yee->hostname, portNum);
		char recvBuff[1024];
		char sendBuff[1024];
		setModeS(sckt,yee->tid);
		grabFileS(sckt,dataSocket,yee->filename,yee->tid);
	}
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

int readS(int sckt, char recvBuff[],int tid){
	int n;
	n = read(sckt, recvBuff, 2048);
	// printf("%s\n",recvBuff);
	if (n <0){
		fprintf(stderr, "Generic error");
	}
	recvBuff[n] = 0;
	pthread_mutex_lock(&readLock);
	if (gA.logfile!=NULL){
		fprintf(gA.logfile, "S->C: %s",recvBuff);
	}
	else if(gA.output == 1){
		fprintf(stdout, "%d\tS->C: %s",tid,recvBuff);
	}
	pthread_mutex_unlock(&readLock);
	return n;
}

int grabSize(int sckt, char *filename){
	// fprintf(stdout,"grabSize\n");
	char recvBuff[1024];
	char sendBuff[1024];
	char nah[100],yee[100];
	sprintf(sendBuff,"SIZE %s\r\n", filename);
	writeF(sckt,sendBuff);
	readF(sckt,recvBuff);
	sscanf(recvBuff,"%s %s",nah, yee);


	return atoi(yee);
}

void grabFile(int sckt, int dataSocket, char *filename){
	char recvBuff[1024];
	char sendBuff[1024];
	char recvData[2048];
	bzero(recvData,2048);
	// readF(sckt,recvBuff);
	sprintf(sendBuff,"RETR %s\r\n", filename);
	writeF(sckt,sendBuff);
	readF(sckt,recvBuff);
	// fprintf(stdout, "before reading data socket\n");
	readF(dataSocket,recvData);
	// while (readF(dataSocket,recvData) != NULL){
	// 	fprintf(stdout, "in while loop\n");
	// 	readF(dataSocket,recvData);
	// }
	gA.new_file = fopen(gA.filename, "w");
	fprintf(gA.new_file, "%s\n", recvData);
	// printf("%s\n",new_file );
	close(dataSocket);
	fclose(gA.new_file);
	// readF(dataSocket,recvData);
	// fprintf(stdout, "end of grabFile\n");
}

void grabFileS(int sckt, int dataSocket, char *filename, int tid){
	
	char recvBuff[1024];
	char sendBuff[1024];
	char recvData[2048];
	char *test;
	char *copy;
	bzero(recvData,2048);
	int count = 1;
	// readF(sckt,recvBuff);
	sprintf(sendBuff,"RETR %s\r\n", filename);
	writeS(sckt,sendBuff, tid);
	readS(sckt,recvBuff, tid);
	// fprintf(stdout, "before reading data socket\n");
	readS(dataSocket,recvData,tid);
	fprintf(stdout, "HERE\n");
	// test = strtok(filename,"/");


	while(test != NULL) {
		if (count == 1 ){
			test = strtok(filename,"/");
			count++;
		}
		else {
			copy = test;
			test = strtok(NULL, "/");
		}
	}

	printf("%s\n", copy);
	pthread_mutex_lock(&writeLock);
	// filename = fopen(filename, "w");
	// gA.new_file = fopen(gA.filename, "w");
	// fprintf(gA.new_file, "%s\n", recvData);
	// printf("%s\n",new_file );
	close(dataSocket);
	pthread_mutex_unlock(&writeLock);
	// fclose(gA.new_file);
	// readF(dataSocket,recvData);
	// fprintf(stdout, "end of grabFile\n");
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

void writeS(int sckt, char sendBuff[], int tid){
	pthread_mutex_lock(&writeLock);
	write(sckt, sendBuff, strlen(sendBuff));
	if (gA.logfile!=NULL){
		fprintf(gA.logfile, "C->S: %s",sendBuff);
	}
	else if(gA.output == 1){
		fprintf(stdout, "%d\tC->S: %s",tid,sendBuff);
	}
	pthread_mutex_unlock(&writeLock);
}

void setActive(int sckt){
// 	char recvBuff[1024];
// 	char sendBuff[1024];
// 	char hostName[1024];
// 	int sockfd, newsockfd, portno,port;
//     socklen_t clilen;
//     int connectionSock;
//     char buffer[256];
//     struct sockaddr_in serv_addr, cli_addr;
//     struct hostent *IP;
//     int cli_addrLen;
//     int n,i;

//     // sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     // if (sockfd < 0) 
//     //     error("ERROR opening socket");

//     // bzero((char *) &serv_addr, sizeof(serv_addr));
//     // // portno = 0;
//     // serv_addr.sin_family = AF_INET;
//     // serv_addr.sin_addr.s_addr = INADDR_ANY;
//     // serv_addr.sin_port = htons(0);

//     // if (bind(sockfd, (struct sockaddr *) &serv_addr,
//     //         sizeof(serv_addr)) < 0) 
//     //         error("ERROR on binding");
//     //--------------------------------------------//
    
//     getsockname(sckt, (struct sockaddr *) &cli_addr, &cli_addrLen);
//     port = ntohs(cli_addr.sin_port);

//     if(gethostname(hostName,sizeof(hostName))==0){
//     	IP = gethostbyname(hostName);
//     	sprintf(hostName, "%s", inet_ntoa(*(struct in_addr *)IP->h_addr));

//     	for (i=0;i<strlen(hostName); i++){
//     		if (hostName[i] == '.'){
//     			hostName[i] = ',';
//     		}
//     		sprintf(sendBuff,"PORT %s,%d,%d\r\n", hostName, port/256,port%256);
//     		fprintf(stdout, "%s\n", sendBuff);
//     	}}
//     else{
//     	fprintf(stdout, "ERROR: poop\n");
//     }
//     // sprintf(sendBuff,"PORT 127,0,0,1,10,0\r\n");
//     listen(sockfd,5);
// 	writeF(sckt,sendBuff);
// 	// readF(sckt,recvBuff);
//     listen(sockfd,5);
//     clilen = sizeof(cli_addr);
//     connectionSock = accept(sockfd, 
//                 (struct sockaddr *) &cli_addr, 
//                 &clilen);
//     if (newsockfd < 0) 
//         error("ERROR on accept");
//     readF(newsockfd,buffer);
// 	sprintf(sendBuff,"PORT 127,0,0,1,45,45\r\n");
// 	writeF(socket,sendBuff);
// 	readF(socket,recvBuff);
}

int setPasvS(int socket,int tid){
	char recvBuff[1024];
	char sendBuff[1024];
	sprintf(sendBuff,"PASV \r\n");
	writeS(socket,sendBuff,tid);
	readS(socket,recvBuff,tid);
	char portBuff[100];
	char port2[100];
	int i = 0;
	if(strncmp(recvBuff,"227",3)==0){
		char *pch;
		pch = strtok(recvBuff,"(");
		while(pch != NULL) {
			i ++;
			pch = strtok(NULL,",). ");
			// int yes = atoi(pch);
			if(i==5){
				sprintf(portBuff,"%s",pch);
			}
			if(i==6){
				sprintf(port2,"%s",pch);
			}
		}
	}
	// fprintf(stdout, "sup");
	int portNum = atoi(portBuff)*256 + atoi(port2);
	// fprintf(stdout, "%d\n", portNum);
	return portNum;

}

int setPasv(int socket){
	char recvBuff[1024];
	char sendBuff[1024];
	sprintf(sendBuff,"PASV \r\n");
	writeF(socket,sendBuff);
	readF(socket,recvBuff);
	char portBuff[100];
	char port2[100];
	int i = 0;
	if(strncmp(recvBuff,"227",3)==0){
		char *pch;
		pch = strtok(recvBuff,"(");
		while(pch != NULL) {
			i ++;
			pch = strtok(NULL,",). ");
			// int yes = atoi(pch);
			if(i==5){
				sprintf(portBuff,"%s",pch);
			}
			if(i==6){
				sprintf(port2,"%s",pch);
			}
		}
	}
	// fprintf(stdout, "sup");
	int portNum = atoi(portBuff)*256 + atoi(port2);
	// fprintf(stdout, "%d\n", portNum);
	return portNum;

}

int authenticate(int sckt, char *password, char *username, int swarrm){
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

int authenticateS(int sckt, char *password, char *username, int tid){
	int n= 0;
	//if(gA.logging==1)
	char recvBuff[1024];
	char sendBuff[1024];
	char *ptr;
	ptr = recvBuff;
	// bzero(recvBuff,1024);
	readS(sckt,recvBuff,tid);
	// recv(sckt,recvBuff,1024,0);
	// printf("%s\n", );

	if(strncmp(recvBuff, "220",3)==0){
		sprintf(sendBuff,"USER %s\r\n", username);
		writeS(sckt,sendBuff,tid);
	}

	readS(sckt,recvBuff,tid);
	if(strncmp(recvBuff,"331",3)==0){
		sprintf(sendBuff,"PASS %s\r\n", password);
		writeS(sckt,sendBuff,tid);
	}

	readS(sckt,recvBuff,tid);

	while(strncmp(recvBuff,"230-",4)==0){
		readS(sckt,recvBuff,tid);
		if (strstr(recvBuff,"230 ")){
			return 0;
		}
	}
}



void summary(){
	fprintf(stdout, "Filename:%s\tServer:%s\nPort:%d\t\t\tUsername:%s\nPassword:%s  Active:%d\nMode:%s\t\tLogfile:%s\n",gA.filename,gA.hostname,gA.portnr,gA.username,gA.password,gA.active,gA.mode,gA.log);
}

void setMode(int socket){
	char recvBuff[1024];
	char sendBuff[1024];
	// printf("%s\n",gA.mode);
	if(gA.mode == "binary"){
		sprintf(sendBuff,"TYPE L 8\r\n");
		writeF(socket,sendBuff);
		readF(socket,recvBuff);
	}
	else if(gA.mode == "ASCII"){
		sprintf(sendBuff,"TYPE A\r\n");
		writeF(socket,sendBuff);
		readF(socket,recvBuff);
	}
}

void setModeS(int socket,int tid){
	char recvBuff[1024];
	char sendBuff[1024];
	// printf("%s\n",gA.mode);
	if(gA.mode == "binary"){
		sprintf(sendBuff,"TYPE L 8\r\n");
		writeS(socket,sendBuff,tid);
		readS(socket,recvBuff,tid);
	}
	else if(gA.mode == "ASCII"){
		sprintf(sendBuff,"TYPE A\r\n");
		writeS(socket,sendBuff,tid);
		readS(socket,recvBuff,tid);
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
	int portNum;
	int swarm;
	struct sArgs *ptr;

	// struct sArgs *ptr2;
	// pthread_mutex_t readLock;
	// pthread_mutex_t writeLock;
	pthread_mutex_init(&readLock,NULL);
	pthread_mutex_init(&writeLock,NULL);

	gA.filename = NULL;
	gA.hostname = NULL;
	gA.portnr = 21;
	gA.username = "anonymous";
	gA.password = "user@localhost.localnet";
	gA.active = 0;
	gA.mode = "binary";
	gA.logfile = NULL;
	gA.log = NULL;
	gA.swarmfile = NULL;
	gA.swarming = 0;
	gA.output = 0;
	gA.threadCount = 0;


	extern char *optarg;
	extern int optind,optopt;

	if (argc == 1){
		fprintf(stderr, "Error: No options given\n\n");
		explain(stdout);
	}

	while((c = getopt_long(argc,argv,"hvf:s:p:n:P:am:l:w:",longOptions,&longIndex)) != -1){
		switch(c) {
			case 'h':
				explain(stdout);
				break;
			case 'v':
				fprintf(stdout, "mftp 0.1\nAuthor:Enrique Gutierrez");
				break;
			case 'f':
				// fprintf(stdout, " in f\n");
				gA.filename = optarg;
				break;
			case 's':
				gA.hostname = optarg;
				// fprintf(stdout, " in s\n");
				break;
			case 'p':
				gA.portnr = atoi(optarg);
				// fprintf(stdout, " in p\n");
				break;
			case 'n':
				gA.username = optarg;
				// fprintf(stdout, " in n\n");
				break;
			case 'w':
				gA.swarmfile = optarg;
				swarm = 1;
				break;
			case 'b':

				break;
			case 'P':
				gA.password = optarg;
				// fprintf(stdout, " in P\n");
				break;
			case 'a':
				gA.active = 1;
				// fprintf(stdout, "in a\n");
				break;
			case 'm':
				if (strcmp(optarg,"binary")==0){
					// gA.mode = optarg;
				}
				else if (strcmp(optarg,"ASCII")==0){
					gA.mode = optarg;
				}
				else if (strcmp(optarg,"BINARY")==0){
					// gA.mode = "binary";
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
					gA.log = optarg;
					gA.logfile = fopen(gA.log, "w");
				// gA.logging = 1;
				}
				// fprintf(stdout, " in l\n");
				break;
		}
	}

	if(swarm == 0){
		int sckt = establishConnection(gA.hostname, gA.portnr);
		// printf("%d\n",sckt);
		authenticate(sckt,gA.password,gA.username,0);

		if (gA.filename!=NULL){
			// fprintf(stdout, "about to get file\n");
			if(gA.active == 1){
				setActive(sckt);
			}
			else{
				portNum = setPasv(sckt);
				int dataSocket = establishConnection(gA.hostname, portNum);
				char recvBuff[1024];
				char sendBuff[1024];
				setMode(sckt);
				grabFile(sckt,dataSocket,gA.filename);
			}

		}
		summary();
		// fclose(gA.logfile);
	}
	else {
		char *username;
		char *hostname;
		char *password;
		char *filename;
		char *all_usernames[2048];
		char *all_passwords[2048];
		char *all_filenames[2048];
		char *all_hostnames[2048];
		char line[2048];
		// ptr2 = (struct sArgs *)malloc(sizeof(struct sArgs));


		FILE *fp = fopen(gA.swarmfile, "r");
		int internal_counter = 0;
		int num_threads = 0;
		char *position_ptr;
		while(fgets(line, sizeof(line), fp)) {
			position_ptr = strtok(line, "/");
			printf("%s\n", position_ptr );
			while(position_ptr != NULL) {
				if (internal_counter == 0) {
					position_ptr = strtok(NULL, ":"); // Deal with username
					username = (char *)malloc(strlen(position_ptr) + 1);
					//username = position_ptr;
					strcpy(username, position_ptr);
					internal_counter++;
					continue;
				}
				if (internal_counter == 1) {
					position_ptr = strtok(NULL, "@"); // password
					password = (char *)malloc(strlen(position_ptr) + 1);
					//password = position_ptr;
					strcpy(password, position_ptr);
					//printf("Password: %s\n", password );
					internal_counter++;
					continue;
				}
				if (internal_counter == 2) {
					position_ptr = strtok(NULL, "/"); // password
					hostname = (char *)malloc(strlen(position_ptr) + 1);
					//hostname = position_ptr;
					strcpy(hostname, position_ptr);
					//printf("hostname: %s\n", hostname );
					internal_counter++;
					continue;
				}
				if (internal_counter == 3) {
					position_ptr = strtok(NULL, " "); // password
					filename = (char *)malloc(strlen(position_ptr) + 1);
					//filename = position_ptr;
					strcpy(filename, position_ptr);
					//printf("filename: %s\n", filename);
					internal_counter++;
					continue;
				}
				position_ptr = strtok(NULL, " \n");
				internal_counter = 0;
				username = strtok(username, "/");
				printf("Username: %s\nPassword: %s\nHostname: %s\nFilename: %s\n", username, password, hostname, filename);
				// printf("HERE\n");
				// Store the values into char * arrays 
				all_usernames[num_threads] = username;
				// printf("HERE\n");
				all_passwords[num_threads] = password;
				all_filenames[num_threads] = filename;
				all_hostnames[num_threads] = hostname;
				//free(username);
				// printf("HERE\n");
				//free(password);
				//free(filename);
				//free(hostname);
					
			}
			num_threads++;
		}
		//GET SIZE OF FILE
		int sckt = establishConnection(all_hostnames[0], 21);
		authenticate(sckt,all_passwords[0],all_usernames[0],1);
		setMode(sckt);
		int size = grabSize(sckt,all_filenames[0]);
		int num_bytes = size/num_threads;
		int last = size%num_threads;
		fprintf(stdout, "Size: %d\n",size);
		fprintf(stdout, "%d\n", num_bytes);
		fprintf(stdout, "%d\n", last);
		close(sckt);
		gA.threadCount = num_threads;
		pthread_t *threadId = (pthread_t *)malloc(sizeof(pthread_t)* num_threads);
		int i;
		for(i=0;i<num_threads;i++){
			pthread_t thread;
			ptr = (struct sArgs *)malloc(sizeof(struct sArgs));
			// ptr = malloc (sizeof(sA)); 
			ptr->filename = all_filenames[i];
			ptr->username = all_usernames[i];
			ptr->password = all_passwords[i];
			ptr->portnr = gA.portnr;
			ptr->hostname = all_hostnames[i];
			ptr->tid = i;
			ptr->numbytes = num_bytes;
			ptr->last = last;
			// printf("In DA THREADS BRAH\n");
			pthread_create(&(threadId[i]),NULL,Part1,(void *)ptr);
		}
		int j;
		for(j=0;j<num_threads;j++){
			pthread_join(threadId[j],NULL);
		}
	}
}