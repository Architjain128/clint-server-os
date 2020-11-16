#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
char a[400000];
char argv[2000][200];
static char bar[] = "===================================================";

void help()
{
    printf("%shelp                   to know about commands\n%s",BCYAN,DEFAULT); 
    printf("%sexit                   to exit client CLI\n%s",BCYAN,DEFAULT); 
    printf("%sget <file1>            to send request for a file with name file1\n%s",BCYAN,DEFAULT); 
    printf("%sget <file1> <file2>    to send request for multiple files by entering multiples name seprated by space\n\n%s",BCYAN,DEFAULT); 
}
int main()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char client_buffer_file[BUFSIZE1]={0};
    char client_buffer_rece[BUFSIZE1]={0};
    char client_buffer_extr[BUFSIZE1]={0};
    char *client_buffer_data = (char *)calloc(BUFSIZE2, sizeof(char));
    char client_buffer_mssg[BUFSIZE1]={0};
    char *msg = "";
    char *msg1 = "file_ready_to_transfer";
    char *msg2 = "no_file_ready_to_transfer";
    float percent=0;
    int bari=0;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error ");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        perror("Invalid address or Address not supported ");
        return -1;
    }

    printf("%sClient is started and trying to connect with server...\n%s",LYELLOW,DEFAULT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        perror("Connection Failed");
        return -1;
    }
    printf("%sClient connected to server\n\n%s",BYELLOW,DEFAULT);
    help();
    while (1)
    {
        printf("%sClient>%s ",BBLUE,DEFAULT);
        memset(client_buffer_mssg, 0, sizeof client_buffer_mssg);
        memset(client_buffer_file, 0, sizeof client_buffer_file);
        memset(client_buffer_rece, 0, sizeof client_buffer_rece);
        memset(client_buffer_extr, 0, sizeof client_buffer_extr);
        memset(client_buffer_data, 0, sizeof client_buffer_data);
        memset(a, 0, sizeof a);
        memset(argv[0], 0, sizeof argv[0]);
    
        fgets(a,sizeof(a),stdin);
        a[strlen(a)-1]='\0';

        ll argc=0; 
        char* cmdd =(char *)malloc(1024*(sizeof(char)));
        cmdd = strtok(a, " \t\0");
        while( cmdd != NULL ) 
        {
            strcpy(argv[argc],cmdd);
            argc++;
            cmdd = strtok(NULL, " \t\0");
        }
        if(argv[0][0]=='e' && argv[0][1]=='x' && argv[0][2]=='i' && argv[0][3]=='t' && strlen(argv[0])==4)
        {
            if(argc>1)
            printf("%sWARNING : no flag(s) exists : executing default tasks as of <exit> only\n%s",BYELLOW,DEFAULT); 
            send(sock , "exit" , strlen("exit") , 0 );  // use sendto() and recvfrom() for DGRAM  // send the message.
            break;
        }
        else if(argv[0][0]=='h' && argv[0][1]=='e' && argv[0][2]=='l' && argv[0][3]=='p' && strlen(argv[0])==4)
        {
            if(argc>1)
            printf("%sWARNING : no flag(s) exists : executing default tasks as of <help> only\n%s",BYELLOW,DEFAULT); 
            help();
        }
        else if(argv[0][0]=='g' && argv[0][1]=='e' && argv[0][2]=='t' && strlen(argv[0])==3)
        {
           
            if(argc==1)
            printf("%sERROR : no file is selected to tranfer%s\n",BRED,DEFAULT);
            else
            {
                ll file_to_transfer = argc-1;
                memset(client_buffer_extr, 0, sizeof client_buffer_extr);
                snprintf (client_buffer_extr, sizeof(client_buffer_extr), "%lld",file_to_transfer);
                send(sock , client_buffer_extr , strlen(client_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM  // send the message.
                memset(client_buffer_mssg, 0, sizeof client_buffer_mssg);  
                valread = read( sock , client_buffer_mssg, 1024);
                if(strcmp(client_buffer_mssg,client_buffer_extr)==0)
                {
                    ll cur_file=0;
                    int err=0;
                    while(cur_file<file_to_transfer)
                    {    
                        err=0;
                        strcpy(client_buffer_file,argv[1+cur_file]);
                        printf("%s->>Request for transfering [%s] sent by client\n%s",LMAGENTA,client_buffer_file,DEFAULT);
                        send(sock , client_buffer_file , strlen(client_buffer_file) , 0 );  // send the message.
                        memset(client_buffer_mssg, 0, sizeof client_buffer_mssg);  
                        valread = read( sock , client_buffer_mssg, 1024);  // receive message back from server, into the buffer
                                    
                        if(strcmp(client_buffer_mssg,msg2)==0)
                        {
                            printf("%s   No file named [%s] exists in server\n%s",LRED,client_buffer_file,DEFAULT);
                            err=1;
                        }
                        else if(strcmp(client_buffer_mssg,msg1)==0)
                        {
                            printf("%s   File named [%s] is ready for transfer\n%s",LCYAN,client_buffer_file,DEFAULT);
                            if (remove(client_buffer_file) == 0) printf("%s   Deleted [%s] which is already present\n%s",LBLUE,client_buffer_file,DEFAULT); 
                                
                            int cur_fd = open(client_buffer_file ,O_CREAT | O_APPEND | O_WRONLY , 0644);
                            if (cur_fd < 0)
                            {
                                printf("%s   Cannot create newfile named as [%s]\n%s",BRED,client_buffer_file,DEFAULT);
                                err=1;
                            }
                            else
                            {
                                send(sock ,"send", strlen("send") , 0 );  // send the message.
                                memset(client_buffer_rece, 0, sizeof client_buffer_rece);
                                valread = read( sock , client_buffer_rece, BUFSIZE2);  // receive message back from server, into the buffer
                                int p=atoi(client_buffer_rece);
                                send(sock ,client_buffer_rece, strlen(client_buffer_rece) , 0 );  // send the message.
                                memset(client_buffer_rece, 0, sizeof client_buffer_rece);
                                valread = read( sock , client_buffer_rece, BUFSIZE2);  // receive message back from server, into the buffer
                                int o = atoi(client_buffer_rece);
                                int oh=0;
                                ll size=o*BUFSIZE2;
                                if(p!=0)
                                {
                                    size-=BUFSIZE2;
                                    size+=p;
                                }
                                ll temp_size=0;
                                printf("%s   Start receiving file [%s]\n%s",LGREEN,client_buffer_file,DEFAULT);
                                while(oh<o)
                                {
                                    if(oh+1<o)
                                    {
                                        memset(client_buffer_extr, 0, sizeof client_buffer_extr);
                                        snprintf (client_buffer_extr, sizeof(client_buffer_extr), "%d",oh);
                                        send(sock , client_buffer_extr , strlen(client_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM  // send the message.
                                        memset(client_buffer_data, 0, sizeof client_buffer_data);
                                        valread = read( sock , client_buffer_data, BUFSIZE2);  // receive message back from server, into the buffer
                                        write(cur_fd,client_buffer_data,strlen(client_buffer_data));
                                        oh++;
                                        temp_size+=BUFSIZE2;
                                    }
                                    else if(oh+1==o)
                                    {
                                        char *client_buffer_ldata = (char *)calloc(p, sizeof(char));
                                        memset(client_buffer_extr, 0, sizeof client_buffer_extr);
                                        snprintf (client_buffer_extr, sizeof(client_buffer_extr), "%d",oh);
                                        send(sock , client_buffer_extr , strlen(client_buffer_extr) , 0 );  // use sendto() and recvfrom() for DGRAM  // send the message.
                                        memset(client_buffer_ldata, 0, sizeof client_buffer_ldata);
                                        valread = read( sock , client_buffer_ldata, p);  // receive message back from server, into the buffer
                                        write(cur_fd,client_buffer_ldata,strlen(client_buffer_ldata));
                                        oh++;
                                        temp_size+=p;
                                    }
                                    percent = (float)(temp_size)/(float)size*100;
                                    bari = (int)(percent-0.1)/2.00;
                                    printf("   %s%6.6f [%s>]\r%s",LYELLOW,percent,&bar[49-bari],DEFAULT);
                                }
                                if(err==0)
                                {
                                    printf("   %s100.000000 [%s=]\n%s",LYELLOW,&bar[0],DEFAULT);
                                    printf("   %sTransfer of [%s] completed successfully\n%s",BGREEN,client_buffer_file,DEFAULT);
                                }
                                else
                                {
                                    printf("   %sTransfer of [%s] is unsuccessful\n%s",BRED,client_buffer_file,DEFAULT);
                                    err=0;
                                }
                            }
                            if(close(cur_fd)<0)
                            {
                                perror("Closing file");
                            }
                        }
                        // else
                        // {
                        //     printf("[%s]\n",client_buffer_mssg);
                        // }    
                        if(err==1)
                        {
                            printf("%s   Transfer of [%s] is unsuccessful\n%s",BRED,client_buffer_file,DEFAULT);
                            err=0;
                        }
                        cur_file++;
                    }
                }
                else
                {
                    printf("%sERROR : BLUNDER : Acknowlegement is incorrect%s\n",BRED,DEFAULT);
                }
                printf("%sALL task(s) completed\n\n%s",BYELLOW,DEFAULT);
            }
        }
        else
        {
            if(strlen(argv[0])!=0)
            {
                printf("%sERROR : no such command found%s\n",BRED,DEFAULT);
                help();
            }
        }    
    }

    if(close(sock)<0)
    {
        perror("Closing file");
    }
    else
    {
        printf("%sClient disconnected to server\n%s",BYELLOW,DEFAULT);    
    }
    
    return 0;
}