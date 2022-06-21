
# Chat Server

The project is an implementation of a chat server written in C++, using the concepts of
sockets and multi-threading, which works on Linux based OS. This server helps any user in 
effectively communicating with other users on the server and only requires a username from
the user, after which they can use the server. Since the sending and recieving of messages is 
done paralelly by individual threads for each client, hence the performance of the server is increased.



## Deployment

Since the chat server is written in cpp; therefore installation of
g++ is a prerequisite. 

To deploy the chat server, first open the terminal and compile the server.cpp and 
client.cpp files by commands:

```bash
  g++ -pthread -o server server.cpp
  g++ -pthread -o client client.cpp
```
After compiling the files, run the server by the command:
```bash
./server
```

After that, clients can join the server by:
```bash
./client
```


## Internal Working

The chat server conists of the server side and the client side which are connected to each other
through sockets. The port to which both the server and client sockets connect has been pre-set to 10000.
The concept of multi threading is used for sending and recieving the messages from a client. 
This is done to increase the performance of the server, since individual threads govern the sending and 
recieving of messages. Also, to mutex locks are used while adding and removing a client to the user queue.
This is to ensure that a single client's process is in critical section at a time and thus common data can only 
be changed by one client at a time.


### Server

The server side is designed to control how a client interacts with server
as well as with the other clients. For that, primarily we need functions
to add as well as remove clients from the queue. Since multi threading is
used in the server, hence it is essential to use mutex locks while adding
and removing clients from the list. A broadcast_message function has been used, which sends the message of a 
client to all other clients and the server. 

On runnning the server, intially the socket is created and after 
the bind socket and listen functions are executed the server is opened for client
processes to join. When any client attempts to join the server a new thread is
created and the client is added to the queue of active users.
Further, when a valid name is entered, the client becomes eligible to interact in the 
chat room. Then, all the messages of the client are sent with the broadcast_message
function to other clients and the server until the client sends the message exit.
On doing so, the client is removed from the server.

### Client

The client side primarily controls the sending and recieving of the messages. 
It consists of two functions handler_send and handler_recieve. 

When the server is active, on running the client and entering a valid name, 
the socket is created. After connecting to the socket, the name is sent to the
server and the client is ready to interact with the server and all other clients as well as
the server recieve a message that the client has joined the chat room. Whenever a message 
is sent by any other client, the name of the sending client along with the message
is diplayed on the client's terminal. If the client writes a message and unless
the message is not "exit" the message is broadcasted to all the users. On writing
exit, the client is removed from the server and all the other clients and the server
recieve a message forthe same.

## Video Demo

https://user-images.githubusercontent.com/98050969/174826909-ce090684-f7d1-47b6-91b6-0c994555a5e5.mp4

## Learnings

- In the course of this project, I have learned the working of sockets
and connecting as well as sending data between different sockets. 
- I have also understood how threads work as well as the concept of
multi-threading and mutex locks, which have been implemented
through POSIX threads in the chat server. 
## Additional Tasks

- From the basic socket programming model using multi-threading, the implementation of a real time chat server has been done.
 
- Threads are assigned to individual clients for sending and recieving messages as well as adding and recieving clients to the user queue.

- Mutex/locks have been used while adding and removing any client from the
  list of active users to ensure that common data is changed only by one client at a time.

- The joining and leaving of clients is color coded.

- Use of a keyword ("exit") has been done to remove a client from the server.
## References

- https://www.geeksforgeeks.org/socket-programming-cc/
- https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading 
- https://www.geeksforgeeks.org/multithreading-c-2/

