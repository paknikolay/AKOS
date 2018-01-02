#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#define PORT 5555
#define  BUFFERSIZE 1024
char message[] = "Hello there!\n";
char buf[BUFFERSIZE];

int main()
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    printf("got data %d\n", read(sock, buf, BUFFERSIZE));
    int height = buf[1];
    int n = buf[0];
    printf("data size: %d, height: %d \n", n, height);

    char localField[BUFFERSIZE];
    char localNextField[BUFFERSIZE];



    while(true) {
        bzero(localField, BUFFERSIZE);
        printf("got field %d\n", read(sock, localField, BUFFERSIZE));

        bzero(localNextField, BUFFERSIZE);

        for (int i = 1; i < height - 1; ++i) {
            for (int j = 0; j < n; ++j) {
                printf("%d ", localField[i * n + j]);
                int amountOfAlive = 0;
                for (int q = -1; q <= 1; ++q)
                    for (int w = -1; w <= 1; ++w) {
                        if (q != 0 || w != 0) {
                            if (localField[(i + q) * n + (j + w + n) % n] == 1)
                                ++amountOfAlive;
                        }
                    }
                //printf("(%d , %d) ",  );
                if (localField[i * n + j] == 1) {
                    if (amountOfAlive < 2 || amountOfAlive > 3)
                        localNextField[i * n + j] = 0;
                    else
                        localNextField[i * n + j] = 1;
                } else {
                    if (amountOfAlive == 3)
                        localNextField[i * n + j] = 1;
                    else
                        localNextField[i * n + j] = 0;
                }

            }
            printf("\n");
        }
        printf("\n");

        printf("field sent %d\n", write(sock, localNextField, BUFFERSIZE));


        for (int i = 1; i < height-1; ++i) {
            for (int j = 0; j < n; ++j) {
                printf("%d ", localNextField[i * n + j]);
            }
            printf("\n");
        }

    }

    close(sock);

    return 0;
}