// CS 2690 Program 1 
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 3/2/2023
// Isaac Hill CS 2690-001 2/28/2023
// Windows Version: Windows 10    Visual Studio Version: Visual Studio Community 2019
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the 
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other 
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero 
// on this project if I violate this policy.
// ----------------------------------------------------------------------------

// Minimum required header files for C Winsock program
#include <stdio.h>       // for print functions
#include <stdlib.h>      // for exit() 
#include <winsock2.h>	 // for Winsock2 functions
#include <ws2tcpip.h>    // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h>    // optional - needed for MS IP Helper

// #define ALL required constants HERE, not inline 
// #define is a macro, don't terminate with ';'  For example...
#define RCVBUFSIZ 50

// declare any functions located in other .c files here
void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination

void main(int argc, char* argv[])   // argc is # of strings following command, argv[] is array of ptrs to the strings
{
	// Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
	WSADATA wsaData;                // contains details about WinSock DLL implementation
	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info
	int numArgs, len, size;         // numArgs: contains the number of arguments   len: contains current length of phrase  size: contains size of serverInfo
	unsigned short serverPort;      // contains the server port number
	char* serverIPaddr, * phrase;   // serverIPaddr: contains the server IP address   phrase: contains the message to pass to server and recieve back
	SOCKET sock;                    // contains the IPv6 TCP stream socket
	char rcvBuffer[RCVBUFSIZ];      // contains the returned message from the server
	int bytesSent = 0;              // contains the amount of bytes sent to the server
	int rcvRead = 0;                // contains the amount of bytes returned by the server (per call of recv)
	int bytesRead = 0;              // contains the total amount of bytes returned by the server (added after each call of recv)

	// Verify correct number of command line arguments, else do the following:
	// fprintf(stderr, "Helpful error message goes here"\n");
	// exit(1);	  // ...and terminate with abnormal termination code (1)
	numArgs = argc;
	//If there are not 4 arguments (3 not including file) give an error
	if (numArgs != 4)
	{
		fprintf(stderr, "Incorrect number of command line arguments. 3 needed. Exiting...\n");
		exit(1);
	}

	// Retrieve the command line arguments. (Sanity checks not required, but port and IP addr will need
	// to be converted from char to int.  See slides 11-15 & 12-3 for details.)
	serverIPaddr = argv[1];
	serverPort = atoi(argv[2]);
	phrase = argv[3];
	len = strlen(phrase);
	size = sizeof(serverInfo);

	// Initialize Winsock 2.0 DLL. Returns 0 if ok. If this fails, fprint error message to stderr as above & exit(1).  
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		fprintf(stderr, "Error Initializing Winsock 2.0. Exiting...\n");
		exit(1);
	}

	// Create an IPv6 TCP stream socket.  Now that Winsock DLL is loaded, we can signal any errors as shown on next line:
	// DisplayFatalErr("socket() function failed.");
	// Display helpful confirmation messages after key socket calls like this:
	// printf("Socket created successfully.  Press any key to continue...");
	// getchar();     // needed to hold console screen open
	sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		DisplayFatalErr("socket() function failed.");
	}
	printf("Socket created successfully. Press any key to continue...");
	getchar();

	// Zero out the sockaddr_in6 structure and load server info into it.  See slide 11-15.
	// Don't forget any necessary format conversions.
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
	inet_pton(AF_INET6, serverIPaddr, &serverInfo.sin6_addr);

	// Attempt connection to the server.  If it fails, call DisplayFatalErr() with appropriate message,
	// otherwise printf() confirmation message
	if (connect(sock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) < 0)
	{
		DisplayFatalErr("connect() function failed.");
	}

	// Send message to server (without '\0' null terminator). Check for null msg (length=0) & verify all bytes were sent...
	// ...else call DisplayFatalErr() with appropriate message as before

	// Verifies that message length is not 0, calls send, and puts the amount of bytes sent into bytesSent
	if (len == 0 || (bytesSent = send(sock, phrase, len, 0)) != len)
	{
		DisplayFatalErr("send() function failed.");
	}

	// Retrieve the message returned by server.  Be sure you've read the whole thing (could be multiple segments). 
	// Manage receive buffer to prevent overflow with a big message.
	// Call DisplayFatalErr() if this fails.  (Lots can go wrong here, see slides.)

	// Continue looping until the bytesRead equals the bytesSent (may run once of more depending on how much is sent back)
	// This will also print the message returned by the server as it recieves it
	while (bytesRead != bytesSent)
	{
		//Call recv() and update rcvRead to equal the amount of bytes recieved and if it is less than 0 give an error
		if ((rcvRead = recv(sock, rcvBuffer, RCVBUFSIZ - 1, 0)) <= 0)
		{
			DisplayFatalErr("recv() function failed.");
		}
		bytesRead += rcvRead;       //Add rcvRead bytes to the total bytesRead
		rcvBuffer[rcvRead] = '\0';  //Add a null terminator to the end of the current recieved message
		printf("%s", rcvBuffer);    //Print the current message in rcvBuffer
	} // end while (bytesRead != bytesSent)
	printf("\n");  //Prints new line after recieveing all of the message.

	// Display ALL of the received message, in printable C string format.
	//rcvBuffer[RCVBUFSIZ - 1] = '/0';
	//printf("Message Recieved: %s\n", rcvBuffer);

	// Close the TCP connection (send a FIN) & print appropriate message.
	printf("Closing IPv6 TCP stream socket... Good bye!\n");
	closesocket(sock);
	

	// Release the Winsock DLL
	WSACleanup();
	printf("Closing Winsock...\n");
	exit(0);
}
