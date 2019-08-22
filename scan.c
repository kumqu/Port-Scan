#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define THREAD_NUM 100
#define SEG_LEN 655
#define MAX_PORT 65535

typedef struct port_segment {
    char *ip;
    unsigned int min_port;
    unsigned int max_port;
} port_segment;

void *scan(void *arg);


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Usage: ./scan ip1 ip2 ... \n");
        exit(-1);
    }


    for ( int j = 0; j < argc - 1; ++j )
    {
        pthread_t * thread;
        thread = ( pthread_t * )malloc( THREAD_NUM * sizeof(pthread_t) );

        for ( int i = 0; i < THREAD_NUM; ++i )
        {
            port_segment port;

            port.ip = argv[j+1];

            port.min_port = i * SEG_LEN + 1;

            /* the last segment */
            if ( i == (THREAD_NUM - 1) )
                port.max_port = MAX_PORT;
            else
                port.max_port = port.min_port + SEG_LEN - 1;


            if ( pthread_create(&thread[i], NULL, scan, (void *)&port) != 0 )
            {
                perror( "pthread_create failed\n" );
                free(thread);
                exit(-2);
            }

            if(pthread_join(thread[i], NULL) != 0)
            {
                perror("pthread_join error");
                exit(-1);
            }
        }

        free(thread);
    }

    return 0;
}


void *scan(void *arg)
{
    int sockfd;
    struct sockaddr_in to;
    int ret;
    struct servent *server_name;
    port_segment port;

    memcpy( &port, arg, sizeof(struct port_segment) );

    memset(&to,0,sizeof(struct sockaddr_in));
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = inet_addr(port.ip);


    for (int i = port.min_port; i <= port.max_port; i++)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        to.sin_port = htons(i);

        if (0>(ret=connect(sockfd,(struct sockaddr*)&to,sizeof(struct sockaddr))))
		{
		    close(sockfd);
		    continue;
		}

        if((server_name = getservbyport(htons(i),NULL))== 0)
        {
            printf("%s %d  %s\n", port.ip, i, "Unknown");
            close(sockfd);
            continue;
        }

        printf("%s %d  %s\n", port.ip, i, server_name->s_name);

        close(sockfd);

    }

    return 0;
}


