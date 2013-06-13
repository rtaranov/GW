	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
    #include <unistd.h>
    #include <fcntl.h>    
    #include <unistd.h>   
    #include <errno.h>    
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>     

    #define MAX_BYTES 20	 
	 
	void *call_cat( void *ptr );
	void *processFile( void *ptr );
	
	//*************Service functions********************
	
	void replaceElement (char *buf, int bytes, char with);
	void fullWrite (int sockfd, char *buf, int bytes);

	void error(const char *msg);
	
	char ifFileStarted = 0;
	
	int sockfd;		
	 
	main(int argc, char *argv[])
	{
	  pthread_t thread1, thread2;
	  char *message1 = "Thread 1";
	  char *message2 = "Thread 2";
	  int  iret1, iret2;	 
      int  portno;
      struct sockaddr_in serv_addr;
      struct hostent *server;

      char buffer[256];
      
      if (argc < 3) 
      {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
      } 
      
      portno = atoi(argv[2]);
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) 
        error("ERROR opening socket");
        
      server = gethostbyname(argv[1]);
      
      if (server == NULL) 
      {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
      }
      
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
      serv_addr.sin_port = htons(portno);
      
      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");    
                	 
	  iret1 = pthread_create( &thread1, NULL, call_cat, (void*) message1);
	  iret2 = pthread_create( &thread2, NULL, processFile, (void*) message2);	 
	 
	  pthread_join( thread1, NULL);
	  pthread_join( thread2, NULL);
	 
	  printf("Thread 1 returns: %d\n",iret1);
	  printf("Thread 2 returns: %d\n",iret2);
	  
	  exit(0);
	}
	 
	void *call_cat( void *ptr )
	{
      char buf[256];

      sprintf(buf, "cat /dev/ttyS4 > /home/japonomatj/com/r");
      printf("%s \n", buf);  
      ifFileStarted = 1;            
      system(buf);	                
	}

	void *processFile( void *ptr )
	{
      int    fd; 
      size_t bytes; 
      char   buffer[MAX_BYTES+2]; 
      
      while(1)
	  {
        if(ifFileStarted == 1)
	    {
		  int fileLen = 0;
		
          fd = open("/home/japonomatj/com/r", O_RDONLY | O_NONBLOCK);

          if (fd == -1) 
          {
            if (errno = EAGAIN)
              puts("Open would block");
            else
              puts("Open failed");
            exit(-1);
          }
          else
          {
		   puts("File Opened!");
		  } 
		                    
          while(1)
          {
            do 
            {
               bzero(buffer,MAX_BYTES);
             
               bytes = read(fd, buffer, MAX_BYTES-1);

               if (bytes == -1) 
               {
                 if (errno = EAGAIN)
 	               puts("Normally I'd block, but you told me not to");
                 else
	               puts("Another read error");
                 exit(-1);
               }

               if (bytes > 0) 
               {
			     fileLen += bytes;
	             replaceElement (buffer, bytes, ' ');                                   

                 fullWrite (sockfd, buffer, bytes);
               
                 bzero(buffer,MAX_BYTES);
             
                 read(sockfd, buffer, MAX_BYTES);
                }           
             }
             while (bytes > 0);
           
             sleep(1);            
           
           }//while(1)           
	    }//if(ifFileStarted == 1)
	    else
	    {
	      sleep(1);
	    }
      }      
	  close(sockfd);
	}
	
	void fullWrite (int sockfd, char *buf, int bytes)
	{
	  int n;
	  
      do
      {				  
        n = write(sockfd,buf,bytes);

        if (n < 0) 
          error("ERROR writing to socket");
                   
      }while ((n-bytes) > 0);		
	}
	
	void replaceElement (char *buf, int bytes, char with)
	{
      int i;
      
      for(i=0; i<bytes; i++)
      {
		if(buf[i] == 0)
		  buf[i]=with;
	  }      
	}

	void error(const char *msg)
	{
      perror(msg);
      exit(0);
	}
