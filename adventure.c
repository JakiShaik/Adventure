#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
struct room {
    char name[20];
    char type[20];
    int numOutboundConnections;
    char outBoundConnections[6][20];
};
struct room* selectedRooms[7];
int starting_index , ending_index;
void readDataOfRooms();
//Read data of rooms from directory to play game.
int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

//file creation time
struct stat getFileCreationTime(char *filePath)
{
    struct stat attrib;
    stat(filePath, &attrib);
	return attrib;
//    char date[10];
//    strftime(date, 10, "%d-%m-%y", gmtime(&(attrib.st_ctime)));
//    printf("%s %s\n", filePath, date);

}

//checkig latest directory
int isLatest(char *AfilePath,char *BfilePath)
{
struct stat a=getFileCreationTime(AfilePath);
struct stat b=getFileCreationTime(BfilePath);
    char datea[50];
    strftime(datea,50,"%Y-%m-%d-%h-%M-%S", gmtime(&(a.st_ctime)));
    char dateb[50];
    strftime(dateb,50,"%Y-%m-%d-%h-%M-%S", gmtime(&(b.st_ctime)));
	return strcmp(datea,dateb);
}

//reading data from files
void readDataOfRooms()
{
  char directory_path[50] ;

    DIR * d = opendir("."); // open the current directory

    if(d==NULL) return; 

    struct dirent * dir; 
    while ((dir = readdir(d)) != NULL) // if directory is not empty
	{
	  //getting all directories aprt from current directory and previous directory
	  if(isDirectory(dir->d_name)&&strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
		{
//			char pathA[50]=readdir(d)->d_name;
		  //printf("##### dirnsme is %s\n ",dir->d_name);
		  strcpy(directory_path , dir->d_name);
			while((dir = readdir(d)) != NULL) // if we were able to read something from the directory
			{

				if(isDirectory(dir->d_name)&&strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"shaikj.buildrooms.")!=0)
				{
					char pathB[50];
					strcpy(pathB,dir->d_name);
					//printf("PathB is %s\n",pathB);
					if(isLatest(directory_path,pathB)<0)
					{
					  //printf("dirname is %s and pathB is %s \n");
						strcpy(directory_path , pathB);
					}
				}
			}
		}
	}
	if(strlen(directory_path)==0)
	{
	printf("No rooms directory avaliable");
	exit(0);
	}
	//print latest directory
	d = opendir(directory_path); // open the path
    if(d==NULL) return; // if was not able return

    int j=0;
    //reading file contents and storing them in rooms structure 
    while ((dir = readdir(d)) != NULL) 
    {
      if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
      {
      selectedRooms[j]= (struct room *) malloc(sizeof(struct room));
      char file_path[50];
      strcpy(file_path,directory_path);
      strcat(file_path,"/");
      strcat(file_path,dir->d_name);
      FILE *fp = fopen(file_path,"r");
      char data[80];
      fgets(data,80,fp);//reading data using fgets
      int i=0,k=0;
      while(data[i]!=':')//splitting using ':'
      i++;
      for(i=i+2;data[i]!='\0';i++)
      {
         selectedRooms[j]->name[k++]=data[i];
      }
      selectedRooms[j]->name[k]='\0';
     selectedRooms[j]->numOutboundConnections = 0;
     while(fgetc(fp) == 'C')//checking for connections
     {
         fseek(fp,-1,SEEK_CUR);
         fgets(data,80,fp);
         i=0,k=0;
         while(data[i]!=':')
         i++;
      for(i=i+2;data[i]!='\0';i++)
      {
         selectedRooms[j]->outBoundConnections[selectedRooms[j]->numOutboundConnections][k++]=data[i];
      }
      selectedRooms[j]->outBoundConnections[selectedRooms[j]->numOutboundConnections][k]='\0';
      selectedRooms[j]->numOutboundConnections++;
    }
    fgets(data,80,fp);

         i=0,k=0;
         while(data[i]!=':')
         i++;
      for(i=i+2;data[i]!='\0';i++)
      {
          selectedRooms[j]->type[k++]=data[i];
      }
      selectedRooms[j]->type[k]='\0';//getting start room and end room
      if(selectedRooms[j]->type[0] == 'S')
      {
          starting_index = j;
      }
      if(selectedRooms[j]->type[0] == 'E')
      {
          ending_index = j;
      }

      j++;
      }

    }
    closedir(d); // finally close the directory
}

bool delta=false;
//writing time to stdout
void* writeTime(void* pString)
{
	while(true)
	{
		while(!delta);
		time_t timer;
		char buffer[26];
		char buffer1[26];
		struct tm* tm_info;

		time(&timer);
		tm_info = localtime(&timer);

		strftime(buffer, 26, "%-I:%M%p, %A, ", tm_info);
		strftime(buffer1, 26, "%B %d,%Y", tm_info);
		strcat(buffer,buffer1);

		FILE *fptr;

		fptr = fopen("currentTime.txt", "w");
		if(fptr == NULL)
		{
			printf("Error!");
			exit(1);
		}

		fprintf(fptr,"%s", buffer);
		fclose(fptr);
		delta=false;
	}
    return 0;
}
//writing time to file
void printTime()
{
		printf("\n");
		FILE *fptr;
		char c;
		fptr = fopen("currentTime.txt", "r");
		if(fptr == NULL)
		{
			printf("Error!");
			exit(1);
		}

		while ((c = getc(fptr)) != EOF)
			putchar(c);
		fclose(fptr);
		printf("\n");

}

//main thread which will read data from structures and give it to stdout
void* mainThread(void* pString)
{
    readDataOfRooms();
    char current_move[50];
    char path[10000][20];
    int no_of_moves = 0;
    do
    {
        printf("\nCURRENT LOCATION: %s",selectedRooms[starting_index]->name);
        printf("\nPOSSIBLE CONNECTIONS:\n ");
        int i,j;
        for(i=0;i<selectedRooms[starting_index]->numOutboundConnections;i++)
        {

            printf("\t%s", selectedRooms[starting_index]->outBoundConnections[i]);
	    //if(i != selectedRooms[starting_index]->numOutboundConnections -1) {printf(",");} //if mid connection print comma
	    //else printf("."); //if last outbound connection print fullstop
	}
        printf("\nWHERE TO? >");
        fgets(current_move,50,stdin); //take input for next move
	

        if(strcmp(current_move , "time\n")==0)
        {
            	delta=true;
		while(delta);
		printTime(); // if user enters time command, call printTime method and print current time
        }
        else
        {
	  bool flag=false; // set flag to readin from user and return next move
            for(i=0;i<selectedRooms[starting_index]->numOutboundConnections;i++)
            {
	      if( strcmp(selectedRooms[starting_index]->outBoundConnections[i] , current_move) == 0)
                {
                    flag = true;
                    strcpy(path[no_of_moves] , current_move);
                    for(j = 0; j<7; j++)
                    {
		      if( strcmp( current_move , selectedRooms[j]->name) == 0)
                        {
                            starting_index = j;
                        }
                    }
                    no_of_moves++;
                    if(starting_index == ending_index)
                    {
                        printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEP(S). YOUR PATH TO VICTORY WAS:\n",no_of_moves);
                        for(j = 0; j <no_of_moves ; j++)
                        {
                            printf("\t%s",path[j]);
                        }
                        exit(0);
                    }
                }
            }
            if(flag==false)
            {
                printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
            }
        }
    }while(1);


}

//Mutex formation, one thread for time and one thread for game
int main(void)
{
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, writeTime, "Time");
    pthread_create(&thread2, NULL, mainThread, "Game");

    pthread_join(thread2, NULL);

    return 0;
}

