--------------------------------------------------------------------------------
Author:         Alex Rueb
Course Name:    CS372_400_SU19
Last Modified:  7/28/2019
--------------------------------------------------------------------------------


START THE CHAT SERVER AND CLIENT
--------------------------------------------------------------------------------
Step 1: Start the chat server using the following terminal commands:

  make server
  ./chatserve.py <PORT NUMBER>

  Note: <PORT NUMBER> is the desired port number that the server will use to
        listen for incoming connections (eg: 30020).

        Alternatively, you can also run the command:
        python3 chatserve.py <PORT NUMBER>


Step 2: Compile and run the chat client with the following terminal commands:

  make client
  ./chatclient <HOSTNAME> <PORT NUMBER>

  * <HOSTNAME> refers to the chat server (eg: flip1.engr.oregonstate.edu).
    <PORT NUMBER> refers to the port specified for the chat server (eg: 30020).


USAGE NOTES
--------------------------------------------------------------------------------
* The server should be running prior to starting the client.
* The server and client will each prompt the user to enter their name to be used
  as chat handles (maximum of 10 characters).
* The client must send the first message before the server can respond.
* The client and server alternate sending one message at a time (maximum of 500
  charachters).
* Either the client or server can terminate the connection by typing "\quit"
  (without quotation marks).
* If the server disconnects the client by calling the \quit command, the client
  program will terminate, but the server will remain running and can accept
  other clients.
* If the client disconnects from the server by calling the \quit command, the
  client will close and the server will continue running.
