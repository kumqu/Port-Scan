#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char** argv)
{
    char *ip;
    int start_port,end_port;
    int sockfd;
    struct sockaddr_in to;
    int ret, i;
    struct servent *server_name;

    if(argc != 4)
    {
        printf("Usage: ./scan ip start_port end_port\n");
        exit(-1);
    }
    ip = argv[1];
    start_port = atoi(argv[2]);
    end_port = atoi(argv[3]);



    if(start_port < 1 || end_port > 65535 || start_port > end_port)
    {
        printf("Input port error!\n");
        exit(-1);
    }

    for (i = start_port; i <= end_port; i++)
    {
        memset(&to,0,sizeof(struct sockaddr_in));
        to.sin_family = AF_INET;
        to.sin_addr.s_addr = inet_addr(ip);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        to.sin_port = htons(i);


        if (0>(ret=connect(sockfd,(struct sockaddr*)&to,sizeof(struct sockaddr))))
		{
		    close(sockfd);
		    continue;
		}

        if((server_name = getservbyport(htons(i),NULL))== 0)
        {
            printf("%d  %s\n",i, "Unknown");
            close(sockfd);
            continue;
        }

        printf("%d  %s\n",i, server_name->s_name);

        close(sockfd);

    }

    return 0;
}
