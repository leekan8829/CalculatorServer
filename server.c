#include <stdio.h>            /*  standard I/O  */
#include <stdlib.h>           /*  standard library */
#include <string.h>           /*  string library */
#include <sys/socket.h>       /*  socket definitions */
#include <sys/types.h>        /*  socket types       */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>

#define  MYPORT    12345
#define  BACKLOG   5
#define  MAXLENGTH 1024
int main(void)
{
    int  sockfd, new_fd,ret; 
    struct sockaddr_in   my_addr;
    struct sockaddr_in   their_addr; 
    socklen_t sin_size;
    char buffer[MAXLENGTH] = {};

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET; // host byte order
    my_addr.sin_port = htons(MYPORT); // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), 0, 8); // zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
       exit(1);
     }
     if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
         exit(1);
     }

     while(1) {                     // main accept() loop

        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
             perror("accept");
             continue;
        }

        int pid = fork();

        if(pid == 0){
            printf("accept from IP: %s\n",inet_ntoa(their_addr.sin_addr));
            memset(buffer,0,sizeof(buffer));
            ret = read(new_fd,buffer,MAXLENGTH);
            float x=0,y=0,result = 0;
            char *response;

            char op;
            sscanf(buffer, "%f %c %f", &x, &op, &y);
            switch (op) {
            case '+':
                result = x+y;
                break;
            case '-':
                result = x-y;
                break;
            case '*':
                result = x*y;
                break;
            case '/':
                result = x/y;
                break;
            default:
                break;
            }
            printf("%f\n",result);
            response = (char *)malloc(50 * sizeof(char));
            snprintf(response, 50, "%.2f", result);
            send(new_fd,response, sizeof(response), 0);
        }
        else if(pid<0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        close(new_fd); 

    } // end of while
    return 0;
}
