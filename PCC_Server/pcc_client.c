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

#define MB 1000000
#define N_SIZE 4
uint32_t get_file_size(FILE* fd);

int main(int argc, char* argv[]){

    uint32_t file_size; 
    FILE * fd;
    uint32_t N_nbo; 

    int sockfd     = -1;
    int client_to_srver_fd    = -1;
    int client_to_srver_fd    = -1;
    int read_from_buff =  0;
    int written_to_buff = 0;
    int left_in_buff =0;
    int total_sent 0;
    int total_left 0;
    int n_sent = 0;
    uint32_t C = 0;
    
    char* recv_buff;
    char* send_buff;


    struct sockaddr_in serv_addr; // where we Want to get to
    struct sockaddr_in client_addr;   // where we actually connected through 
    struct sockaddr_in peer_addr; // where we actually connected to
    socklen_t addrsize = sizeof(struct sockaddr_in );
    

    if(argc != 4){
        perror("Invalid Input!\n");
        exit(1);
    }

    fd = fopen(argv[3], "O_RDONLY");
    if (fd < 1){
        perror("Error opening file");
        exit(1);
    }

    // get file size in Network Byte order
    file_size = get_file_size(fd);
    N_nbo = htonl(file_size);

    send_buff = (char*)malloc(MB); 
    if(send_buff < 0){
        perror("insufficient memory \n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM);
    if (sockfd < 0){
        perror("Could not create socket \n");
        exit(1);
    }

    memset(&serv_addr, 0, addrsize);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htonl(argv[2]); //specified port
    serv_addr.sin_addr.s_addr = inet_addr(inet_pton(argv[1])); //specified IP address 

    if(connect(sockfd, (struct sockaddr*) &serv_addr, addrsize) < 0) {
        perror(" Could not connect \n");
        exit(1);
    }

    // sending N 
    memset(send_buff, N_nbo, N_SIZE);
    bytes_left = N_SIZE; // N is a 32 bit number 
    while(bytes_left > 0)
    {
        n_sent = write(sockfd, send_buff + bytes_written, bytes_left);
        // check if error occured (client closed connection?)
        
        if(n_sent < 0){
            perror(" Write call failed \n");
            exit(1);
        }

        bytes_written  += n_sent;
        bytes_left -= n_sent;
    }

    // N is sent 
    // Now sending the data 

    memset(send_buff, 0, MB);

    total_left = file_size; 
    total_bytes_sent = 0; 
    read_from_buff = 0; 
    written_to_buff = 0; 
    
    while(total_left > 0){

        // 1 MB of data in buffer
        n = fread(send_buff, sizeof(char), MB/sizeof(char), fd);
        if (n < 0){
            perror(" Fread call failed \n");
            exit(1);
        }
        left_in_buff = n; 
        read_from_buff = 0; 
        
        while(left_in_buff > 0){
            n_sent = write(sockfd, send_buff + read_from_buff, left_in_buff);
            // check if error occured (client closed connection?)
            if(n_sent < 0){
                perror(" write call failed \n");
                exit(1);
            }
            
            read_from_buff  += nsent;
            left_in_buff -= nsent;
        }
        total_sent += read_from_buff;
        total_left -= read_from_buff;
        memset(send_buff, 0, MB);

    }

    // Reading the number of prontable chars from Server 
    
    recv_buff = &C;
    read_from_buff = 0; 
    bytes_left = N_SIZE;
    while(bytes_read < N_SIZE){

        n = read(sockfd, recv_buff + bytes_read, N_SIZE);
        if(n < 0){
            perror("read call failed \n");
            exit(1);
        }
        bytes_read += n;
        bytes_left -= n;
    }

    printf("# of printable characters: %u\n", ntohl(C));
    close(fd);
    free(send_buff);
    free(recv_buff);
    return 0; 
}

uint32_t get_file_size(FILE* fd){

    uint32_t size;
    // Code from Stack OverFlow
    //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
    fseek(fd, 0, SEEK_END); // seek to end of file
    size = (uint32_t)ftell(fd); // get current file pointer
    fseek(fd, 0, SEEK_SET); // seek back to beginning of file
    return size;
}







    




}