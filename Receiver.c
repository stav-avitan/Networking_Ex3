#include <stdio.h>
#include "myqueue.h"
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>


#define SERVER_PORT 6000
#define FILE_SIZE 2136287
#define CHUNK_SIZE 1

//Function that print the report.
void printreport(int numofiterations)
{
    long int avgitcubic = 0;
    long int avgitreno = 0;
    long int avgittotal = 0;
    long int numofdequeue = 0;

    // Dequeue the queue and print out the report
    while (head != NULL)
    {
        if (*head->c0r1 == 0)
        {
            avgitcubic += *head->tms;
        }
        else if (*head->c0r1 == 1)
        {
            avgitreno += *head->tms;
        }
        avgittotal += *head->tms;
        dequeue();
        numofdequeue++;
    }

    printf("-----------------------\n");
    printf("-----------------------\n");
    printf("the average time for cubic is %ld \n", avgitcubic / numofiterations);
    printf("the average time for reno is %ld \n", avgitreno / numofiterations);
    printf("the average time for total is %ld \n", avgittotal / numofdequeue);
}


int main()
{
    //Calculation the XOR
    int id1 = 8859;
    int id2 = 5872;
    int XOR = id1 ^ id2;

    // Open the listening socket , using the IPv4 and TCP protocol
    int listeningsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningsocket == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    //Creating a memory space for the file that we will receive
    char *receiveSpace = (char *)calloc((FILE_SIZE / 2), sizeof(char));
    if (receiveSpace == NULL)
    {
        perror("Memory error\n");
        return -1;
    }

    //Creating sockaddr_in struct, reset it, and enter important values.
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    //INADDR_ANY - means any IP can connect.
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    //Bind the socket to the port with any IP at this port
    int bindResult = bind(listeningsocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        perror("Bind failed");
        close(listeningsocket);
        return -1;
    }

    //Make the socket listening, 50 is the size of queue connection requests
    int listenResult = listen(listeningsocket, 50);
    if (listenResult == -1)
    {
        perror("listen didn't work");
        close(listeningsocket);
        return -1;
    }
    printf("Waiting for incoming TCP-connections...\n");

    //Endless loop for incoming connections
    while(1)
    {

    //Build a struct for the client
    struct sockaddr_in clientAddress; //
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);

    //The receiver will accept requests.
    int client_socket = accept(listeningsocket, (struct sockaddr *)&serverAddress, &clientAddressLen);
    if (client_socket == -1)
    {
        perror("accept didn't work");
        close(listeningsocket);
       exit(1);
    }

    //The number of iterations
    int iterationnumber = 0;

    printf("A new client connection accepted\n");

    //Endless loop that allows the client to send us many requests.
    while (1)
    {
        //Setting the CC algorithm to cubic.
        if (setsockopt(listeningsocket, SOL_SOCKET, SO_REUSEADDR, "cubic", 5) == -1)
        {
            perror("setsockopt() failed");
            return -1;
        }

        //We set timeval struct for measuring the time of the first part
        struct timeval starttimecubic, endtimecubic, resultcubic;
        struct timeval starttimereno, endtimereno, resultreno;
        //Start count for the first part of the file
        gettimeofday(&starttimecubic, NULL);

        //Receive the first part of the file
        int bytescount = 0;
        int bytesreceived;

        //Sends file in chunks of 1 byte
        while (bytescount + CHUNK_SIZE <= FILE_SIZE / 2)
        {
            bytesreceived = recv(client_socket, receiveSpace, CHUNK_SIZE, 0);
            if (bytesreceived == -1)
            {
                perror("recv failed chunks");
                close(listeningsocket);
                close(client_socket);
                return -1;
            }
            bytescount += bytesreceived;
        }

        iterationnumber++;
        //Stop the count of the first part.
        gettimeofday(&endtimecubic, NULL);
        //Total time of cubic.
        timersub(&endtimecubic, &starttimecubic, &resultcubic);
        long int *timepassedcubic = (long int *)malloc(sizeof(long int));
        //The time is in ms so we multiply by 1000000.
        *timepassedcubic = resultcubic.tv_sec * 1000000 + resultcubic.tv_usec;
        //The iteration number.
        int *iterationnumberp = (int *)malloc(sizeof(int));
        *iterationnumberp = iterationnumber;
        int *cubicparam = (int *)malloc(sizeof(int));
        *cubicparam = 0;

        //We are adding the time to our queue.
        enqueue(timepassedcubic, iterationnumberp, cubicparam);
        printf("Algorithm: cubic, time has taken:%ld.%07ld, iteration number:%d\n",
               (long int)resultcubic.tv_sec,
               (long int)resultcubic.tv_usec,
               iterationnumber);

        //Sending the XOR to the client
        int num_message = XOR;
        if (send(client_socket, &num_message, sizeof(num_message), 0) == -1)
        {
            perror("send() failed");
            close(client_socket);
            close(listeningsocket);
            return -1;
        }

        //We alter the CC algorithm to reno.
        if (setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, "reno", 4) == -1)
        {
            perror("setsockopt() failed");
            return -1;
        }

        //Starting the time for the second part.
        gettimeofday(&starttimereno, NULL);
        //Receive the second part of the file
        bytescount = 0;
        //Sends file in chunks of 1 byte
        while (bytescount + CHUNK_SIZE <= FILE_SIZE / 2)
        {
            bytesreceived = recv(client_socket, receiveSpace, CHUNK_SIZE, 0);
            if (bytesreceived == -1)
            {
                perror("recv failed chunks");
                close(listeningsocket);
                close(client_socket);
                return -1;
            }
            bytescount += bytesreceived;
        }

        //Stop the count for the second part.
        gettimeofday(&endtimereno, NULL);
        //Total time for reno.
        timersub(&endtimereno, &starttimereno, &resultreno);
        printf("Algorithm: reno, time:%ld.%07ld, iteration number:%d\n", (long int)resultreno.tv_sec, (long int)resultreno.tv_usec, iterationnumber);
        //The time is in ms, so we multiply by 1000000.
        long int timepassedreno = resultreno.tv_sec * 1000000 + resultreno.tv_usec;
        long int *timepassedrenop = (long int *)malloc(sizeof(long int));
        *timepassedrenop = timepassedreno;
        int *renoparam = (int *)malloc(sizeof(int));
        *renoparam = 1;

        //We are adding the time to our queue.
        enqueue(timepassedrenop, iterationnumberp, renoparam);



        // Asking the client if he wants to send the file again.
        char ans;
        printf("%d", iterationnumber);
        recv(client_socket, &ans, sizeof(char), 0);
        //If not than print the report
        if (ans == 'N')
        {
            printf("---------------------------------------\n");
            printf("The report is:\n");
            printreport(iterationnumber);
            printf("Closing client and server socket\n");
            close(client_socket);
            close(listeningsocket);
            printf("bye friends ! we will miss you until the next time \n");
            printf(" - Stav & Avichi  XDXDXD - \n");
            exit(1);
        }
        else
        {
            continue;
        }

       }
        free(receiveSpace);
    }
}