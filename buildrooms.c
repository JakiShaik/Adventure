#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//create struct for room
struct room {
    char name[20];
    char *type;
    int numOutboundConnections;
    char outBoundConnections[6][20];
};

//array of pointers to 7 rooms that will randomly be selected
struct room* selectedRooms[7];

//hardcoded values of all room names
char* allRooms[10] = {
    "kelly",
    "johnson",
    "valley",
    "milam",
    "bexell",
    "dixon",
    "kearny",
    "lasells",
    "mu",
    "illc"
};

void populateRooms();
bool IsGraphFull();
void AddRandomConnection();
void ConnectRoom(struct room*, struct room*);
struct room* GetRandomRoom();
bool CanAddConnectionFrom(struct room*);
bool IsSameRoom (struct room* , struct room*);
bool ConnectionAlreadyExists(struct room*, struct room*);

int main() {
    time_t t;
    srand((unsigned) time(&t));
    populateRooms(); //assigning random names and random mid,start and end rooms 
    // For creating a complete graph with random connections
    while (IsGraphFull() == false)
    {
        AddRandomConnection();
    }
    char directory_path[50] ;
    sprintf(directory_path,"shaikj.rooms.%d",getpid());
    mkdir(directory_path, 0755); //make directory and set directory permissions to make it accessible
    int i,j;
    for(i=0;i<7;i++)
    {
        char file_path[50];
        strcpy(file_path,directory_path);
        strcat(file_path,"/");
        strcat(file_path,selectedRooms[i]->name);
        //strcat(file_path,"_room");
        FILE *fp = NULL;
        fp = fopen(file_path , "w"); // open file with write permission
        fprintf(fp, "ROOM NAME: %s\n",selectedRooms[i]->name);
        for(j=0;j<selectedRooms[i]->numOutboundConnections;j++){
            fprintf(fp,"CONNECTION %d: %s\n",j+1,selectedRooms[i]->outBoundConnections[j]);
        }
        fprintf(fp,"ROOM TYPE: %s\n",selectedRooms[i]->type);
        fclose(fp);
    }
    return 0;
}


// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()
{
    int i;
    for( i=0; i<7; i++) {
        if(selectedRooms[i]->numOutboundConnections < 3)
        {
            return false;
        }

    }
    return true;
}


// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()
{
    struct room *A;
    struct room *B;

    while(true)
    {
      A = GetRandomRoom(); //call the function and fetch a random room

        if (CanAddConnectionFrom(A) == true)
        break;
    }

    do
    {
        B = GetRandomRoom();
    }
    while((CanAddConnectionFrom(B) == false) || (IsSameRoom(A, B) == true) || (ConnectionAlreadyExists(A, B) == true)); //check if connection can be added or if it's the same room

    ConnectRoom(A, B);
    ConnectRoom(B, A);
}
// Returns a random Room
struct room* GetRandomRoom()
{
    int randomIndex = rand() % 7;
    return selectedRooms[randomIndex];
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct room *x)
{
    if(x->numOutboundConnections <= 6 )
    {
        return true;
    }
    return false;
}
  // Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct room *x, struct room *y)
{
    int i;
    for(i=0;i < x->numOutboundConnections ;i++)
    {
        if(strcmp(x->outBoundConnections[i] , y->name) == 0)
        {
            return true;
        }
    }
    return false;
}

  // Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct room *x, struct room *y)
{
    if(strcmp(x->name , y->name) == 0)
    {
        return true;
    }
    return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room *x, struct room *y)
{
    //x->outBoundConnections[x->numOutboundConnections] = y;
    strcpy(x->outBoundConnections[x->numOutboundConnections] , y->name );
    x->numOutboundConnections++;

}

//initialize the array of structs of the selected rooms
void populateRooms() {
    int randArray[7], i=0, length = -1,j;
    while(i<7)
    {
        int flag = 0;
        int randomIndex = rand() % 10;
        for(j=length;j>=0;j--)
        {
            if(randArray[j] == randomIndex)
            {
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            randArray[++length] = randomIndex;
            selectedRooms[i]= (struct room *) malloc(sizeof(struct room));
            strcpy(selectedRooms[i]->name, allRooms[randomIndex]);
            selectedRooms[i]->numOutboundConnections=0;
            i++;
        }
    }
    int start_room_index = rand() % 7;
    int end_room_index;
    do{
        end_room_index = rand() % 7;

    }while(start_room_index == end_room_index);
    selectedRooms[start_room_index]->type = "START_ROOM"; //set whether room is start room, mid room or end room
    selectedRooms[end_room_index]->type = "END_ROOM";
    for( i=0; i<7; i++) {
            if(i!=start_room_index && i!=end_room_index)
            selectedRooms[i]->type = "MID_ROOM";
        }
    }
















