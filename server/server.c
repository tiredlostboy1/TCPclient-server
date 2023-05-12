#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>

#define CONNECTIONS_N 8

// checking if the given number is prime
bool is_prime(int num)
{
    if (num <= 1)
    {
        return false;
    }

    for (int i = 2; i <= num / 2; ++i)
    {
        if (num % i == 0)
        {
            return false;
        }
    }
    return true;
}

// computing the m_th prime number
int mth_prime(int m)
{
    int counter = 0;
    int num = 2;
    while (counter < m)
    {
        if (is_prime(num))
        {
            counter++;
        }
        num++;
    }
    return num - 1;
}

// handling each client, which will be executed in a seperate thread
void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);

    while(1){
    // recieving input from client
    int m;
    ssize_t n = recv(client_fd, &m, sizeof(m), 0);
    if (n < 0)
    {
        perror("recv error");
        close(client_fd);
        return NULL;
    }

    // computing the result
    int result = mth_prime(m);

    // sending the result back to the client
    n = send(client_fd, &result, sizeof(result), 0);
    if (n < 0)
    {
        perror("send error");
    }
    }

    // closing connection
    close(client_fd);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("wrong number of arguments");
        return -1;
    }

    // creating socket fd for server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket creating error");
        return -1;
    }

    // SO_REUSEADDR option allows binding to a port TIME_WAIT state.
    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        perror("setsockopt");
        return -1;
    }

    // struct with server's IP address, port number
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind error");
        return -1;
    }

    if (listen(server_fd, CONNECTIONS_N) < 0)
    {
        perror("listen error");
        return -1;
    }

    while (1)
    {
        struct sockaddr_in client_addr = {0};
        socklen_t client_len = sizeof(client_addr);

        // Accept the incoming client connection & create fd
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        // Create a new thread to handle the  connection
        pthread_t tid;
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL)
        {
            perror("Failed to allocate memory");
            close(client_fd);
            continue;
        }

        *arg = client_fd;

        if (pthread_create(&tid, NULL, handle_client, arg) != 0)
        {
            perror("Failed to create thread for client");
            free(arg);
            close(client_fd);
            continue;
        }
        pthread_detach(tid);
    }
    return 0;
}