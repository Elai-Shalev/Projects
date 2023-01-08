#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>




int main(int argc, char* argv[]){

    uint32_t file_size; 
    int fd; 
    int  sockfd     = -1;
    int  bytes_read =  0;

    struct sockaddr_in serv_addr; // where we Want to get to
    struct sockaddr_in client_addr;   // where we actually connected through 
    struct sockaddr_in peer_addr; // where we actually connected to
    socklen_t addrsize = sizeof(struct sockaddr_in );

    if(argc != 4){
        perror("Invalid Input!\n");
        exit(1);
    }

    fd = open(argv[3], "O_RDONLY");
    if (fd < 1){
        perror("Error opening file");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM);
    if (sockfd < 0){
        // ERROR
        //exit(1); // ??
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htonl(argv[2]);


    




}