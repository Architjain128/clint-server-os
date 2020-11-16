#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
typedef long long int ll;
typedef long double ld;
#define f(a,b,i) for(ll i=a;i<b;i++) 
#define BRED "\033[1;31m"
#define LRED "\033[0;31m"
#define BGREEN "\033[1;32m"
#define LGREEN "\033[0;32m"
#define BYELLOW "\033[1;33m"
#define LYELLOW "\033[0;33m"
#define BBLUE "\033[1;34m"
#define LBLUE "\033[0;34m"
#define BMAGENTA "\033[1;35m"
#define LMAGENTA "\033[0;35m"
#define BCYAN "\033[1;36m"
#define LCYAN "\033[0;36m"
#define DEFAULT "\033[0m"
#define PORT 8000
#define BUFSIZE1 1024
#define BUFSIZE2 30000

static char bar[] = "===================================================";
int main()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char server_buffer_file[BUFSIZE1]={0};
    char server_buffer_rece[BUFSIZE1]={0};
    char server_buffer_extr[BUFSIZE1]={0};
    char *server_buffer_data = (char *)calloc(BUFSIZE2, sizeof(char));
    char *msg = "";
    char *msg1 = "file_ready_to_transfer";
    char *msg2 = "no_file_ready_to_transfer";
    float percent=0;
    int bari=0;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    printf("%sServer is started and waiting for client...\n%s",LYELLOW,DEFAULT);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("%sConnected with client\n%s",BYELLOW,DEFAULT);


    while(1)
    {
        printf("%s\nServer>\n%s",BBLUE,DEFAULT);
        fflush(stdout);
        int run=0;
        int err=0;
        memset(server_buffer_extr, 0, sizeof server_buffer_extr);
        valread = read(new_socket , server_buffer_extr, 1024);  // read infromation received into the buffer
        if(strcmp(server_buffer_extr,"exit")==0) break;
        run=atoi(server_buffer_extr);
        send(new_socket ,server_buffer_extr, strlen(server_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM

        while (run)
        {
            err=0;
            memset(server_buffer_file, 0, sizeof server_buffer_file);
            valread = read(new_socket , server_buffer_file, 1024);  // read infromation received into the buffer    
            printf("%s->>Client's requested for [%s]\n%s",LMAGENTA,server_buffer_file,DEFAULT);
            printf("%s   Finding [%s]\n%s",LCYAN,server_buffer_file,DEFAULT);
            int cur_fd = open(server_buffer_file, O_RDONLY);
            if (cur_fd < 0)
            {
                printf("%s   File [%s] doesn't exist\n%s",LRED,server_buffer_file,DEFAULT);
                send(new_socket , msg2 , strlen(msg2) , 0 );  // use sendto() and recvfrom() for DGRAM
                // exit(EXIT_FAILURE);
                err=1;
            }
            else
            {
                send(new_socket , msg1 , strlen(msg1) , 0 );  // use sendto() and recvfrom() for DGRAM
                memset(server_buffer_rece, 0, sizeof server_buffer_rece);
                valread = read(new_socket , server_buffer_rece, 1024);  // read infromation received into the buffer
                ll file_size = lseek(cur_fd, 0, SEEK_END);
                if(strcmp("send",server_buffer_rece)==0)
                {
                    printf("%s   Start sending file [%s]\n%s",LGREEN,server_buffer_file,DEFAULT);
                    int o = file_size/BUFSIZE2;
                    int oj=0;
                    int p = file_size%BUFSIZE2;
                    if(p!=0)o++;
                    snprintf (server_buffer_extr, sizeof(server_buffer_extr), "%d",p);
                    send(new_socket , server_buffer_extr , strlen(server_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM
                    memset(server_buffer_rece, 0, sizeof server_buffer_rece);
                    valread = read(new_socket , server_buffer_rece, 1024);
                    if(atoi(server_buffer_rece)==p)
                    {
                        snprintf (server_buffer_extr, sizeof(server_buffer_extr), "%d",o);
                        send(new_socket , server_buffer_extr , strlen(server_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM
                        int buff=BUFSIZE2;
                        int size=file_size;
                        while (oj<o)
                        {
                            memset(server_buffer_rece, 0, sizeof server_buffer_rece);
                            valread = read(new_socket , server_buffer_rece, 1024);  // read infromation received into the buffer
                            if(oj==atoi(server_buffer_rece))
                            {
                            
                                ll fie = lseek(cur_fd, oj*buff , SEEK_SET);
                                if(file_size<buff)
                                {
                                    buff=file_size;
                                    char *server_buffer_ldata = (char *)calloc(buff, sizeof(char));
                                    memset(server_buffer_data, 0, sizeof server_buffer_ldata);
                                    int pika = read(cur_fd,server_buffer_ldata,buff);
                                    if(pika<0) {
                                        printf("%s   Cannot read from file [%s]\n%s",LRED,server_buffer_file,DEFAULT);
                                        err=1;
                                        break;
                                    }  
                                    send(new_socket , server_buffer_ldata , strlen(server_buffer_ldata) , 0 );  // use sendto() and recvfrom() for DGRAM
                                }
                                else
                                {
                                    memset(server_buffer_data, 0, sizeof server_buffer_data);
                                    int pika = read(cur_fd,server_buffer_data,buff);
                                    if(pika<0) {
                                        printf("%s   Cannot read from file [%s]\n%s",LRED,server_buffer_file,DEFAULT);
                                        err=1;
                                        break;
                                    }
                                    send(new_socket , server_buffer_data , strlen(server_buffer_data) , 0 );  // use sendto() and recvfrom() for DGRAM
                                }
                                oj++;
                                file_size-=buff;
                                percent = (float)(size-file_size)/(float)size*100;
                                bari = (int)(percent-0.1)/2.00;
                                printf("   %s%6.6f [%s>]\r%s",LYELLOW,percent,&bar[49-bari],DEFAULT);
                               
                            }
                        }
                        if(err==0)
                        {
                            printf("   %s100.000000 [%s=]\n%s",LYELLOW,&bar[0],DEFAULT);
                            printf("   %sTransfer of [%s] completed successfully\n%s",BGREEN,server_buffer_file,DEFAULT);
                        }
                        else
                        {
                            err=0;
                            printf("   %sTransfer of [%s] is unsuccessful\n%s",BRED,server_buffer_file,DEFAULT);
                        }
                        
                    }
                    else
                    {
                        printf("%s   ERROR : OOPS! something wrong in transmitting [%s]\n%s",LRED,server_buffer_file,DEFAULT);
                        err=1;
                    }
                    
                }
                else
                {
                    printf("%s   ERROR : OOPS! no comfirmation by client to send [%s]\n%s",LRED,server_buffer_file,DEFAULT);
                    err=1;
                }
                if(close(cur_fd)<0)
                {
                    perror("Closing file");
                }
            }
                if(err==1) printf("%s   Transfer of [%s] is unsuccessful\n%s",BRED,server_buffer_file,DEFAULT);

            run--;
        }
        printf("%sALL task(s) completed\n%s",BYELLOW,DEFAULT);
    }
    if(close(server_fd)<0)
    {
        perror("Disconnecting server");
    }
    else
    {
        printf("%sDisconnected with client(s)\n%s",BYELLOW,DEFAULT);
    }
    return 0;
}