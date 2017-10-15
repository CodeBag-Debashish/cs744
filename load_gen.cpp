/*
	Debashish Deka
	173050055
	*/

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>
#include <sys/time.h>

using namespace std;

#define N 			2
#define PORT 		8787
#define BUF_SIZE	1024
#define BUF_SIZE_SMALL 64
#define ID_SIZE		10
#define MT_LOGIN	0
#define MT_CONV 	1
#define MT_ENTRY	2
#define MT_WIN		3
#define MT_DRAW		4
#define REJECT_1 	5
#define REJECT_2	6
#define REJECT_3	7
#define ME 			1
#define OP 			0

#define bytes_to_read_for_turn	1
#define bytes_to_read_for_move	1




struct id_pair {
	int p1;
	int p2;
	int user_number;
	double __time;
};

typedef struct data {
  int socket_fd;
}p_address;

char DELIM = ':';
/*template code for LOG purpose */
template<typename T> 
void LOG(T const& t) { 
	std::cout << t; 
}
template<typename First, typename ... Rest> 
void LOG(First const& first, Rest const& ... rest) {
	std::cout << first<<" "; LOG(rest ...);
}

/*template code for log purpose */

unsigned int client_status = 0; // this will act as flag
void display_table(char (*)[3]);
void clear_table(char (*)[3],bool (*)[3]);
//void update_table(vector<pair<int,int>>,char (*)[3],char,char);
bool is_correct_move(vector<pair<int,int>>,char (*)[3],char);
bool check(char (*)[3],char);
void clear_stdin();

char * server_address;

void update_table(vector<pair<int,int>> &positions,char table[][3],char choice,char marker) {
	table[positions[choice].first][positions[choice].second] = marker;
}


int ITER = 0;
int NUM_THREADS= 0;


typedef unsigned long long timestamp_t;

static timestamp_t get_timestamp() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

void *load_gen(void *ptr) {	
	
	int total_time = 0;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0) {
		
		perror("\t\tsocket creation failed in client side : ");
		exit(1);
	
	}
	
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;  
	addr.sin_addr.s_addr = inet_addr(server_address);
	addr.sin_port = PORT;

	if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {  
		perror("\t\tError connecting to the server-1:\n");  
		exit(1);
	}
		
	char my_id[ID_SIZE],op_id[ID_SIZE];
	
	sprintf(my_id, "%d", ((id_pair*)ptr)->p1);
	sprintf(op_id, "%d", ((id_pair*)ptr)->p2);

	char *buffer = (char *)malloc(BUF_SIZE_SMALL);
	memset(buffer,0,BUF_SIZE_SMALL);
	
	buffer[0] = buffer[0] | (1<< MT_LOGIN);
	
	int idx = 1;
	for(int i=0;i<strlen(my_id);i++) buffer[idx++] = my_id[i];
	buffer[idx++] = DELIM;
	for(int i=0;i<strlen(op_id);i++) buffer[idx++] = op_id[i];
	buffer[idx] = DELIM; 




/*	clock_t _begin = clock();
	clock_t _end = clock();*/
	timestamp_t _begin = get_timestamp();
	timestamp_t _end;

	int aa = 0;
	if( (aa = send(sockfd,buffer,strlen(buffer),0) )< 0) {
	
		perror("\t\tError ending data.\n");
		exit(0);
	
	}else {

	
	}

	


	memset(buffer,0,BUF_SIZE_SMALL);
	


	
	int res = 0;
	bool op_move_already_recieved = false;
	char op_move;

	if( (res = recv(sockfd,buffer,BUF_SIZE_SMALL,0) ) < 0) { // waiting and recieve the ok code for the game to start
		perror("\t\tError : recv failed inside rcv_data_from_server function\n");
		exit(0);
	}else {
		
		if(res >= 2) {
			cout <<" HELLO = "<< res <<endl;
			op_move_already_recieved = true;
			op_move = buffer[1];

		}
	}



	bool turn = (buffer[0] == 1)?true:false;


	vector<pair<int,int>> positions(10,make_pair(0,0));
	
	positions[1].first = 0; positions[1].second = 0;
	positions[2].first = 0; positions[2].second = 1;
	positions[3].first = 0; positions[3].second = 2;
	positions[4].first = 1; positions[4].second = 0;
	positions[5].first = 1; positions[5].second = 1;
	positions[6].first = 1; positions[6].second = 2;
	positions[7].first = 2; positions[7].second = 0;
	positions[8].first = 2; positions[8].second = 1;
	positions[9].first = 2; positions[9].second = 2;

	while(true) {

			
			
			char table[3][3];
			
			bool flag[3][3];
			memset(flag,0,sizeof(flag));
			memset(table,0,sizeof(table));
			
			char my_marker;
			char op_marker;
			if(turn) {
				my_marker = 'x';
				op_marker = 'o';
			}else {
				my_marker = 'o';
				op_marker = 'x';
			}

			char choice = 1;
			


			bool game_over = false;
			bool is_full = false;
			bool draw = false;
			bool status_shown = false;
			clear_table(table,flag);
			
			bool score_recieved = false;

			while(!game_over && !is_full) {

				memset(buffer,0,BUF_SIZE_SMALL);
				
				if(turn) {

					update_table(positions,table,choice,my_marker);
					
					if(check(table,my_marker)) {
						game_over = true;
					}
					
					bool ok = false;
					for(int i=0;i<3 && !ok;i++) {
						for(int j=0;j<3 && !ok;j++) {
							if(table[i][j] == ' ') ok = true; 
						}
					}

					if(!ok) is_full = true;
					if(is_full && !game_over) draw = true;
					if(!game_over && !is_full) {
						buffer[0] = buffer[0] | (1<< MT_ENTRY);
						buffer[1] = choice;
					}else if(game_over){
						buffer[0] = buffer[0] | (1<< MT_WIN);
						buffer[1] = choice;
					}else if(draw && !game_over) {

						buffer[0] = buffer[0] | (1<< MT_DRAW);
						buffer[1] = choice;
					}	
					int bb = 0;
					if( (bb = send(sockfd,buffer,strlen(buffer),0) ) < 0) {
						perror("Error ending data.\n");
						exit(0);
					}else {
						//cout << " bbb = "<<bb << endl;
					}
					
					turn ^= 1;

				}else {


					int res2 = 0;

					if(!op_move_already_recieved) {
						if( ( res2 = recv(sockfd,buffer,BUF_SIZE_SMALL,0) ) < 0) {
							perror("\t\tError : recv failed inside rcv_data_from_server function \n");
							exit(0);
						}else {

							if(res2 >= 2) {

								score_recieved = true;
								cout << "important : "<<res2<<buffer<<endl;

							}							
						
						}
					}else {

						choice = op_move;
						op_move_already_recieved = false;
					
					}


					
					choice = buffer[0];


					//printf("choice = %d\n", choice);
					
					update_table(positions,table,choice,op_marker);
					if(check(table,op_marker)) {
						game_over = true;

					}
					bool ok = false;
					for(int i=0;i<3 && !ok;i++) {
						for(int j=0;j<3 && !ok;j++) {
							if(table[i][j] == ' ') ok = true; 
						}
					}
					if(!ok) is_full = true;
					if(is_full && !game_over) draw = true;
					if(draw) {

						status_shown = true;
					}

					choice++;
					turn ^= 1;
				}
			}

			memset(buffer,0,BUF_SIZE_SMALL);
			buffer[0] = 'd';
			if( send(sockfd,buffer,strlen(buffer),0)  < 0) {
				perror("Error ending data. vvvv\n");
				exit(0);
			}else {
				//cout << " bbb = "<<bb << endl;
			}

			
			char c;
						
			int res3 = 0;
			
			if(!score_recieved) {

				if( (res3 = recv(sockfd,buffer,BUF_SIZE_SMALL,0)) < 0) {
					
					perror("\t\tError : recv failed inside rcv_data_from_server function \n");
					exit(0);
				
				}else {
					// printf("res3 = %d\n",res3);

					int scores[3],num = 0,idx = 0;
					for(int i=0;i<3;i++) {
						while((c = buffer[idx++]) != DELIM) {
							num = num * 10 + (c - '0');
						}
						scores[i] = num;
						num = 0;
					}


					//printf("Total Matches = %d | Total wins = %d | Total lost =  %d | Total Draw = %d\n",
					//			scores[0],scores[1],scores[2],scores[0] - scores[1] - scores[2]);
				
				}
			}else {
				
				// int scores[3],num = 0,idx = 1;
				// for(int i=0;i<3;i++) {
				// 	while((c = buffer[idx++]) != DELIM) {
				// 		num = num * 10 + (c - '0');
				// 	}
				// 	scores[i] = num;
				// 	num = 0;
				// }

				score_recieved = false;
			
			}




			
			//_end = clock();
			_end = get_timestamp();

			total_time++;
			if(total_time >= ITER) break;


		}

	((id_pair*)ptr)->__time += ( _end - _begin ) / 1000000.0L;

	free(buffer);
	close(sockfd);
	//free(ptr);

	pthread_exit((void*)(ptr));

}





int main(int argc,char **argv) {
	
	server_address = argv[1];

	ITER  = atoi(argv[2]);

	NUM_THREADS = atoi(argv[3]);

	printf("num_threads = %d\n",NUM_THREADS);

	//pthread_t thread_pool[NUM_THREADS];
	
	pthread_t *thread_pool;
	thread_pool = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);

	//id_pair * a[NUM_THREADS]; // shared variable : False sharing possible
	
	// clock_t _begin = clock();
	timestamp_t t0 = get_timestamp();

	for(int i=1;i<=NUM_THREADS;i++) {
		id_pair * a = (id_pair*)(malloc(sizeof(id_pair)));
		if(i%2) {
			a->p1 = i;
			a->p2 = i+1;
		}else {
			a->p1 = i;
			a->p2 = i-1;
		}
		a->user_number = i;
		a->__time = 0.0;

		if(pthread_create(&thread_pool[i-1],NULL,load_gen,(void*)(a))) {
			perror("\t\t Error : ");
			exit(0);
		}
	}
	int cnt = 0;
	void *result;
	double X = 0.0;
	double avg_res = 0.0;
	for(int i=0;i<NUM_THREADS;i++) {

		pthread_join(thread_pool[i],&result);
		avg_res += (((id_pair*)result)->__time)/ITER;
	
	}


	timestamp_t t1 = get_timestamp();
	double net_time = (t1 - t0) / 1000000.0L;
	
	avg_res = avg_res / NUM_THREADS;
	
	X = (ITER*NUM_THREADS) / net_time;

	cout << "X = "<< X <<"  T = "<<net_time<<"  R = "<<avg_res<< endl;

	//for(int i=0;i<NUM_THREADS;i++) {
	//	free(a[i]);
	//}

	free(thread_pool);

	exit(1);
}












void display_table(char table [][3]) {
	for(int i=0; i<3; i++) {
		printf(" %c | %c | %c ",table[i][0], table[i][1], table[i][2]);
		if(i<2) printf("\n---|---|---\n");
	}
	printf("\n");
}
void clear_table(char table[][3],bool flag[][3]) {
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			table[i][j] = ' ';
			flag[i][j] = false;			
		}
	}
}
bool is_correct_move(vector<pair<int,int>> &positions,char table[][3],char choice) {
	if( choice >= 1 && choice <= 9 ) {
		if(table[positions[choice].first][positions[choice].second] == ' ') {
			return true;
		}else 
			return false;
	}else {
		return false;
	}
}
bool check(char table[][3],char marker) {
	for(int i=0; i<3; i++)
		if(table[i][0] == marker && table[i][0]==table[i][1] && table[i][0]==table[i][2]) return true;
	for(int i=0; i<3; i++)
		if(table[i][0] == marker && table[0][i]==table[1][i] && table[0][i]==table[2][i]) return true;
	if(table[0][0] == marker && table[0][0]==table[1][1] && table[1][1]==table[2][2]) return true;
	if(table[0][2] == marker && table[0][2]==table[1][1] && table[1][1]==table[2][0]) return true;
	return false;
}
void clear_stdin() {
	char c;
	while (     (c = getchar()) != '\n'   &&      c != EOF    )  { }
}
