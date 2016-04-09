# FTP_Client

Simple FTP Client 

##Usage
compile main.c 
```
gcc main.c -o mftp
``` 
or run Makefile. Then run 
```
./mftp [options]
OPTIONS:
-h, --help                       Displays this help information
-v, --version                    Prints name of application, version number and author 
-f, --file [file]                Specifies file to download
-s, --server [hostname]          Specifies the server to download the file from
-p, --port [port]                Specifies the port to be used when contacting the server (default:21)
-n, --username [user]            Uses the username user when logging into the FTP server (default:anonnymous)
-P, --password [password]        Uses the password password when loggin into the FTP server(default:user@localhost.loalnet)
-m, --mode [mode]                Specifies the mode to be used for the transfer (ASCII or binary)(default:binary)
-l, --log [logfile]              Logs all the FTP commands exchanged with the server and the corresponding replies to file logfile. If the filename is '-' then the commands are printed to stdout
```
