#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX 100


typedef struct Layer{
	char layerName[MAX];
	//Physical Layer
	char senderMAC[MAX];
	char receiverMAC[MAX];
	//Network Layer
	char senderIP[MAX];
	char receiverIP[MAX];
	//Transport Layer
	char senderPortNumber[MAX];
	char receiverPortNumber[MAX];
	//Application Layer
	char senderID[MAX];
	char receiverID[MAX];

	char messageChunk[MAX];
	
}Layer;

typedef struct Frame{
	Layer layers[4];
	int size;
	int hopsSize;
}Frame;

typedef struct Queue{
	int capacity;
	int size;
	int front;
	int rear;
	Frame *frames; 
}Queue;


typedef struct Client{
	char ID[MAX];
	char ip_addr[MAX];
	char mac_addr[MAX];
	char neigbours[MAX];
	 
	Queue *incomingQueue;
	Queue *outgoingQueue;
	
}Client;

void frameInit(Frame *frame){
	frame->size = 0;
	frame->hopsSize = 0;
}

Layer* frameTop(Frame *frame){
	if(frame->size == 0){
		return;
	}
	
	return &frame->layers[frame->size-1];
} 

Layer* frameSomewhere(Frame *frame, int index){
	if(frame->size == 0){
		return;
	}
	
	return &frame->layers[frame->size-1-index];
} 

void framePush(Frame *frame, Layer layer){
	if(frame->size < 4){
		frame->layers[frame->size++] = layer;
	}
	else{
	}
}

void framePop(Frame *frame){
	if(frame->size == 0){
	}
	else{
		frame->size--;
	}
}

Queue* createQueue(int queueSize){
	Queue *clientQueue;
	clientQueue = (Queue*)malloc(sizeof(Queue));
	
	clientQueue->frames = (Frame*)malloc(sizeof(Frame)*queueSize);
	clientQueue->capacity = queueSize;
	clientQueue->size = 0;
	clientQueue->front = 0;
	clientQueue->rear = -1;
	
	return clientQueue;
}

void Dequeue(Queue *clientQueue){
	//Check if queue is empty
	if(clientQueue->size == 0){
		return;
	}
	else{
		clientQueue->size--;
		clientQueue->front++;
		//Circular queue
		if(clientQueue->front == clientQueue->capacity){
			clientQueue->front = 0;
		}
	}
	return;
}

Frame* frontQueue(Queue *clientQueue){
	if(clientQueue->size == 0){
		exit(0);
	}

	return &clientQueue->frames[clientQueue->front];
}

Frame* somewhereQueue(Queue *clientQueue, int index){
	if(clientQueue->size == 0){
		exit(0);
	}
	return &clientQueue->frames[(clientQueue->front+index)%clientQueue->capacity];
}

void Enqueue(Queue *clientQueue, Frame element){
	//If queue is full then realloc
	if(clientQueue->size == clientQueue->capacity){
		printf("Buraya realloc gelecek\n");
	}
	else{
		clientQueue->size = clientQueue->size + 1;
		clientQueue->rear = clientQueue->rear + 1;
		//Circular
		if(clientQueue->rear == clientQueue->capacity){
			clientQueue->rear = 0;
		} 
		
		clientQueue->frames[clientQueue->rear] = element;
		
	}
	return;
}

void findRoute(Client* clientList, char* newID, int clientSize, char senderID[], char receiverID[]){
	int i, l, g;
	for(i = 0; i < clientSize; i++){
		if(strcmp(clientList[i].ID, senderID) == 0){
			for(l = 0; l < clientSize-1; l++){
				char myChar = receiverID[0];
				char neigh1 = clientList[i].neigbours[2*l];
				char neigh2 = clientList[i].neigbours[2*l+1];
				char neigh1Str[2]; 
				neigh1Str[0] = neigh1;
				neigh1Str[1] = '\0';
				char neigh2Str[2]; 
				neigh2Str[0] = neigh2;
				neigh2Str[1] = '\0';
			
				if(neigh1 == myChar && neigh2 == myChar){
					for(g = 0; g < clientSize; g++){
						if(strcmp(neigh1Str, clientList[g].ID) == 0){
							strcpy(newID, clientList[g].ID);
						}
					}
				}
				else if(neigh1 == myChar && neigh2 != myChar){			
					for(g = 0; g < clientSize; g++){
						if(strcmp(neigh2Str, clientList[g].ID) == 0){
							strcpy(newID, clientList[g].ID);
						}
					}
				}
			}
		}
	
	}

}


char* sendMessage(Client* clientList, int clientSize, char staticSenderID[], char senderID[], char receiverID[], int messagePartSize, int hops){
	int i, j, k, l;
	char routeID[255], message[1000], partOfMessage[messagePartSize];
	findRoute(clientList, routeID, clientSize, senderID, receiverID);

	for(i = 0; i < clientSize; i++){
		if(strcmp(clientList[i].ID, senderID) == 0 && strcmp(receiverID, routeID) == 0 && strcmp(staticSenderID, senderID) == 0){
			for(j = 0; j < clientSize; j++){
				if(strcmp(clientList[j].ID, receiverID) == 0){
					clientList[j].incomingQueue = createQueue(clientList[i].outgoingQueue->size);
					int sizeOfQueue = clientList[i].outgoingQueue->size;
					
	
					for(k = 0; k < sizeOfQueue; k++){
						Enqueue(clientList[j].incomingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
						Dequeue(clientList[i].outgoingQueue);
					}
				
					
					strncpy(partOfMessage, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, sizeOfQueue-1), 3)->messageChunk, messagePartSize);
					strcpy(message, partOfMessage);
					for(l = 1; l < sizeOfQueue; l++){
						strncat(message, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, sizeOfQueue-1-l), 3)->messageChunk, messagePartSize);
											
					}
					
					printf("A message received by client %s from client %s after a total of %d hops.\n", receiverID, staticSenderID, hops);
					printf("Message: %s\n", message);
				}
	
			}
			
		}
		else if(strcmp(clientList[i].ID, senderID) == 0 && strcmp(receiverID, routeID) == 0){
			printf("A message received by client %s, but intended for client %s. Forwarding...\n", senderID, receiverID);
			for(j = 0; j < clientSize; j++){
				if(strcmp(clientList[j].ID, receiverID) == 0){
					
					clientList[j].incomingQueue = createQueue(clientList[i].outgoingQueue->size);
					int sizeOfQueue = clientList[i].outgoingQueue->size;
					
					for(k = 0; k < sizeOfQueue; k++){
						strcpy(frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, k), 0)->senderMAC, clientList[i].mac_addr);
						strcpy(frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, k), 0)->receiverMAC, clientList[j].mac_addr);
					}
					for(k = 0; k < sizeOfQueue; k++){
						Enqueue(clientList[j].incomingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
						Dequeue(clientList[i].outgoingQueue);
					}
				
					
					strncpy(partOfMessage, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, sizeOfQueue-1), 3)->messageChunk, messagePartSize);
					strcpy(message, partOfMessage);
					for(l = 1; l < sizeOfQueue; l++){
						strncat(message, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, sizeOfQueue-1-l), 3)->messageChunk, messagePartSize);
											
					}
					for(k = 0; k < sizeOfQueue; k++){
						printf("\tFrame #%d MAC address change: New sender MAC %s, new receiver MAC %s\n", k+1, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, k), 0)->senderMAC, frameSomewhere(somewhereQueue(clientList[j].incomingQueue, k), 0)->receiverMAC);
					}
					printf("A message received by client %s from client %s after a total of %d hops.\n", receiverID, staticSenderID, hops);
					printf("Message: %s\n", message);
				}
	
			}
			
		}
		else if(strcmp(clientList[i].ID, senderID) == 0 && strcmp(staticSenderID, senderID) == 0 && strcmp(routeID, receiverID) != 0){
			
			for(j = 0; j < clientSize; j++){
				if(strcmp(clientList[j].ID, routeID) == 0){
					clientList[j].incomingQueue = createQueue(clientList[i].outgoingQueue->size);
					clientList[j].outgoingQueue = createQueue(clientList[i].outgoingQueue->size);
					int sizeOfQueue = clientList[i].outgoingQueue->size;
					for(k = 0; k < sizeOfQueue; k++){
						Enqueue(clientList[j].incomingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
						Enqueue(clientList[j].outgoingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
					}
					for(k = 0; k < sizeOfQueue; k++){
						Dequeue(clientList[i].outgoingQueue);
					}
					
				}
			
				
				
			}
				
				sendMessage(clientList, clientSize, staticSenderID, routeID, receiverID, messagePartSize, hops);
		}
		else if(strcmp(clientList[i].ID, senderID) == 0 && strcmp(staticSenderID, senderID) != 0){
			printf("A message received by client %s, but intended for client %s. Forwarding...\n", senderID, receiverID);
			
			int sizeOfQueue = clientList[i].outgoingQueue->size;
			for(j = 0; j < clientSize; j++){
				if(strcmp(clientList[j].ID, routeID) == 0){
					
					clientList[j].incomingQueue = createQueue(clientList[i].outgoingQueue->size);
					clientList[j].outgoingQueue = createQueue(clientList[i].outgoingQueue->size);			
					
					for(k = 0; k < sizeOfQueue; k++){
						strcpy(frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, k), 0)->senderMAC, clientList[i].mac_addr);
						strcpy(frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, k), 0)->receiverMAC, clientList[j].mac_addr);
					}
					for(k = 0; k < sizeOfQueue; k++){
						Enqueue(clientList[j].incomingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
						Enqueue(clientList[j].outgoingQueue, clientList[i].outgoingQueue->frames[sizeOfQueue-k-1]);
					}
					for(k = 0; k < sizeOfQueue; k++){
						Dequeue(clientList[i].outgoingQueue);
						Dequeue(clientList[i].incomingQueue);
					}
				
					
					
					for(k = 0; k < sizeOfQueue; k++){
						printf("\tFrame #%d MAC address change: New sender MAC %s, new receiver MAC %s\n", k+1, frameSomewhere(somewhereQueue(clientList[j].outgoingQueue, k), 0)->senderMAC, frameSomewhere(somewhereQueue(clientList[j].outgoingQueue, k), 0)->receiverMAC);
					}
					
				}	
				
			}	
		
				
			sendMessage(clientList, clientSize, staticSenderID, routeID, receiverID, messagePartSize, hops);
		}
	}
}


void findPath(Client* clientList, int clientSize, char path[], char senderID[], char receiverID[]){
	char route[255];
	findRoute(clientList, route, clientSize, senderID, receiverID);
	if(strcmp(route, receiverID) == 0){
		strcat(path, receiverID);
		return;
	}
	else{
		strcat(path, route);
		findPath(clientList, clientSize, path, route, receiverID);
	}
}





int main(int argc, char *argv[]) {

	int messageSize = atoi(argv[4]);
	char sendingSocket[255], listeningSocket[255];
	strcpy(sendingSocket, argv[5]);
	strcpy(sendingSocket, argv[6]);
	time_t current_time;
	struct tm * time_info;
	char timeString[messageSize];
	time( & current_time);
    time_info = localtime( & current_time);

    strftime(timeString, sizeof(timeString), "%F %H:%M:%S", time_info);
    char buff[255], buff2[255], buff3[255], buff4[255], buffCommand[255], routeID[255], originalMessage[1000], path[255], originalSenderID[255], originalReceiverID[255];
    int clientSize, commandNumber, originalFrameNumber;
	
	//Reading Files
	FILE *clientsFile, *routingFile, *commandsFile, *commandsFile2;
	clientsFile = fopen(argv[1], "r");
	routingFile = fopen(argv[2], "r");
	commandsFile = fopen(argv[3], "r");
	commandsFile2 = fopen(argv[3], "r");
	//Take clients number
    fscanf(clientsFile, "%s", buff);
	
	//Create space for clients
	Client *clientList = (Client*)malloc(atoi(buff) * sizeof(Client*));
	
	int i, j, k, l, g, z, t, x;
	clientSize = atoi(buff);
	

	for(i = 0; i < clientSize; i++){
		fscanf(clientsFile, "%s", clientList[i].ID);
		fscanf(clientsFile, "%s", clientList[i].ip_addr);
		fscanf(clientsFile, "%s", clientList[i].mac_addr);
	}
	

	

	//Adding neighbours
	for(i = 0; i< clientSize; i++){
		fscanf(routingFile, "%s", buff);
		strcpy(clientList[i].neigbours, buff);
		for(j = 0; j < 2*(clientSize-1)-1; j++){
			fscanf(routingFile, "%s", buff);
			strcat(clientList[i].neigbours, buff);
		}
		fscanf(routingFile, "%s", buff);
	}
	

	fscanf(commandsFile, "%s", buff);
	commandNumber = atoi(buff);
	
	fgets(buffCommand, 255, commandsFile2);

	

	for(z = 0; z < commandNumber; z++){
		fscanf(commandsFile, "%s", buff);
		if(strcmp("MESSAGE", buff) == 0){
			
			//Taking Sender and Receiver ID's
			fscanf(commandsFile, "%s %s", buff, buff2);
			char senderID[MAX], receiverID[MAX];
			strcpy(senderID, buff);
			strcpy(originalSenderID, buff);
			strcpy(receiverID, buff2);
			strcpy(originalReceiverID, buff2);
			//Find path
			
			strcpy(path, senderID);
			findPath(clientList, clientSize, path, senderID, receiverID);
			
		
			
			//Take the message
			fscanf(commandsFile, "%c", buff);
			fscanf(commandsFile, "%c", buff);
			fgets(buff2, 256, commandsFile);
			strtok(buff2, "#");
			strcpy(originalMessage, buff2);
			int messageLength =	strlen(buff2);
			int frameNumber = (messageLength / messageSize) + 1;
			originalFrameNumber = (messageLength / messageSize) + 1;
				
			//Create memory for texts
//			char** text = (char**)malloc(frameNumber * sizeof(char*));
//			for(i=0; i<frameNumber; i++){
//				*(text+i) = (char*)malloc(sizeof(char));
//			}
			char text[frameNumber][frameNumber];
			//Filling texts
			fseek(commandsFile, 15, SEEK_SET);
			
			for(i = 0; i< frameNumber; i++){
				fscanf(commandsFile, "%c", buff);
				strcpy(&text[i][0], buff);
				if(i == (frameNumber - 1)){
					for(k = 0; k < (messageLength - (i * messageSize))-1; k++){
						fscanf(commandsFile, "%c", buff);
						strcat(&text[i][k], buff);
					}
				}
				else{
						for(j = 1; j < messageSize; j++){
					fscanf(commandsFile, "%c", buff);
					strcat(&text[i][j], buff);				
				}
				}
			}
			
		
			//Create layers
			Layer physicalLayer, networkLayer, transportLayer; 
			Layer* appLayerList = (Layer*)malloc(frameNumber * sizeof(Layer));		
			for(i = 0; i < clientSize; i++){
				for(j = 0; j < clientSize; j++){
					if(strcmp(clientList[j].ID, receiverID) == 0){
						if(strcmp(clientList[i].ID, senderID) == 0){
							strcpy(physicalLayer.layerName, "PHYSICAL LAYER");
							strcpy(physicalLayer.senderMAC, clientList[i].mac_addr);	
			
							findRoute(clientList, routeID, clientSize, senderID, receiverID);
							for(l = 0; l < clientSize; l++){
								
								if(strcmp(routeID, clientList[l].ID) == 0){
									strcpy(physicalLayer.receiverMAC, clientList[l].mac_addr);
								}
							}
							strcpy(networkLayer.layerName, "NETWORK LAYER");
							strcpy(networkLayer.senderIP, clientList[i].ip_addr);
							strcpy(networkLayer.receiverIP, clientList[j].ip_addr);
							strcpy(transportLayer.layerName, "TRANSPORT LAYER");
							strcpy(transportLayer.senderPortNumber, "0706");
							strcpy(transportLayer.receiverPortNumber, "0607");
							for(k = 0; k < frameNumber; k++){
								strcpy(appLayerList[k].layerName, "APPLICATION LAYER");
								strcpy(appLayerList[k].senderID, senderID);
								strcpy(appLayerList[k].receiverID, receiverID);
								strncpy(appLayerList[k].messageChunk, &text[k][0], messageSize);
							
							}						
						}	
					}
				}	
			}
		
			// Create Frames
			Frame* frameList = (Frame*)malloc(frameNumber * sizeof(Frame));	
			for(i = 0; i < frameNumber; i++){
				frameInit(&frameList[i]);
				framePush(&frameList[i], appLayerList[i]);
				framePush(&frameList[i], transportLayer);
				framePush(&frameList[i], networkLayer);
				framePush(&frameList[i], physicalLayer);
			}
				
			//Printing output
			
			
			
			printf("---------------------------------------------------------------------------------------\n");
			fgets(buffCommand, 255, commandsFile2);
			printf("Command: %s", buffCommand);
			printf("---------------------------------------------------------------------------------------\n");
			char partOfMessage[messageSize];
			for(i = 0; i < frameNumber; i++){
				printf("Frame #%d\n", i+1);	
				printf("Sender MAC address: %s, Receiver MAC address: %s\n", physicalLayer.senderMAC, physicalLayer.receiverMAC);	
				printf("Sender IP address: %s, Receiver IP address: %s\n", networkLayer.senderIP, networkLayer.receiverIP);
				printf("Sender port number: %s, Receiver port number: %s\n", transportLayer.senderPortNumber, transportLayer.receiverPortNumber);	
				printf("Sender ID: %s, Receiver ID: %s\n", appLayerList[i].senderID, appLayerList[i].receiverID);
				strncpy(partOfMessage, appLayerList[i].messageChunk, messageSize);
				printf("Message chunk carried: %s\n", partOfMessage);
				printf("--------\n");
			}
			for(i = 0; i < clientSize; i++){
				if(strcmp(clientList[i].ID, senderID) == 0){
					clientList[i].outgoingQueue = createQueue(frameNumber);
					clientList[i].incomingQueue = createQueue(0);
					for(j = 0; j < frameNumber; j++){						
                    	Enqueue(clientList[i].outgoingQueue, frameList[j]);	
					}					
				}
			}
			
		}
		
		else if(strcmp("SHOW_FRAME_INFO", buff) == 0){		
			fscanf(commandsFile, "%s %s %s", buff2, buff3, buff4);
			printf("--------------------------------\n");
			fgets(buffCommand, 255, commandsFile2);
			printf("Command: %s", buffCommand);
			printf( "--------------------------------\n");		
			
			for(i = 0; i < clientSize; i++){
				if(strcmp(clientList[i].ID, buff2) == 0){
					if(strcmp(buff3, "out") == 0){
						if(atoi(buff4) <= clientList[i].outgoingQueue->frames->size){
							printf( "Current Frame #%s on the outgoing queue of client %s\n", buff4, buff2);				
							printf("Carried Message: \"%.20s\"\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->messageChunk);	
							printf("Layer 0 info: Sender ID: %s, Receiver ID: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->senderID, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->receiverID);
							printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 2)->senderPortNumber, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 2)->receiverPortNumber);
							printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 1)->senderIP, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 1)->receiverIP);
							printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 0)->senderMAC, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 0)->receiverMAC);
							for(x = 0; x < strlen(path); x++){
								char ss = path[x];
								char dd = buff2[0];
								if(ss == dd){
									printf("Number of hops so far: %d\n", x); 
								}	
							}
							
						}
						else{
							printf("No such Frame\n");
						}
					}
					else if(strcmp(buff3, "in") == 0){
						if(atoi(buff4) <= clientList[i].outgoingQueue->frames->size){
							printf("Current Frame #%s on the incoming queue of client %s\n", buff4, buff2);				
							printf("Carried Message: \"%.20s\"\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->messageChunk);
							printf("Layer 0 info: Sender ID: %s, Receiver ID: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->senderID, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 3)->receiverID);
							printf("Layer 1 info: Sender port number: %s, Receiver port number: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 2)->senderPortNumber, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 2)->receiverPortNumber);
							printf("Layer 2 info: Sender IP address: %s, Receiver IP address: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 1)->senderIP, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 1)->receiverIP);
							printf("Layer 3 info: Sender MAC address: %s, Receiver MAC address: %s\n", frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 0)->senderMAC, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, atoi(buff4)-1), 0)->receiverMAC);
							for(x = 0; x < strlen(path); x++){
								
								char ss = path[x];
								char dd = buff2[0];
								if(ss == dd){
									printf("Number of hops so far: %d\n", x); 
								}
								
							}
						}
						else{
							printf("No such Frame\n");
						}
					}
				}
			}
		
		}
		else if(strcmp("SHOW_Q_INFO", buff) == 0){
			fscanf(commandsFile, "%s %s", buff2, buff3);
			printf("--------------------------\n");
			fgets(buffCommand, 255, commandsFile2);
			printf("Command: %s", buffCommand);
			printf("--------------------------\n");		
			for(i = 0; i< clientSize; i++){
				if(strcmp(clientList[i].ID, buff2) == 0){
					if(strcmp(buff3, "out") == 0){
						printf("Client %s Outgoing Queue Status\n", buff2);
						printf("Current total number of frames: %d\n", clientList[i].outgoingQueue->size);
					}
					else if(strcmp(buff3, "in") == 0){
						printf("Client %s Incoming Queue Status\n", buff2);
						printf("Current total number of frames: %d\n", clientList[i].incomingQueue->size);
					}
				}
			}
		}
		else if(strcmp("SEND", buff) == 0){
			
			fscanf(commandsFile, "%s", buff2);
			printf("----------------\n");
			fgets(buffCommand, 255, commandsFile2);
			printf("Command: %s", buffCommand);
			printf("----------------\n");
			
			for(i = 0; i < clientSize; i++){
				if(strcmp(clientList[i].ID, buff2) == 0){
					
					for(k = 0; k < clientSize; k++){
						if(strcmp(clientList[k].ID, routeID) == 0){
						
							sendMessage(clientList, clientSize, clientList[i].ID, clientList[i].ID, frameSomewhere(somewhereQueue(clientList[i].outgoingQueue, 1-1), 3)->receiverID, messageSize, strlen(path)-1);							
						}
						
					}
					
					
				}
			}		
	
			
		}
		
		else if(strcmp("PRINT_LOG", buff) == 0){
			
			
			fscanf(commandsFile, "%s", buff2);
			printf("---------------------\n");
			fgets(buffCommand, 255, commandsFile2);
			printf("Command: %s", buffCommand);
			printf("---------------------\n");
			
			printf("Client %s Logs:\n", buff2);
			printf("--------------\n");
			
			char bb = path[0];
			char ss = path[strlen(path)-1];
			char dd = buff2[0];
			if(bb == dd){
				printf("Log Entry #1:\n"); 
				printf("Timestamp: %s\n", timeString); 
				printf("Message: %s\n", originalMessage); 
				printf("Number of frames: %d\n", originalFrameNumber); 
				for(x = 0; x < strlen(path); x++){
					char aa = path[x];
					char gg = buff2[0];
					if(aa == gg){
						printf("Number of hops: %d\n", x); 
					}
				}
				printf("Sender ID: %s\n", originalSenderID); 
				printf("Receiver ID: %s\n", originalReceiverID); 
				printf("Activity: Message Forwarded\n", x); 
				printf("Success: Yes\n"); 
			}	
			else if(bb == dd){
				printf("Log Entry #1:\n"); 
				printf("Timestamp: %s\n", timeString); 
				printf("Message: %s\n", originalMessage); 
				printf("Number of frames: %d\n", originalFrameNumber); 
				for(x = 0; x < strlen(path); x++){
					char aa = path[x];
					char gg = buff2[0];
					if(aa == gg){
						printf("Number of hops: %d\n", x); 
					}
				}
				printf("Sender ID: %s\n", originalSenderID); 
				printf("Receiver ID: %s\n", originalReceiverID); 
				printf("Activity: Message Received\n", x); 
				printf("Success: Yes\n"); 
			}
			else{
				printf("Log Entry #1:\n"); 
				printf("Timestamp: %s\n", timeString); 
				printf("Message: %s\n", originalMessage); 
				printf("Number of frames: %d\n", originalFrameNumber); 
				for(x = 0; x < strlen(path); x++){
					char aa = path[x];
					char gg = buff2[0];
					if(aa == gg){
						printf("Number of hops: %d\n", x); 
					}
				}
				printf("Sender ID: %s\n", originalSenderID); 
				printf("Receiver ID: %s\n", originalReceiverID); 
				printf("Activity: Message Received\n", x); 
				printf("Success: Yes\n"); 
				printf("--------------\n");
				printf("Log Entry #2:\n"); 
				printf("Timestamp: %s\n", timeString); 
				printf("Message: %s\n", originalMessage); 
				printf("Number of frames: %d\n", originalFrameNumber); 
				for(x = 0; x < strlen(path); x++){
					char aa = path[x];
					char gg = buff2[0];
					if(aa == gg){
						printf("Number of hops: %d\n", x); 
					}
				}
				printf("Sender ID: %s\n", originalSenderID); 
				printf("Receiver ID: %s\n", originalReceiverID); 
				printf("Activity: Message Forwarded\n", x); 
				printf("Success: Yes\n"); 
			}
			
			
		}
	
	
		
	}
	
	

	
	
	return 0;
}
