//lab2

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void sendFile(int * sockfd, FILE * transFile, int fsize, char * filename)
{
	char * buf = (char *) malloc(sizeof(1000));
	bzero(buf,1000);
	int netfsize = htons(fsize);	
	memcpy(buf, &netfsize,sizeof(int));
	memcpy(buf+4,filename,strlen(filename));
	int read = fread(buf+24,1,1000-24,transFile);
	while(read != 1000-24)
	{
		if(feof(transFile))
		{
			break;
		}
		else
		{
			fseek(transFile,0,SEEK_SET);
			read = fread(buf+24,1,1000-24,transFile); 
			continue;
		}
	}
	send(*sockfd,buf,read,0);
	
	while(!feof(transFile))
	{
		read = fread(buf,1,1000,transFile);
		if(send(*sockfd,buf,read,0) != read)
		{
			fprintf(stderr,"%s\n","failed to send correctly");
			exit(0);
		}
	}

	return;
}

unsigned long fileSize(const char *filePath)
{
 
 struct stat fileStat;//declare file statistics struct
 
 stat(filePath,  &fileStat);//filling fileStat with statistics of file
 
 return fileStat.st_size;//returning only the size of file from stat stru    ct
}



void createConnection(int * sockfd, struct sockaddr_in * sockaddr, char *port, char * serverIp)
{
	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0))
	{
		fprintf(stdout, "%s\n", "socket could not be made");
		exit(0);
	}
	
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(atoi(port));
	sockaddr->sin_addr.s_addr = inet_addr(serverIp);	
	memset(&(sockaddr->sin_zero),'\0',8);
	if(connect(*sockfd,(struct sockaddr *)sockaddr,sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s\n", "failed to connect");
		close(*sockfd);
		exit(0);
	}
	return;
}


int main(int args, char * argv[])
{
	if (args != 4)
	{
		fprintf(stderr,"%s %d %s\n" , "please give three aurguments,",args - 1,"were entered, the aurguments are remote-ip, remote-port, local-file-to-transfer");

	}
	
	FILE* transFile = fopen(argv[3],"r");//trying to open local-file-to-transfer  in read mode

	if(transFile == NULL)//Checking if input file was opened correctly
 	{
		fprintf(stdout,"%s","please enter a valid path to an existing file for the third argument\n");
		exit(0);//if not opened correctly, exit program
	}
 	int fsize = fileSize(argv[3]);	
	int sockfd;
	struct sockaddr_in sockaddr;		
	createConnection(&sockfd, &sockaddr, argv[2], argv[1]);
	sendFile(&sockfd,transFile,fsize,argv[3]);
	fprintf(stdout, "%s %lu %d \n", "ended normally", sizeof(int), fsize);
}
