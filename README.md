# Distributed-Lookup-Service
It is a dictionary search to overcome the task of tracking down data across a distributed network. 

Pre-requirement:
Ubuntu 16.04 

The client issues a dictionary key search to the main server after establishing a TCP connection with the server. The main server then further iterates the key search request of the client to the three backend servers via UDP connection, receives the key value pair from the backend servers and replies the client with the answer over the same TCP connection previously established. It includes TCP and UDP Socket Programming in C.
The project is implemented on Ubuntu 16.04 Virtual Machine.
