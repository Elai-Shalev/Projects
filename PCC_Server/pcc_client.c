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

#define MB 500
#define N_SIZE 4
uint32_t get_file_size(FILE* fd);

int main(int argc, char* argv[]){

    uint32_t file_size; 
    FILE * fd;
    uint32_t N;

    int sockfd     = -1;
    int read_from_buff =  0;
    int left_in_buff =0;
    int total_sent =0;
    int total_left =0;
    int bytes_read =0;
    int bytes_written = 0;
    int bytes_left =0;
    int n = 0;
    uint32_t C = 0;
    
    char* send_buff;


    struct sockaddr_in serv_addr; // where we Want to get to
    socklen_t addrsize = sizeof(struct sockaddr_in );
    

    if(argc != 4){
        perror("Invalid Input!\n");
        exit(1);
    }

    fd = fopen(argv[3], "r");
    if (fd == NULL){
        perror("Error opening file");
        exit(1);
    }

    // get file size in Network Byte order
    file_size = get_file_size(fd);
    N = htonl(file_size);

    send_buff = (char*)malloc(MB); 
    if(send_buff < 0){
        perror("insufficient memory \n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd < 0){
        perror("Could not create socket \n");
        exit(1);
    }

    memset(&serv_addr, 0, addrsize);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2])); //specified port
    if ( 1 > inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr)){
        perror("client failed at inet pton\n");
        close(sockfd);
        return 1;
    }

    if(connect(sockfd, (struct sockaddr*) &serv_addr, addrsize) < 0) {
        perror(" Could not connect \n");
        exit(1);
    }

    // sending N 
    bytes_left = N_SIZE; // N is a 32 bit number 
    bytes_written = 0;
    while(bytes_left > 0)
    {
        n = write(sockfd, &N + bytes_written, bytes_left);
        if (n == 0 || (n<0 && errno != EINTR)){
            perror("Write N failed\n");
            //done = 0; 
            exit(1);
        }
    
        if(n > 0){
            bytes_written  += n;
            bytes_left -= n;
        }

        
    }

    // N is sent 
    // Now sending the data 

    
    for(int j=0; j < MB; j++){
        send_buff[j] = 0;
    }

    total_left = file_size; 
    total_sent = 0; 
    read_from_buff = 0; 
    
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
            n = write(sockfd, send_buff + read_from_buff, left_in_buff);
            if (n == 0 || (n<0 && errno != EINTR)){
                fprintf(stderr, "Error in writing \n");
                //done = 0; 
                exit(1);
            }
        
            if(n > 0){
                read_from_buff  += n;
                left_in_buff -= n;
            }
            
        }
        
        total_sent += read_from_buff;
        total_left -= read_from_buff;
        
    }

    // Reading the number of prontable chars from Server 
    
    read_from_buff = 0; 
    bytes_left = N_SIZE;
    bytes_read = 0;
    while(bytes_read < N_SIZE){

        n = read(sockfd, &C + bytes_read, bytes_left);
        
        if (n == 0 || (n<0 && errno != EINTR)){
            //done = 0; 
            exit(1);
        }
    
        if(n > 0){
            bytes_read += n;
            bytes_left -= n;
        }
        
    }
    printf("# of printable characters: %u\n", ntohl(C));
    fclose(fd);
    free(send_buff);
    
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



