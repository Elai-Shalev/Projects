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
#include <signal.h>


#define BACKLOG 10
#define N_SIZE 4
#define MB 500
#define _GNU_SOURCE


void update_pcc_total();
void init_pcc_total();
void update_pcc_current(char* recv_buff, int n);
void print_pcc();



unsigned int* pcc_total; 
unsigned int* pcc_current;
uint32_t C;
uint32_t total_C;
int stop_service =0;
int conn_fd = -1;

void signal_handler(int signum){
    
    if(conn_fd == -1){
        print_pcc();
        free(pcc_current);
        free(pcc_total);
        exit(0);
    }
    else{
        stop_service = 1; 
    }
    
    return;
} 

void init_pcc(){
    pcc_total = (unsigned int*)calloc(sizeof(unsigned int), 95);
    if(pcc_total < 0){
        perror("insufficient memory \n");
        exit(1);
    }
    pcc_current = (unsigned int*)calloc(sizeof(unsigned int), 95);
    if(pcc_current < 0){
        perror("insufficient memory \n");
        exit(1);
    }
}

void update_pcc_total(){
    
    total_C += C;
    for(int i = 0; i < 95; i++){
        pcc_total[i] += pcc_current[i]; 
    }

    for(int j=0; j<95; j++){
        pcc_current[j] = 0;
    }
    
    C = 0;
}

void update_pcc_current(char* recv_buff, int n){

    for(int i=0; i < n; i++){

        int c_i = (int)recv_buff[i];
        if (c_i >= 32 && c_i <= 126){
            pcc_current[(int)(c_i - 32)]++;
            C++;
        }
    }

}

void print_pcc(){

    for(int i=0; i<95; i++){
        printf("char '%c' : %u times\n", (char)(i+32), pcc_total[i]);
    }
}


int main(int argc, char* argv[]){

    int listen_fd;
    uint32_t N;
    //int read_from_buff =  0;
    int bytes_written = 0;
    //int total_sent = 0;
    //int total_left = 0;
    int n = 0;
    int optval = 1;
    int bytes_left = 0;
    int bytes_read = 0;
    int done = 1;
    
    char* recv_buff; // Size will vary 

    if(argc != 2){
        perror("Invalid Input!\n");
        exit(1);
    }


    //Signal Init

    signal(SIGINT, signal_handler);

    init_pcc(); 

    // SOCKET INIT
    struct sockaddr_in serv_addr; // where we Want to get to
    struct sockaddr_in peer_addr;
    socklen_t addrsize = sizeof(struct sockaddr_in);    
    
    listen_fd = socket(AF_INET, SOCK_STREAM,0);
    if (listen_fd < 0){
        perror("Could not create socket \n");
        exit(1);
    }

    //SO_REUSE
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        // handle setsockopt error
        perror("set sock problem\n");
        exit(1);
    }

    memset( &serv_addr, 0, addrsize );
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = atoi(argv[1]);
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    

    // BIND
    if( 0 != bind(listen_fd, (struct sockaddr*) &serv_addr, addrsize)){
        perror("Bind Problem\n");
        exit(1);
        
    }

    // LISTEN
    if( 0 != listen(listen_fd, BACKLOG)){
        perror("Listen Problem\n");
        exit(1);
    }

    recv_buff = (char*)malloc(MB); //allocating first MB


    
    while(stop_service != 1){
        
        //Accept and establish Connection
        conn_fd = accept(listen_fd, (struct sockaddr*) &peer_addr, &addrsize);
        if(conn_fd < 0) {
            perror("Accept Problem\n");
            exit(1);
        }
        done =1;
        
        bytes_left = N_SIZE;
        bytes_read = 0;
        //read_from_buff =0;
        N = 0;       

        // Recieve N
        while(bytes_read < N_SIZE){
            n = read(conn_fd, &N + bytes_read, bytes_left); //reading into N
            
            if (n == 0 || (n<0 && errno != EINTR)){
                fprintf(stderr, "Error in recieving N\n");
                done =0; 
                break;
            }
            
            if(n > 0){
                bytes_read += n;
                bytes_left -= n;    
            }

        }
        
        
        //Prepare for reading N bytes
        N = ntohl(N); // Convert to host order 
        //recv_buff = (char*)malloc(MB); //allocating first MB
        
        for(int j=0; j<MB; j++){
            recv_buff[j] = 0;
        }

        bytes_left = N;
        bytes_read = 0;
        //read_from_buff = 0; 

        if (done == 1){
            //Read N Bytes
            while(bytes_left > 0){
                
                n = read(conn_fd, recv_buff, MB);
                if (n == 0 || (n<0 && errno != EINTR)){
                    fprintf(stderr, "Error in reading \n");
                    done = 0; 
                    break;
                }
            
                if(n > 0){
                    bytes_left -= n;
                    update_pcc_current(recv_buff, n);
                }
            }
        }


        // all N bytes are read
        // Send C to the client
        bytes_left = N_SIZE; // N is a 32 bit number 
        bytes_written = 0;
        C = htonl(C);

        if(done == 1){
        while(bytes_left > 0)
            {
                //print_pcc();
                n = write(conn_fd, &C + bytes_written, bytes_left);  
                
                if (n == 0 || (n<0 && errno != EINTR)){
                    fprintf(stderr, "Error in writing\n");
                    done = 0; 
                    break;
                }

                if(n > 0){
                    bytes_written  += n;
                    bytes_left -= n;
                }
            }   
        }

        if (done){
            update_pcc_total();
        }
        close(conn_fd);
        conn_fd = -1;
    }

    close(conn_fd);
    conn_fd = -1;
    free(recv_buff);
    print_pcc();
    free(pcc_current);
    free(pcc_total);
    exit(0);
}
