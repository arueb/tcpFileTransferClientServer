// ---------------------------------------------------------------------------//
// Programmer Name:   Alex Rueb
// Program Desc:      A simple chat client that utilizes TCP sockets to
//                    communicate with associated chatserve.py server.
// Course Name:       CS372_400_SU19
// Last Modified:     7/28/2019
//
// Sources Referenced (see citations in code)
// 1) https://beej.us/guide/bgnet/pdf/bgnet_USLetter.pdf
// 2) https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
// 3) https://www.codingame.com/playgrounds/14213/how-to-play-with-strings-in-c/string-concatenation
// 4) https://stackoverflow.com/questions/19472546/implicit-declaration-of-function-close
// 5) https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
// 6) https://www.geeksforgeeks.org/escape-sequences-c/
// 7) https://www.geeksforgeeks.org/clearing-the-input-buffer-in-cc/
// ---------------------------------------------------------------------------//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> // for close() function (cite reference #4)


// ---------------------------------------------------------------------------//
// Function: newAddress()
//
// Desc:  Creates and returns an addrinfo struct containing address information
//        to be used in connecting the socket.
//
// Args:  host (char *) - hostname of server
//        port (int) - port number of server
//
// Pre:   host and port char arrays most be supplied as command line arguments
//
// Post:  Returns an struct addrinfo containing address information from the
//        supplied host and port arguments.
// ---------------------------------------------------------------------------//
struct addrinfo* newAddress (char *host, char *port){
    int status; // for error checking in getaddrinfo() return
    struct addrinfo hints; // used to configure address
    struct addrinfo *res;  // holds resulting addrinfo struct

    // configure hints (cite reference #1)
    memset(&hints, 0, sizeof hints);    // clear memory
    hints.ai_family = AF_INET;          // specify ip protocol version
    hints.ai_socktype = SOCK_STREAM;    // specify socket type

    // create the address struct (cite reference #1)
    if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }
    return res;
}


// ---------------------------------------------------------------------------//
// Function: newSocket()
//
// Desc:  Creates and connects a new socket from a supplied pointer to a struct
//        addrinfo and returns a socket descriptor (int) to be used in several
//        other functions
//
// Args:  address (struct addrinfo *) - contains address information created in
//        the newAddress() function
//
// Pre:   must have already created the address argument
//
// Post:  Returns a socket descriptor or exits if an error is detected
// ---------------------------------------------------------------------------//
int createSocket(struct addrinfo *address){
    // create a new socket
    int sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (sockfd == -1){
        perror("Socket creation error");
        exit(1);
    }

    // connect the socket
    if (connect(sockfd, address->ai_addr, address->ai_addrlen) == -1) {
        close(sockfd);
        perror("Connecting error");
        exit(1);
    }

    return sockfd;
}


// ---------------------------------------------------------------------------//
// Func:  getUserName()
//
// Desc:  Asks the user for their name and stores it in a newly created buffer
//        that is returned.
//
// Args:  maxLength (char *) - the maximum name characters allowed
//
// Pre:   None
//
// Post:  Returns a pointer to a char array containing the user's name. The
//        user's handle will be truncated if they enter more than the max length.
//        The stdin buffer is cleared of any characters that exceed the limit.
// ---------------------------------------------------------------------------//
char* getUserName(int maxLength){
    char *buffer = malloc (sizeof (char) * (maxLength)); // to store name

    // prompt user and get the message
    printf("Please enter your name (%d charachters max):\n", maxLength);
    fgets(buffer,maxLength + 1,stdin);

    // strip off newline or return char
    buffer[strcspn(buffer, "\r\n")] = 0; // (cite reference #2)

    // clear the stdin buffer if needed (cite reference #7)
    if (strlen(buffer) == maxLength) while ((getchar()) != '\n');
    return buffer;
}


// ---------------------------------------------------------------------------//
// Func:  getMsg()
//
// Desc:  Creates a messsage from user input and prepends it with the sender's
//        handle (ie: name> my message).  If the user enters \quit, a return
//        value of -1 is returned to signal exit of loop in beginChat
//
// Args:  rcvBuffer (char *) - buffer allocated for incoming message
//        sockfd (int) - socket file descriptor
//        maxName (int) - max chars in sender's handle
//        maxMsg (int) - maximum chars in message
//        bytes_rcvd (int) - number of bytes received, used for error handling
//
// Pre:   Socket connection must be established with server.
//
// Post:  received messsage is displayed
// ---------------------------------------------------------------------------//
int getMsg(int maxLength, char *senderName, char *buffer){
    char msg[maxLength]; // to hold message
    fgets(msg,maxLength + 1,stdin); // get the message
    msg[strcspn(msg, "\r\n")] = 0; //strip off the return char (cite reference #2)

    // clear the stdin buffer if needed (cite reference #7)
    if (strlen(msg) == maxLength) while ((getchar()) != '\n');

    if (strcmp(msg,"\\quit") == 0){ //cite reference #6
        printf("Goodbye!\n");
        return - 1; // signal to break chat loop
    }

    // concatenate strings to create full message (cite reference #3)
    strcat(buffer,senderName);
    strcat(buffer,"> ");
    strcat(buffer,msg);
    return 0;
}


// ---------------------------------------------------------------------------//
// Func:  receiveMsg()
//
// Desc:  Receives an incoming message from the chat server
//
// Args:  rcvBuffer (char *) - buffer allocated for incoming message
//        sockfd (int) - socket file descriptor
//        maxName (int) - max chars in sender's handle
//        maxMsg (int) - maximum chars in message
//        bytes_rcvd (int) - number of bytes received, used for error handling
//
// Pre:   Socket connection must be established with server.
//
// Post:  received messsage is displayed
// ---------------------------------------------------------------------------//
void receiveMsg(char * rcvBuffer, int sockfd, int maxName, int maxMsg, int bytes_rcvd){
  memset(rcvBuffer, 0, (maxName + maxMsg + 2)); // clear the buffer

  // receive incoming message
  bytes_rcvd = recv(sockfd, rcvBuffer, maxName + maxMsg + 3, 0);
  // if (strlen(rcvBuffer) == (maxMsg + maxName + 2)) while ((getchar()) != '\n');

  // check for error conditions
  if ( bytes_rcvd== -1) {
      perror("Error receiving data");
      free(rcvBuffer);
      exit(1);
  } else if (bytes_rcvd == 0){
      printf("The connection was terminated by the server.\n");
      exit(0);
  }
  printf("%s\n", rcvBuffer); // display the message
  return;
}



// ---------------------------------------------------------------------------//
// Func:  sendMsg()
//
// Desc:  Gets a message from the user and sends it to the chat server
//
// Args:  rcvBuffer (char *) - buffer allocated for incoming message
//        sockfd (int) - socket file descriptor
//        maxName (int) - max chars in sender's handle
//        maxMsg (int) - maximum chars in message
//        bytes_rcvd (int) - number of bytes received, used for error handling
//
// Pre:   Socket connection must be established with server.
//
// Post:  If user enters '\quit', a return value of -1 signals condition
//        to exit loop in beginChat() function so allocated memory can be freed
// ---------------------------------------------------------------------------//
int sendMsg(char *senderName, char *sendBuffer, int sockfd, int maxName, int maxMsg, int bytes_sent){
    memset(sendBuffer, 0, (strlen(senderName) + maxMsg + 2)); // clear the buffer
    printf("%s> ", senderName); // print handle prompt

    // gets the message contents from user
    if (getMsg(maxMsg, senderName, sendBuffer) == -1){ // check if user entered \quit
        return -1; // signal to exit loop in beginChat()
    }
    // send the message
    bytes_sent = send(sockfd, sendBuffer, strlen(sendBuffer), 0);

    // check for errors
    if (bytes_sent== -1){
        perror("Error sending data");
        free(sendBuffer);
        exit(1);
    }
    return 0;
}


// ---------------------------------------------------------------------------//
// Function: beginChat()
//
// Desc:  Initiates loop of send/receive sequence.
//
// Args:  senderName (char *) - sender's handle
//        sockfd (int) - socket file descriptor
//        maxName (int) - max chars in sender's handle
//        maxMsg (int) - maximum chars in message
//
// Pre:   Socket connection must be established with server.
//
// Post:  memory allocated sendBuffer and rcvBuffer are freed
// ---------------------------------------------------------------------------//
void beginChat(char *senderName, int sockfd, int maxName, int maxMsg){
    int bytes_sent, bytes_rcvd;
    char *sendBuffer = malloc(sizeof(char) * (strlen(senderName) + maxMsg + 2));
    char *rcvBuffer = malloc(sizeof(char) * (maxName + maxMsg + 2));

    while(1){
        // get message from user and sent it to the server
        if (sendMsg(senderName, sendBuffer, sockfd, maxName, maxMsg, bytes_sent) == -1){
          break;
        }

        // receive incoming message from the server and display it
        receiveMsg(rcvBuffer, sockfd, maxName, maxMsg, bytes_rcvd);
    }

    free(sendBuffer);
    free(rcvBuffer);
}


// ---------------------------------------------------------------------------//
// Function: main()
//
// Desc:  This is the primary function that controls the program flow and
//        makes calls to the other functions.
//
// Args:  argc (int) - number of command line arguments
//        argv (char *) - array of command line arguments
//
// Pre:   Host and port must be supplied as command line arguments
//
// Post:  Program execution completed.
// ---------------------------------------------------------------------------//
int main(int argc, char *argv[]){
    #define MAX_CHAT_CHARS 500 // max message length
    #define MAX_NAME_CHARS 10 // max username length

    // check for proper number of command line arguments (cite reference #5)
    if (argc != 3){
      printf("Please supply the required command line arguments.\n");
      printf("Usage: ./chatclient [host] [port]\n");
      exit(0);
    }

    struct addrinfo *address = newAddress(argv[1], argv[2]); // create address
    int sockfd = createSocket(address); // create and connect socket

    char *senderName = getUserName(MAX_NAME_CHARS); // get user name
    beginChat(senderName, sockfd, MAX_NAME_CHARS, MAX_CHAT_CHARS); //start chat

    // clean up
    close(sockfd);
    freeaddrinfo(address);
    free(senderName);
    printf("The server connection has been terminated. Goodbye!\n");
}
