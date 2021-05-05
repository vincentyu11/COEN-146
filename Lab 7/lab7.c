//Name: Vincent Yu
//Date: 05/19/2020
//Title: Lab 7
//Description: This program demonstrates link state routing with djikstra's algorithm

/*
 * Link State Routing
 * Lab 7
 */

#include <unistd.h>
// Import the required libraries
//******STUDENT WORK******
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>


//defines
#define	N			4
#define	INFINITE	100



// types
typedef struct machines
{
	//******STUDENT WORK******	
	char name[30];
	char ip[30];
	int port;


} MACHINES;


// global variables
MACHINES	machines[N];
int			costs[N][N];
int			distances[N];
int			myid, nodes;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;



// functions
void *run_link_state (void *);
void *receive_info (void *);
void print_costs (void);


/**********************
 * main
 **********************/
int
main (int argc, char *argv[])
{
	FILE	*fp;
	int		i, j;
	pthread_t	thr1, thr2;
	int		id, cost;
	int		packet[3];

	if (argc < 5)
	{
		// check the number of arguments
        //******STUDENT WORK******
		fprintf(stderr, "Usage: %s <routerid> <nodes> <file1> <file2>\n", argv[0]);
        return 1;
		
	}

	myid = atoi(argv[1]); //******STUDENT WORK******
	nodes = atoi(argv[2]); //******STUDENT WORK******

	if (myid >= N)
	{
		printf ("wrong id\n");
		return 1;
	}	

	if (nodes != N)
	{
		printf ("wrong number of nodes\n");
		return 1;
	}	

	//get info on machines from the file using fopen and fscanf and populate the array of machines-struct
	//******STUDENT WORK******
	char str1[30], str2[30];
	int mindex = 0;
	int portnum;
	FILE *fp1;
	fp1 = fopen(argv[4], "r");
	while (!feof(fp1)) {
		fscanf(fp1,"%s %s %d\n",str1, str2, &portnum);
		memcpy(machines[mindex].name, str1, 30);
		memcpy(machines[mindex].ip, str2, 30);
		machines[mindex].port = portnum;
		mindex++;
	}
	fclose(fp1);
	
	int k;
	for (k=0;k<4;k++) {
		printf("%s ",machines[k].name);
		printf("%s ",machines[k].ip);
		printf("%d \n",machines[k].port);
	}

	printf("\n\n");
	

	int costindex = 0;
	int num0, num1, num2, num3;
	// get costs from the costs file and populate the matrix
	fp = fopen(argv[3], "r");
	
	//******STUDENT WORK******
	while (!feof(fp)) {
		fscanf(fp,"%d %d %d %d\n", &costs[costindex][0], &costs[costindex][1], &costs[costindex][2], &costs[costindex][3]);
		costindex++;
	}
	fclose(fp);

	for (k=0;k<4;k++) {
		printf("%d ",costs[k][0]);
		printf("%d ",costs[k][1]);
		printf("%d ",costs[k][2]);
		printf("%d \n",costs[k][3]);
	}
	printf("\n\n");

	//******STUDENT WORK******
		
	
    // init address
    //******STUDENT WORK******
    addr.sin_family = AF_INET;
    addr.sin_port = htons(machines[myid].port);
    addr.sin_addr.s_addr = INADDR_ANY;
	addr_size = sizeof(addr);

    // create socket
    //******STUDENT WORK******
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // bind
    //******STUDENT WORK******
	if ((bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr))) < 0){
	    perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

	// create threads and initiate mutex
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard, update the costs matrix inform other nodes about the changes
	{ // This section can be inside the main or can be a thread by itself
		for (i = 0; i < 3; i++)
		{
			printf("Input new neighbor <id> and <cost>: \n");
			scanf("%d%d", &id, &cost);
			if (id >= nodes) {
				printf("id out of bounds\n");
				break;
			}

			if (id == myid) {
				printf("same id as myid\n");
				break;
			}

			if (cost == INFINITE) {
				printf("no connection between nodes");
				break;
			}

			pthread_mutex_lock (&lock);
			costs[myid][id] = cost;
			costs[id][myid] = cost;
			pthread_mutex_unlock (&lock);
			
			printf("\n");
			print_costs();

			//******STUDENT WORK******
			packet[0] = myid;
			packet[1] = id;
			packet[2] = cost;
			//send packet to neighbors

			int f;
			for(f=0; f<N; f++){
			//******STUDENT WORK******
				pthread_mutex_lock (&lock);
				int connect = costs[f][myid];
				pthread_mutex_unlock (&lock);
				if (f != myid && connect != INFINITE) {
					struct hostent *host;
 					host = (struct hostent *)gethostbyname(machines[f].ip);
					otheraddr.sin_family = AF_INET;
					otheraddr.sin_port = htons(machines[f].port);
					otheraddr.sin_addr = *((struct in_addr *)host->h_addr);
					sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&otheraddr, addr_size);
					printf ("sent to node %d port %d\n", f, machines[f].port);
				}
				
			}
		}
	}	

	sleep (5);

}


/**********************
 * receive info
 **********************/
//If there is a cost update on other machine, they will let us know through a UDP packet. We need to receive that packet and update our costs table
void *receive_info (void *arg)
{
	int	new[3];
	int	nbytes;
	int	n0, n1, n2;
	while (1) {
		recvfrom(sock, new, sizeof(new), 0, NULL, NULL);

		n0 = new[0];//src
		n1 = new[1];//dest
		n2 = new[2];//new cost
		printf("%d %d %d\n", n0,n1,n2);
		
		pthread_mutex_lock (&lock);
		costs[n0][n1] = n2;
		costs[n1][n0] = n2;
		pthread_mutex_unlock (&lock);
		
	}

	
}


/**********************
 * run_link_state
 **********************/
// This thread runs the Dijkastra's algorithm on the cost table and calculates the shortest path
void *
run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j, k;

	//******STUDENT WORK******
	while (1) {
		for (i=0; i<N; i++) {
			distances[i] =  INFINITE;
			taken[i] = 0;
		}
		distances[myid] = 0;

		for (i=0; i<N-1; i++) {
			min = INFINITE;
			for (j=0; j<N; j++) {
				if (taken[j] == 0 && distances[j] <= min) {
					min = distances[j];
					spot = j;
				}
			}
			taken[spot] = 1;
			for (k=0; k<N; k++) {
				pthread_mutex_lock (&lock);
				if ((taken[k] == 0) && (costs[spot][k] != INFINITE) && (distances[spot] != INFINITE) && ((distances[spot]+costs[spot][k]) < distances[k])) {
					distances[k] = distances[spot] + costs[spot][k];
				}
				pthread_mutex_unlock (&lock);
			}
		}


		printf ("new-shortest distances:\n");
		for (i = 0; i < N; i++) {
			printf ("%d ", distances[i]);
		}
		printf ("\n\n");
		sleep(5);
	}
}


/**********************
 * print costs
 **********************/
void
print_costs (void)
{
	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			pthread_mutex_lock (&lock);
			printf ("%d ", costs[i][j]);
			pthread_mutex_unlock (&lock);
		}
		printf ("\n");
	}

}
