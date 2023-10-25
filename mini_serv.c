# include   <sys/socket.h>
# include   <netinet/in.h>
# include    <sys/select.h> 

# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <stdio.h>

void    ft_pustr(char *str)
{
    int i = 0;

    while (str[i])
    {
        write(1, &str[i], 1);
        i++;
    }
}

void    sendAll(int stop, const char *buf, int id, int *clients)
{
    for (int i = 0; i <= stop; i++)
    {
        if (clients[i] != id)
           send(i, buf, strlen(buf), 0);
    } 
}

struct client
{
    int fd;
    int id;
};

int start(int serSocket)
{
    int nfds = serSocket;
    int clientFds[1024] = {0};
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serSocket, &readfds);

    char    msg[1024];
    int     id = 0;
    while (1)
    { 
        fd_set tmp;
        FD_ZERO(&tmp);
        tmp = readfds;
        int retValue = select(nfds + 1, &tmp, NULL, NULL, NULL);
        if (retValue == -1)
        {
            write(2, "Fatal error\n", 13);
            exit(1);
        }
        for (int i = 0; i < nfds + 1; i++)
        {
            if (FD_ISSET(i, &tmp))
            {
                if (i == serSocket)
                {
                    int connect = accept(serSocket, NULL, NULL);
                    if (connect == -1)
                    {
                        write(2, "Fatal error\n", 13);
                        exit(1);
                    }
                    nfds = nfds > connect ? nfds : connect; 
                    clientFds[connect] = id;
                    FD_SET(connect, &readfds);
                    bzero(msg, sizeof(msg));
                    sprintf(msg, "server: client %d just arrived\n", id);
                    sendAll(nfds, msg, id, clientFds);
                    id++;
                }
                else 
                {
                    char buf[1024]; 
                    bzero(buf, sizeof(buf));
                    int rec = recv(i, buf, sizeof(buf), 0);
                    if (rec == 0)
                    {
                        bzero(buf, sizeof(buf));
                        sprintf(buf, "%d disconnected\n", clientFds[i]);
                        ft_pustr(buf);
                        FD_CLR(i, &readfds);
                        close(i);
                    }
                    else
                    {
                        send(i, buf, strlen(buf), 0);
                    }
                }
            }
        }
        
    }
    return 0;
}

int    createServer(int port)
{
    int sock;
    struct sockaddr_in address;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        write(2, "Fatal error\n", 13);
        exit(1);
    }
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(2130706433);

    if (bind(sock, (const struct sockaddr *)&address, sizeof(address)))
    {
        write(2, "Fatal error\n", 13);
        exit(1);
    }
     if (listen(sock, 1024) == -1)
    {
        write(2, "Fatal error\n", 13);
        exit(1);
    }
    if (start(sock) == -1)
        return 1;

    return sock;
}


int main(int ac, char **av)
{
    int socket;

    if (ac != 2)
    {
        write(2, "Wrong number of arguments\n", 27);
        exit(1);
    }
    // listen to socket
   
    // start accepting connections

    // create server;
    if ((socket = createServer(atoi(av[1]))) == -1)
        return 1;
    return 0;
}
