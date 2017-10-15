<========::COMPILE::========>
g++ -std=c++11 server_1.cpp -o server_1 -lpthread
g++ -std=c++11 server_2.cpp -o server_2 -lpthread
g++ -std=c++11 load_gen.cpp -o load -lpthread


<========::EXECUTION::========>

one machine:	
	1. taskset 0x01 ./server_2 <no of iteration>
	2. taskset 0x02 ./server_1 <127.0.0.1> <no of iteration>


another machine:
	2. taskset 0x0f <server_1 IP> <no of iteration> <num_threads>