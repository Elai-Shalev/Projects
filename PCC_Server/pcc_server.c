#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BACKLOG 10

int printable_char_count = [95];

void sig_handler(int signum){
    // do 
}

int* pcc_total; 

void init_pcc_total(){
    pcc_total = (int*)calloc(sizeof(int), 95)
    if(pcc_total < 0){
        perror("insufficient memory \n");
        exit(1);
    }
}


int main(int argc, char* argv[]){

    int listen_fd;
    int conn_fd;

    if(argc != 2){
        perror("Invalid Input!\n");
        exit(1);
    }

    // SOCKET INIT
    struct sockaddr_in serv_addr; // where we Want to get to
    struct sockaddr_in client_addr;   // where we actually connected through 
    struct sockaddr_in peer_addr; // where we actually connected to
    socklen_t addrsize = sizeof(struct sockaddr_in );    
    
    listen_fd = socket(AF_INET, SOCK_STREAM);
    if (listen_fd < 0){
        perror("Could not create socket \n");
        exit(1);
    }

    memset( &serv_addr, 0, addrsize );
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htonl(ntohl(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    

    // BIND
    if( 0 != bind(listen_fd, (struct sockaddr*) &serv_addr, addrsize)){
        // ERROR
        
    }

    // LISTEN
    if( 0 != listen(listen_fd, BACKLOG)){
        //ERROR
    }

    //CONNECT
    while(1)
    {
        connfd = accept( listenfd, (struct sockaddr*) NULL, NULL);
        if( connfd < 0 ) {
            // ERROR
        }
    }



    signal(SIGINT, sig_handler);

    close(conn_fd);
}

// signal handler 
//data structure 


int is_printable_char(char c){


}
