/**
 * @author Mehmet Yasin SEREMET
 * Student ID: 2017400102
 * Bogazici University Computer Engineering Student
 */

/**
 * This project aims that to create 3 tellers in the given theater. These tellers take care of the
 * given number of clients who want to reserve specific seats. There waiting times for arrivals 
 * and services. While implementing this, representing the tellers&clients as threads and using 
 * mutex&semaphores are significant.
 */
 
 /**
 * ticket.cpp consists of a struct, 6 mutexes, 2 semaphores, 10 global variables, and 1 main and 2 thread methods.
 * All variables are explained below. In the main method, the program reads the input file, put the read info to the
 * required variables, and create the teller&client threads. While the client threads wait their turn and choose the
 * available teller. Teller threads help them to reserve a seat without any conflict.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <mutex>
#include <bits/stdc++.h>
#include <string.h>

using namespace std;

//representing each client 
struct client {
	string clientName;		//name of the client
	int arrivalTime;		//arrival time of the client (in ms)
	int serviceTime;		//service time of the client (in ms)
	int seatNumber;			//seat number requested by the client
	int reservedSeatNumber;	//seat number reserved for the client
	int clientNumber;		//specific number for each client to count them
	string whichTeller; 	//the teller making reservation for the client
};

pthread_mutex_t mutex1; //mutex to avoid the conflict of printing at teller arrive
pthread_mutex_t mutex2; //mutex to avoid the conflict of total reservation decrease
pthread_mutex_t mutex3; //mutex to avoid the conflict of choosing the correct teller by tellers
pthread_mutex_t mutex4; //mutex to avoid the conflict of choosing the correct seat
pthread_mutex_t mutex5; //mutex to avoid the conflict of choosing the correct teller by client
pthread_mutex_t mutex6; //mutex to avoid the conflict of printing the client reservation

sem_t tellerEmptySem;	//semaphore checking empty tellers
sem_t tellerBusySem;	//semaphore checking busy tellers

string theaterName; 	//name of the theater 
int totalClientNumber;	//total client number
int totalReservation;	//total reservation number
int seatCapacity;		//total seat capacity of the theater
ofstream outputStream;	//output stream

vector<client> clientVector;		//vector storing clients
vector<bool> seatsVector;			//vector storing seats to check their availability
vector<bool> tellerChooseVector;	//vector for tellers to be correctly chosen by the clients
vector<bool> tellerBusyVector;		//vector storing which teller is busy
vector<int> tellerWhichClientVector;//vector storing the clients taken care of by the tellers

void *tellerThread(void *param);	//thread of tellers
void *clientThread(void *param);	//thread of clients

//reading input, initializing values of the global variables, and creating the threads
int main(int argc, char *argv[]){

	ifstream inputStream(argv[1]); //input stream
	string input;	//temporary string storing input
		
	//reading input file and filling the clientVector with client information
	if(inputStream.is_open()){
		getline(inputStream, input);
		theaterName=input;
		getline(inputStream, input);
		totalClientNumber=stoi(input);
		
		for(int i=0;i<totalClientNumber;i++){
			getline(inputStream, input);
			stringstream ss(input);
			client c1;
			
			string sub;
			getline(ss, sub, ',');
			c1.clientName=sub;
			getline(ss, sub, ',');
			c1.arrivalTime=stoi(sub);
			getline(ss, sub, ',');
			c1.serviceTime=stoi(sub);
			getline(ss, sub, ',');
			c1.seatNumber=stoi(sub);
			c1.reservedSeatNumber=-1;
			c1.clientNumber=i+1;

			clientVector.push_back(c1);
		}
	}
	
	//initializing seat capacity of the theater
	if(theaterName.substr(0,1)=="O"){
		seatCapacity=60;
	}
	else if(theaterName.substr(0,1)=="U"){
		seatCapacity=80;
	}
	else if(theaterName.substr(0,1)=="K"){
		seatCapacity=200;
	}
	
	//initializing all seats empty
	for(int i=0;i<seatCapacity;i++){
		seatsVector.push_back(false);
	}
	
	//initializing tellers empty-not busy
	for(int i=0;i<3;i++){
		tellerChooseVector.push_back(false);
		tellerBusyVector.push_back(false);
		tellerWhichClientVector.push_back(-1);
	}
	
	
	totalReservation=totalClientNumber;
	
	//starting the output
	outputStream.open(argv[2]);
	
	outputStream<<"Welcome to the Sync-Ticket!"<<endl;
	
	//initializing mutex&semaphores
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);
	pthread_mutex_init(&mutex4, NULL);
	pthread_mutex_init(&mutex5, NULL);
	pthread_mutex_init(&mutex6, NULL);
	sem_init(&tellerEmptySem,0,3);
	sem_init(&tellerBusySem,0,0);
	
	pthread_t tid[3]; //teller thread ids
	pthread_t cid[totalClientNumber]; //client thread ids
	
	
	char* str1 = (char *)calloc(100, sizeof(char));
	strcat(str1, "A");
	char* str2 = (char *)calloc(100, sizeof(char));
	strcat(str2, "B");
	char* str3 = (char *)calloc(100, sizeof(char));
	strcat(str3, "C");
	
	
	//creating 3 teller threads
	pthread_create(&tid[0], NULL, tellerThread, str1);
	usleep(10000);
	pthread_create(&tid[1], NULL, tellerThread, str2);
	usleep(10000);
	pthread_create(&tid[2], NULL, tellerThread, str3);
	usleep(10000);
	
	//creating all client threads
	for(int i=0;i<totalClientNumber;i++){
		int *arg = (int *)malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&cid[i],NULL, clientThread, arg);
	}
	
	//waiting for client threads
	for(int i=0;i<totalClientNumber;i++){
		pthread_join(cid[i],NULL);
	}
	
	//waiting for 3 teller threads
	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);
	pthread_join(tid[2],NULL);
		
	outputStream<<"All clients received service."<<endl;
	
	//destroying mutex&semaphores
	pthread_mutex_destroy(&mutex1);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&mutex3);
	pthread_mutex_destroy(&mutex4);
	pthread_mutex_destroy(&mutex5);
	pthread_mutex_destroy(&mutex6);
	sem_destroy(&tellerEmptySem);
	sem_destroy(&tellerBusySem);
	
	return 0;
}

//reserving the correct seats for clients, printing them
void *tellerThread(void *tellerLetter){  
	
	//name of the teller
	string tellerCode=string((char *)tellerLetter);
	free(tellerLetter);
	
	//printing the arrival of teller
	pthread_mutex_lock(&mutex1);
	string tellerEntry="Teller "+tellerCode+" has arrived.";
	outputStream<<tellerEntry<<endl;
	pthread_mutex_unlock(&mutex1);
	
	//while there are clients to service, tellers take care of them
	while(totalReservation>0){	
		pthread_mutex_lock(&mutex2);
		totalReservation--;
		pthread_mutex_unlock(&mutex2);
		 
		//when a client comes, a teller starts the process by this semaphore
		sem_wait(&tellerBusySem);
		
		//choosing the available&correct teller
		pthread_mutex_lock(&mutex3);
		int tellerID=-1;
		if(tellerChooseVector.at(0)==true && tellerBusyVector.at(0)==false){
			tellerBusyVector.at(0)=true;
			tellerID=0;
		}
		else if(tellerChooseVector.at(1)==true && tellerBusyVector.at(1)==false){
			tellerBusyVector.at(1)=true;
			tellerID=1;
		}
		else if(tellerChooseVector.at(2)==true && tellerBusyVector.at(2)==false){
			tellerBusyVector.at(2)=true;
			tellerID=2;
		}
		pthread_mutex_unlock(&mutex3);
		
		
		//reserving the correct seat
		pthread_mutex_lock(&mutex4);
		if(clientVector.at(tellerWhichClientVector.at(tellerID)).clientNumber>seatCapacity){
			
		} 
		else if(clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber>seatCapacity){
			for(int i=0;i<seatCapacity;i++){
				if(seatsVector.at(i)==false){
					clientVector.at(tellerWhichClientVector.at(tellerID)).reservedSeatNumber=i+1;
					seatsVector.at(i)=true;
					break;
				}
			}
		}
		else{
			if(seatsVector.at(clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber-1)==false){
				seatsVector.at(clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber-1)=true;
				clientVector.at(tellerWhichClientVector.at(tellerID)).reservedSeatNumber=clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber;
			}
			else{
				for(int i=0;i<seatCapacity;i++){
					if(seatsVector.at(i)==false){
						clientVector.at(tellerWhichClientVector.at(tellerID)).reservedSeatNumber=i+1;
						seatsVector.at(i)=true;
						break;
					}
				}
			}
		}	
		pthread_mutex_unlock(&mutex4);
		
		//sleeping for service time (in ms)
		usleep(clientVector.at(tellerWhichClientVector.at(tellerID)).serviceTime*1000);
		
		
		//printing the required info
		pthread_mutex_lock(&mutex6);
		if(clientVector.at(tellerWhichClientVector.at(tellerID)).reservedSeatNumber==-1){
			string noSeat=clientVector.at(tellerWhichClientVector.at(tellerID)).clientName+" requests seat "+to_string(clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber)+
			", reserves None. Signed by Teller "+clientVector.at(tellerWhichClientVector.at(tellerID)).whichTeller+".";
			outputStream<<noSeat<<endl;
		}
		else{
			string yesSeat=clientVector.at(tellerWhichClientVector.at(tellerID)).clientName+" requests seat "+to_string(clientVector.at(tellerWhichClientVector.at(tellerID)).seatNumber)+
			", reserves seat "+to_string(clientVector.at(tellerWhichClientVector.at(tellerID)).reservedSeatNumber)+". Signed by Teller "+clientVector.at(tellerWhichClientVector.at(tellerID)).whichTeller+".";
			outputStream<<yesSeat<<endl;
		}
		pthread_mutex_unlock(&mutex6);
		
		//teller becomes available, finishing his job with current client
		tellerChooseVector.at(tellerID)=false;
		tellerBusyVector.at(tellerID)=false;
		tellerWhichClientVector.at(tellerID)=-1;
		
		sem_post(&tellerEmptySem);
		
	}	
	
	//finishing his job, exiting
	pthread_exit(NULL);
	return 0;
}

//waiting for his time, choosing the available teller
void *clientThread(void *param){
	
	//id of the client
	int clientID = *(int *)param;
	free(param);
	
	//sleeping for arrival time
	usleep(clientVector.at(clientID).arrivalTime*1000);
	
	//waiting for available-empty teller
	sem_wait(&tellerEmptySem);
	
	//choosing the available teller
	pthread_mutex_lock(&mutex5);
	if(tellerChooseVector.at(0)==false){
		tellerChooseVector.at(0)=true;
		tellerWhichClientVector.at(0)=clientID;
		clientVector.at(clientID).whichTeller="A";
	}
	else if(tellerChooseVector.at(1)==false){
		tellerChooseVector.at(1)=true;
		tellerWhichClientVector.at(1)=clientID;
		clientVector.at(clientID).whichTeller="B";
	}
	else if(tellerChooseVector.at(2)==false){
		tellerChooseVector.at(2)=true;
		tellerWhichClientVector.at(2)=clientID;
		clientVector.at(clientID).whichTeller="C";
	}
	pthread_mutex_unlock(&mutex5);
	
	//keeping one teller busy, increasing the busy teller number
	sem_post(&tellerBusySem);
	
	
	//waiting for service time
	usleep(clientVector.at(clientID).serviceTime*1000);
	
	//exiting the ticket office
	pthread_exit(NULL);
	
	return 0;
}
