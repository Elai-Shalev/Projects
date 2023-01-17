#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BACKLOG 10
#define N_SIZE 4
#define MB 1000000

int printable_char_count = [95];

void sig_handler(int signum){
    // do 
}

int* pcc_total; 
int* pcc_current;

void init_pcc_total(){
    pcc_total = (int*)calloc(sizeof(int), 95)
    if(pcc_total < 0){
        perror("insufficient memory \n");
        exit(1);
    }
}

void update_pcc_total(int * pcc_total){
}

void update_pcc_current(int * pcc_current){

}


int main(int argc, char* argv[]){

    int listen_fd;
    int conn_fd;
    int read_from_buff =  0;
    int written_to_buff = 0;
    int left_in_buff =0;
    int total_sent 0;
    int total_left 0;
    int n_sent = 0;
    int optval = 1;
    uint32_t N;
    uint32_t C;
    
    char* recv_buff;
    recv_buff = (char*)malloc(MB); 
    if(recv_buff < 0){
        perror("insufficient memory \n");
        exit(1);
    }


    if(argc != 2){
        perror("Invalid Input!\n");
        exit(1);
    }

    init_pcc_total(); 

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

    //SO_REUSE
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) < 0)) {
        // handle setsockopt error
    }

    memset( &serv_addr, 0, addrsize );
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htonl(ntohl(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    

    // BIND
    if( 0 != bind(listen_fd, INADDR_ANY, addrsize)){
        // ERROR
        
    }

    // LISTEN
    if( 0 != listen(listen_fd, BACKLOG)){
        //ERROR
    }


    // HANDLE FIRST READ //====///
    
    // Recieve N
    connfd = accept( listenfd, (struct sockaddr*) NULL, NULL);
    if( connfd < 0 ) {
        // ERROR
    }

    read_from_buff = 0; 
    bytes_left = N_SIZE;
    bytes_read = 0;
    while(bytes_read < N_SIZE){

        n = read(conn_fd, &N + bytes_read, N_SIZE);
        if(n < 0){
            perror("read call failed \n");
            exit(1);
        }
        bytes_read += n;
        bytes_left -= n;
    }
    close(conn_fd);

    bytes_left = N;
    bytes_read = 0;
    // Accepting, reading and writing each time
    while(bytes_left > 0)
    {
        connfd = accept( listenfd, (struct sockaddr*) NULL, NULL);
        if( connfd < 0 ) {
            // ERROR
        }


        // init recv buffer and
        n = read(conn_fd, &recv_buff + bytes_read, N_SIZE);
        if(n < 0){
            perror("read call failed \n");
            exit(1);
        }
        bytes_read += n;
        bytes_left -= n;


        update_pcc_current()







        close(conn_fd);
    }
    
        
    }



    signal(SIGINT, sig_handler);

    
}

// signal handler 
//data structure 


int is_printable_char(char c){


}
