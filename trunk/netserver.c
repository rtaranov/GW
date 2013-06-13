
	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
    #include <unistd.h>
    #include <fcntl.h>    /* for open */
    #include <unistd.h>   /* for read */ 
    #include <errno.h>    /* for errno */
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>    
    #include <arpa/inet.h>
#define BUF_SIZE 20

int main(int argc, char ** argv)
{
     int sock, newsock, port, clen;
     char buf[BUF_SIZE];
     struct sockaddr_in serv_addr, cli_addr;
     if (argc < 2) 
     {
         fprintf(stderr,"usage: %s <port_number>\n", argv[0]);
         return EXIT_FAILURE;
     }
     sock = socket(AF_INET, SOCK_STREAM, 0);
     if (socket < 0)
     {
       printf("socket() failed: %d\n", errno);
       return EXIT_FAILURE;
     }
     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     port = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(port);
     if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     {
       printf("bind() failed: %d\n", errno);
       return EXIT_FAILURE;
     }
     listen(sock, 1);
     clen = sizeof(cli_addr);
     newsock = accept(sock, (struct sockaddr *) &cli_addr, &clen);
     if (newsock < 0) 
     {
       printf("accept() failed: %d\n", errno);
       return EXIT_FAILURE;
     }
     memset(buf, 0, BUF_SIZE);
     
     while(1)
     {
       read(newsock, buf, BUF_SIZE-1);
 
       buf[BUF_SIZE] = 0;
       printf("%s", buf);
       memset(buf, 0, BUF_SIZE);
       write(newsock, "OK\0", 3);
     }
     close(newsock);
     close(sock);
}
