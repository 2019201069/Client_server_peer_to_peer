#include "trackerutil.h"


// void senddata(){
//     int sockfd, portno, n;
//     struct sockaddr_in ser_addr;

//     char buffer[BUFFSIZE],server_reply[BUFFSIZE];
    
//     //portno = 12009;
//     portno = PORTNOC;
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
//     ser_addr.sin_family = AF_INET;
//     ser_addr.sin_addr.s_addr = inet_addr(IPC);
//     ser_addr.sin_port = htons(portno);
    
//     int status = connect(sockfd,(struct sockaddr *) &ser_addr,sizeof(ser_addr));
//     if (status < 0)
//         perror("Error in connecting");
//     else
//         printf("Connected...\n");
     
//      // while(1)
//      // {

//         FILE *fp = fopen("anj.txt", "rb");

//         fseek(fp, 0, SEEK_END);

//         int size = ftell(fp);

//         rewind(fp);

//         send (sockfd , &size, sizeof(size), 0); // first send size of file

//         printf("%d \n", size);
//         char Buffer [BUFFSIZE] ; 
//         while ( ( n = fread( Buffer , sizeof(char) , BUFFSIZE , fp ) ) > 0  && size > 0 ){
//                 printf("%s \n", Buffer);
//                 send (sockfd , Buffer, n, 0 );
//                 memset ( Buffer , '\0', BUFFSIZE);
//                 size = size - n ;
//             }

//         fclose (fp);

//         recv(sockfd , server_reply , BUFFSIZE , 0);
//         printf("Server reply : %s", server_reply);
//      // }
    
//     close(sockfd);

// }


// void recievedata(){
    
//      int sockfd, newsockfd, portno;
//      pid_t pid;
//      socklen_t clientlen;
//      char buffer[BUFFSIZE];
//      struct sockaddr_in ser_addr, cli_addr;
//      int n;
     
//      sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
//      portno = PORTNOS;
//      ser_addr.sin_family = AF_INET;
//      ser_addr.sin_addr.s_addr = inet_addr(IPS);
//      ser_addr.sin_port = htons(portno);
     
//     bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    
//     printf("Server started... Waiting for connection...\n");
    
//     listen(sockfd,5);
    
    
//     clientlen = sizeof(cli_addr);
//     while(1){
    
//         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clientlen);   // accept from the client --> new sockid contains 
        
//         if((pid = fork()) == 0){
//             FILE *fp = fopen ( "cutie.txt"  , "wb" );
//             int fd=fileno(fp);
//             char Buffer[BUFFSIZE] ; 
//             int file_size;

//             recv(newsockfd, &file_size, sizeof(file_size), 0);

//             while (( n = recv(newsockfd , Buffer , BUFFSIZE, 0) ) > 0  &&  file_size > 0){
//                 printf("%d no of bytes  \n", n);
//                 write(fd, Buffer, n);
//                 printf(" buffer has : %s \n", Buffer);
//                 memset ( Buffer , '\0', n);
//                 file_size = file_size - n;
//             } 

//             char msg[100] = "here";
//             printf("Sent to client : %s\n",msg);
//             fclose (fp);
//             close(fd);
//             send(newsockfd , msg , BUFFSIZE, 0);
          
//         }

//     }  // end while 1
//         close(newsockfd);
//         close(sockfd);
   

// }
int sockfd;

void * recievemsgdata(void * threadarg){

    int * newsockfd =  (int *)(threadarg);
    char buffer[BUFFSIZE], msg_data[MSGSIZE]; //params[20][BUFFSIZE]
            string params[20];

            int option;
            recv(*newsockfd, msg_data, sizeof(msg_data), 0);  // get the option from client socket
            
            cout<<msg_data<<endl;
            params[0] = strtok(msg_data,COL);
            //strcpy(params[0],strtok(msg_data,COL));
            int params_num = 0;
            while(params[params_num] != ";"){  // delim used at the end
                params_num++;
                params[params_num] = strtok(NULL,COL);
                cout<<params[params_num]<<endl;
                if(params[params_num] == ";")
                    break;
            }

            option = atoi(params[0].c_str());

            cout<<"option is : "<<option<<endl;
            cout<<"no of params is : "<<params_num<<endl;

            string msg = check_option(option, params, params_num);
            //printf("Sent to client : %s\n",msg);
            cout<<"Sent to client"<<msg;
            char msg_c[100];
            strcpy(msg_c,msg.c_str());
            send(*newsockfd , msg_c , BUFFSIZE, 0);
}

void recievedata(){//void * threadarg){
    
      //params[20][BUFFSIZE]
     int newsockfd, portno;
     struct sockaddr_in ser_addr, cli_addr;
     socklen_t clientlen;
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
        
     portno = PORTNOS;   // serverport no
     ser_addr.sin_family = AF_INET;
     ser_addr.sin_addr.s_addr = inet_addr(IPS);  // server ip
     ser_addr.sin_port = htons(portno);
     
    bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    
    printf("Tracker started... Waiting for connection...\n");
    
    listen(sockfd,5);

    
    int i = 0;
    clientlen = sizeof(cli_addr);
    while(1){
        
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clientlen);   // accept from the client --> new sockid contains 
        if(newsockfd < 0){
            cout<<"unable to accept the socket";
            exit(EXIT_FAILURE);
        }
        
        pthread_t trackerlistnerThread[10];
        int thread_status = pthread_create(&trackerlistnerThread[i], NULL, recievemsgdata, (void *)&newsockfd);
        if(thread_status != 0){
            cout<<"thread creation failed"<<endl;
        } 
        if( i >= 3)
            {
                i = 0;
                while(i < 3)
                {
                        pthread_join(trackerlistnerThread[i++],NULL);
                }
                 i = 0;
            }

    }  // end while 1


        close(newsockfd);
        close(sockfd);

}




int main()
{

    
    recievedata();

    return 0;
}

