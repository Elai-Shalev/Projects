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

#define BACKLOG 10
#define N_SIZE 4
#define MB 1000000
#define _GNU_SOURCE


void update_pcc_total();
void init_pcc_total();
void update_pcc_current(char* recv_buff);

void sig_handler(int signum){
    // do 
}

int* pcc_total; 
int* pcc_current;
uint32_t C;
uint32_t total_C;


void init_pcc(){
    pcc_total = (int*)calloc(sizeof(int), 95);
    if(pcc_total < 0){
        perror("insufficient memory \n");
        exit(1);
    }
    pcc_current = (int*)calloc(sizeof(int), 95);
    if(pcc_current < 0){
        perror("insufficient memory \n");
        exit(1);
    }
}

void update_pcc_total(){
    
    total_C += C;
    for(int i =0; i<95; i++){
        pcc_total[i] += pcc_current[i]; 
    }
    memset(pcc_current, 0, 95);
}

void update_pcc_current(char* recv_buff){

    for(int i=0; i<strlen(recv_buff); i++){

        int c_i = (int)recv_buff[i];
        if (c_i >= 32 && c_i <= 126){
            pcc_current[(int)(c_i - 32)]++;
            C++;
        }
    }

}


int main(int argc, char* argv[]){

    int listen_fd;
    int conn_fd;
    int read_from_buff =  0;
    int bytes_written = 0;
    //int total_sent = 0;
    //int total_left = 0;
    int n = 0;
    int optval = 1;
    int bytes_left = 0;
    int bytes_read = 0;
    uint32_t N;
    
    char* recv_buff; // Size will vary 

    if(argc != 2){
        perror("Invalid Input!\n");
        exit(1);
    }

    init_pcc(); 

    // SOCKET INIT
    struct sockaddr_in serv_addr; // where we Want to get to
    socklen_t addrsize = sizeof(struct sockaddr_in );    
    
    listen_fd = socket(AF_INET, SOCK_STREAM,0);
    if (listen_fd < 0){
        perror("Could not create socket \n");
        exit(1);
    }

    //SO_REUSE
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) < 0)) {
        // handle setsockopt error
        perror("set sock problem\n");
        exit(1);
    }

    memset( &serv_addr, 0, addrsize );
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htonl(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    

    // BIND
    if( 0 != bind(listen_fd, INADDR_ANY, addrsize)){
        perror("Bind Problem\n");
        exit(1);
        
    }

    // LISTEN
    if( 0 != listen(listen_fd, BACKLOG)){
        perror("Listen Problem\n");
        exit(1);
    }

    
    while(1){

        // Handle Sigint 


        //Accept and establish Connection
        conn_fd = accept( listen_fd, (struct sockaddr*) NULL, NULL);
        if(conn_fd < 0) {
            perror("Accept Problem\n");
            exit(1);
        }

        bytes_left = N_SIZE;
        bytes_read = 0;

        // Recieve N
        while(bytes_read < N_SIZE){
            n = read(conn_fd, &N + bytes_read, bytes_left); //reading into N
            if(n < 0){
                perror("read call failed \n");
                exit(1);
            }
            bytes_read += n;
            bytes_left -= n;
        }
        
        
        //Prepare for reading N bytes
        N = htonl(N); // Convert to host order 
        recv_buff = (char*)malloc(MB); //allocating first MB
        bytes_left = N;
        bytes_read = 0;
        read_from_buff = 0; 

        //Read N Bytes
        while(bytes_left > 0){
            
            while (read_from_buff < MB){
                n = read(conn_fd, recv_buff + read_from_buff, MB-read_from_buff);
                if(n < 0){
                    perror("read call failed \n");
                    exit(1);
                }
                bytes_read += n;
                bytes_left -= n;
                read_from_buff += n;
                
            }
            // buffer full
            update_pcc_current(recv_buff);
            read_from_buff = 0;         
        }
        // all N bytes are read

        // Send C to the client
        bytes_left = N_SIZE; // N is a 32 bit number 
        bytes_written = 0;
        C = ntohl(C);

        while(bytes_left > 0)
        {
            n = write(conn_fd, &C, bytes_left);  
            if(n < 0){
                perror(" Write call failed \n");
                exit(1);
            }

            bytes_written  += n;
            bytes_left -= n;
        }
        update_pcc_total();
        close(conn_fd);
    }

    free(pcc_current);
    free(recv_buff);
    free(pcc_total);
    
}
