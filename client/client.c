#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        perror("wrong number of arguments");
        return -1;
    }

    // create a socket for the client to connect to the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket error");
        return -1;
    }

    // set the server address and port
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        return -1;
    }

    // connect to the server
    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect fail");
        return -1;
    }

    while (1)
    {
        int m;
        printf("Enter a number (or type q to quit): ");
        char input[10];
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("fgets error");
            break;
        }
        if (input[0] == 'q')
        {
            break;
        }
        m = atoi(input);

        ssize_t n = send(server_fd, &m, sizeof(m), 0);
        if (n < 0)
        {
            perror("send error");
            break;
        }

        // receive result from the server and print it
        int result;
        n = recv(server_fd, &result, sizeof(result), 0);
        if (n < 0)
        {
            perror("recv error");
            break;
        }
        if (n == 0)
        {
            printf("Connection closed by server\n");
            break;
        }

        printf("Result: %d\n", result);
    }

    close(server_fd);
    return 0;
}
