#!/usr/bin/python3

# ---------------------------------------------------------------------------- #
# Programmer Name:   Alex Rueb
# Program Desc:      A simple chat sever that utilizes TCP sockets to
#                    communicate with associated chatclient.c client.
# Course Name:       CS372_400_SU19
# Last Modified:     7/28/2019
#
# Reference Sources
# 1) https://docs.python.org/3/library/socket.html
# 2) https://pymotw.com/2/socket/tcp.html
# 3) https://stackoverflow.com/questions/40335581/how-do-i-remove-the-b-prefix-when-my-message-is-printed
# 4) https://bytes.com/topic/python/answers/512529-how-know-if-socket-still-connected
# 5) https://www.youtube.com/watch?v=Lbfe3-v7yE0&t=123s

# ---------------------------------------------------------------------------- #

import socket
import sys

# ---------------------------------------------------------------------------- #
# Function: openSocket()
#
# Desc: Creates a new socket for the supplied host and port number and listens
#       for incoming connections from chat clients.
#
# Args: host (string) - the server's hostname or ip address
#       port (int) - the server's port number
#
# Pre:  None
#
# Post: A new socket is opened and a socket descriptor is returned to be used
#       for other functions
# ---------------------------------------------------------------------------- #
def openSocket(host, port):
    # cite references #1 and #5
    serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # create new socket
    server_address = (str(host), int(port)) # create address object
    serverSocket.bind(server_address) # open th socket at address
    serverSocket.listen(5) # listen for up to 5 connections
    print(f"Listening for chat clients on {server_address}")

    return serverSocket # return socket file descriptor


# ---------------------------------------------------------------------------- #
# Function: getUserName()
#
# Desc: Prompts user to enter their name to be used as a chat handle up to the
#       specefied maximum number of characters.  Uses input validation to ensure
#       characters are within the accepted range (1-maxLength)
#
# Args: maxLength (int) - maximum number of name charachters
#
# Pre:  None
#
# Post: User's name is returned
# ---------------------------------------------------------------------------- #
def getUserName(maxLength):
    while True:
        # prompt user and get input
        userName = input(f"Please enter your name ({maxLength} chars max): ")

        # validate to ensure within max specefied length
        if len(userName) not in range(1,maxLength+1):
            print("Please enter a valid user name.")
        else:
            return userName


# ---------------------------------------------------------------------------- #
# Func: receiveMsg()
#
# Desc: Listens for incoming messages and displays them.  Detects if the client
#       has terminated the connection and displays a message to the user.
#       Returns -1 if the connection is terminated by client.
#
# Args: clientSocket - file descriptor
#       maxMsg - maximum message charachters that can be received
#
# Pre:  A connection with a client must be established
#
# Post: incoming message displayed.  -1 is signaled to exit loop in beginChat()
#       if connection terminated by client.
# ---------------------------------------------------------------------------- #
def receiveMsg(clientSocket, maxMsg):
    inMsg = clientSocket.recv(maxMsg)   # Receive message (cite reference #1)
    if len(inMsg.decode()) == 0: # cite reference #4
        print("The connection was terminated by the client.")
        return -1
    print(inMsg.decode()) # cite reference #3


# ---------------------------------------------------------------------------- #
# Func: sendMsg()
#
# Desc: Listens for incoming messages and displays them.  Detects if the client
#       has terminated the connection and displays a message to the user.
#       Returns -1 if the connection is terminated by client.
#
# Args: userName - the sender's name/handle
#       clientSocket - the socket file descriptor
#       outMsg - the message to be sent
#
# Pre:  A connection with a client must be established
#
# Post: Message transmitted.
# ---------------------------------------------------------------------------- #
def sendMsg(userName, clientSocket, outMsg):
    # cite references #1 and #5
    clientSocket.send(bytes(userName+"> "+outMsg,"utf-8"))


# ---------------------------------------------------------------------------- #
# Func: beginChat()
#
# Desc: Initiates loop of receive/send sequence.  Within this loop, the user is
#       prompted to enter their outgoing message.  If the user enters \quit
#       the function will return, and the server will wait for another client
#       connection.
#
# Args: userName - the sender's name/handle
#       clientSocket - the socket file descriptor
#       maxMsg - maximum number of chars to be sent or received in a message
#       maxTotal - maximum number of chars in a message plus handle
#
# Pre:  A connection with a client must be established
#
# Post: Messages can be sent/received until the user enters \quit or the client
#       disconnects.
# ---------------------------------------------------------------------------- #
def beginChat(userName, clientSocket, maxMsg, maxTotal):
    while True:
        if (receiveMsg(clientSocket, maxTotal)) == -1: #receive incoming message
            return # connection terminated by client

        while True:
            print(f"{userName}>", end=" ") # print prompt with user handle
            outMsg = input() # get the outgoing message

            if len(outMsg) not in range(0,maxMsg+1):
                print("You exceed the maximum message length of 500 charachters.")
                print("Message was not sent.")
            else:
                break

        if outMsg == "\\quit": # check for quit directive
            print("The client has been disconnected.")
            clientSocket.close()
            return

        sendMsg(userName, clientSocket, outMsg) # send the message


# ---------------------------------------------------------------------------- #
# Function: main()
#
# Desc: Main function that controls the program flow and makes calls to the
#       other functions
#
# Pre:  hostname and port number must be supplied as command line arguments
#
# Post: Any created connectons will be terminated
# ---------------------------------------------------------------------------- #
def main():
    # define maximum lengths for message and name
    MAX_CHAT_CHARS = 500
    MAX_NAME_CHARS = 10
    MAX_TOTAL = MAX_CHAT_CHARS + MAX_NAME_CHARS + 2

    # check for proper numbr of arguments
    if len(sys.argv) < 2:
        print("Please supply the required command line arguments.")
        print("Usage: python3 server.py [port]")
        sys.exit(0)

    serverSocket = openSocket('', sys.argv[1]) # create the socket
    userName = getUserName(MAX_NAME_CHARS) # get user name

    while True: # loop until program is terminated
        print("Waiting for a connection...")
        clientSocket, address = serverSocket.accept()
        print(f"Connection established with {address}.")
        print("Waiting on message from client...")
        beginChat(userName, clientSocket, MAX_CHAT_CHARS, MAX_TOTAL)

main()
