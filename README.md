<h1>Berkeley's clock synchronization algorithm</h1>

Implemented Berkeley algorithm using C++ to achieve clock synchronization in a distributed
system.<br> 

<h3>The algorithm follows the below steps:</h3>

1. A Master/Daemon node is chosen among all the nodes in a distributed system, and the remaining nodes act as slaves.<br>
2. At first, Daemon sends a message to all the other nodes in the network, asking for their respective clock values.<br>
3. All the nodes send their clock to Daemon as requested.<br>
4. Daemon then calculates the average clock value by including itself.<br>
5. Daemon then adds the calculated time difference to its local clock and then broadcasts the respective time differences to all the other nodes in the network.<br>
6. All nodes then adjust their clocks based on the time difference sent by the daemon.<br>


<h3>How to run?</h3>

1. Run “make clean” - to clean all the object files from the project folder.
2. Run “make” - the make command will compile both "daemon.cpp" and "client.cpp" files to generate server and client executables.
3. Use “./daemon” to run the server program, and run “./client” in a different window will run the client.
4. Use multiple windows to connect m multiple clients to the daemon.


