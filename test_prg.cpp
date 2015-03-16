#include "goldchase.h"
#include "Map.h"
#include "Screen.h"
#include<fstream>
#include<iostream>
#include<stdlib.h>   // for atoi
#include<string>
#include<sys/mman.h>
#include<time.h>
#include<vector>
#include<sys/stat.h>
#include<fcntl.h>
#include<semaphore.h>

using namespace std;

struct GameBoard {
   int columns;
   int rows;
   unsigned char players;
   char board[0];
};

struct GameBoard *gb;
void movePlayer(GameBoard*, int& ,char ,Map&, bool&, unsigned char&, bool &);
int main()
{
   //cout<<endl<<"Inside Gold Chase!!"; 
   srand(time(NULL));
   //ifstream myfile;
   string mapOrig="";
   string numOfGold;
   string line;
   int rows=0, columns=0;	
   bool goldFound = false, isWon = false;		
   unsigned char currPlayer=0;
   int mapSize = 0;
   int fd;// = -1;

   sem_t* sem_ptr = sem_open("/mySemaphore", O_RDWR,S_IRUSR|S_IWUSR,1);
   // create semaphore
   if(sem_ptr == SEM_FAILED)
   {

      ifstream myfile("mymap.txt");
      if(myfile.is_open())
      {		
         getline(myfile,numOfGold);

         while(getline(myfile,line))
         {
            mapOrig= mapOrig+line;
            rows++;
            if(columns == 0)
            {
               columns = line.length();			
            }
         }	
      } 
      myfile.close();		       
      int numOfGold_num = atoi(numOfGold.c_str());
      //mapOrig+='\0';       
      mapSize = mapOrig.length();
      const char *theMine= mapOrig.c_str();

      // End Assign players

      const char* ptr=theMine;
      sem_ptr = sem_open("/mySemaphore", O_RDWR|O_CREAT,S_IRUSR|S_IWUSR,1);
      sem_wait(sem_ptr);
      fd = shm_open("/map_shm",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      ftruncate(fd,mapSize);
      
      gb = (GameBoard*)mmap(0,mapSize ,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);	
      gb->rows = rows;
      gb->columns = columns;       
      currPlayer = G_PLR0;	
      sem_post(sem_ptr); 

      char* mp=gb->board;       
      //Convert the ASCII bytes into bit fields drawn from goldchase.h
      int k = 0;
      while(k<mapSize)
      {
         if(*ptr==' ')      *mp=0;
         else if(*ptr=='*') *mp=G_WALL; //A wall
         ++k; 
         ++ptr;
         ++mp;

      }

      int pos=0;		
      sem_wait(sem_ptr);	
      for(int i=0;i<numOfGold_num-1;i++)
      {      		
         pos = rand()%mapSize;
         while(gb->board[pos]!=0)
         {
            pos = rand()%mapSize;
         }
         gb->board[pos] = G_FOOL;
      }
      pos = rand()%mapSize;
      while(gb->board[pos]!=0)
      {
         pos = rand()%mapSize;

      }
      if(numOfGold_num>0)
         gb->board[pos] = G_GOLD;
      sem_post(sem_ptr);
   }
   else
   { 
      sem_wait(sem_ptr); 
      fd = shm_open("/map_shm",O_RDWR , S_IRUSR | S_IWUSR);;
      read(fd,&columns,sizeof(int));
      read(fd,&rows,sizeof(int));	
      mapSize= columns*rows;	      	
      ftruncate(fd,mapSize);
      gb = (GameBoard*)mmap(0,mapSize+sizeof(GameBoard),PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);	
      gb->rows = rows;
      gb->columns = columns;
      sem_post(sem_ptr);  
   }
   sem_wait(sem_ptr);
   // Assign Players
   if((gb->players & G_PLR0)!= G_PLR0)
   {
      gb->players |= G_PLR0;
      currPlayer = G_PLR0;	
   }
   else if((gb->players & G_PLR1)!= G_PLR1)
   {
      gb->players |= G_PLR1;
      currPlayer = G_PLR1;
   }
   else if((gb->players & G_PLR2)!= G_PLR2)
   {
      gb->players |= G_PLR2;
      currPlayer = G_PLR2;
   }
   else if((gb->players & G_PLR3)!= G_PLR3)
   {
      gb->players |= G_PLR3;
      currPlayer = G_PLR3;
   }
   else if((gb->players & G_PLR4) != G_PLR4)
   {
      gb->players |= G_PLR4;
      currPlayer = G_PLR4;
   }
   else
   {
      cout<<"Game is full. Please wait!"<<endl;
      sem_post(sem_ptr);
      exit(0);
   }

   // Assign current Player

   int playerPos = rand()%mapSize;
   while(gb->board[playerPos]!=0)
   {
      playerPos = rand()%mapSize;
   }
   gb->board[playerPos] = currPlayer;
   sem_post(sem_ptr);

   // Draws the map
   sem_wait(sem_ptr);
   Map goldMine(gb->board,gb->rows,gb->columns);	
   goldMine.postNotice("This is a notice");
   sem_post(sem_ptr);
   // Handle Keys			
   int a = goldMine.getKey();		
   while((a)!='Q')
   {	
      if(isWon)
      {
         break;	
      }      
      sem_wait(sem_ptr);

      int curIndex = playerPos;
      int curCol = playerPos%gb->columns;
      int curRow = playerPos/gb->columns;
      int prevEltCol=-1,prevEltRow=-1,nextEltCol=-1,nextEltRow=-1,downEltCol=-1,downEltRow=-1,upEltCol=-1,upEltRow=-1;
      int upEltPos=-1,downEltPos=-1;		
      switch(a)
      {
         case 'h': // left			
            //cout<<endl<<"in H";
	    movePlayer(gb,playerPos,'h', goldMine, goldFound, currPlayer,isWon);				
            break;

         case 'l': // right
            //cout<<endl<<"in L";
            movePlayer(gb,playerPos,'l', goldMine, goldFound,currPlayer,isWon);				
            break;

         case 'j': // down
            movePlayer(gb,playerPos,'j', goldMine,goldFound,currPlayer,isWon);						
            break;	

         case 'k': // up
            movePlayer(gb,playerPos,'k', goldMine, goldFound,currPlayer,isWon);				
            break;		
      }	
      a = goldMine.getKey();
      sem_post(sem_ptr);
   }
   if((a)=='Q')
   {
      //cout<<endl<<"\t\tPlayers before : "<<int(gb->players);
      gb->players&=~currPlayer;
      //cout<<endl<<"\t\tPlayers after : "<<int(gb->players);
      currPlayer = 0;
   }
   if(gb->players == 0)
   {
      //sem_close(my_sem_ptr);
      sem_unlink("/mySemaphore");
      shm_unlink("/map_shm");

   }
}

void movePlayer(GameBoard *gbp, int &playerPos,char key, Map &goldMine, bool &goldFound,unsigned char& currPlayer,bool &isWon)
{
   //cout<<endl<<"In Move player";
   int newPlayerPos=-1,curRow = playerPos/gbp->columns,prevEltRow = -1, nextEltRow = -1;
   int mapSize = -1, upEltPos = -1, downEltPos=-1;	
   bool boundaryCondition= false;
   
   switch(key)
   {
      case 'h':// left
         newPlayerPos = playerPos-1;
         prevEltRow = (newPlayerPos)/gbp->columns;	
         boundaryCondition = (curRow==prevEltRow);			
         break;
      case 'l':// right
         newPlayerPos = playerPos+1;
         nextEltRow = (newPlayerPos)/gbp->columns;
         boundaryCondition = (curRow == nextEltRow);
         break;
      case 'j':// down
         newPlayerPos = playerPos+gbp->columns;

         mapSize = gbp->rows * gbp->columns;			
         downEltPos = playerPos+gbp->columns;			
         boundaryCondition = (downEltPos<=mapSize);			
         break;
      case 'k':// up
         newPlayerPos = playerPos-gbp->columns;
         upEltPos = playerPos-gbp->columns;		
         boundaryCondition = (upEltPos>=0);			
         break;	
   }

   if(boundaryCondition && (gbp->board[newPlayerPos]==0|| gbp->board[newPlayerPos] & G_FOOL || gbp->board[newPlayerPos] & G_GOLD))
   {							
      unsigned char x = gbp->board[playerPos];					
      if(!(x ^ (currPlayer|G_FOOL)) || !(x ^ (currPlayer|G_GOLD))) 
      {					
         gbp->board[playerPos]=(!(x ^ (currPlayer|G_FOOL)))?G_FOOL:G_GOLD;				
      } 
      else 
      {
         gbp->board[playerPos]=0;					
      }
      
      //cout<<endl<<"In Move Player";
      
      //cout<<endl<<"Old Player Pos : "<< playerPos; 	      
      playerPos = newPlayerPos;
      //cout<<endl<<"New Player Pos : "<< playerPos;

      if(gbp->board[playerPos]==0) 
      {
         gbp->board[playerPos]= currPlayer;
	 //cout<<endl<<"Current Player : "<<(int)currPlayer;
	 //cout<<endl<<"Moved Player";
      } 
      else if(gbp->board[playerPos] & G_FOOL || gbp->board[playerPos] & G_GOLD) 
      {			
         goldMine.drawMap();			
         if(gbp->board[playerPos] & G_FOOL)			
         {
            gbp->board[playerPos]= G_FOOL|currPlayer;
            goldMine.postNotice("This is Fool's gold!");					
         }
         else if(gbp->board[playerPos] & G_GOLD)			
         {
            gbp->board[playerPos]= G_GOLD|currPlayer;
            goldMine.postNotice("Congratulations!! Gold found.");
            goldFound = true;
         }			
      }
   }

   else if(!boundaryCondition && goldFound)
   {
      unsigned char x = gbp->board[playerPos];
      if(!(x ^ (currPlayer|G_FOOL)) || !(x ^ (currPlayer|G_GOLD))) 
      {					
         gbp->board[playerPos]=(!(x ^ (currPlayer|G_FOOL)))?G_FOOL:G_GOLD;			
      }					
      gbp->board[playerPos]=0;
      gbp->players&=~currPlayer;
      currPlayer = 0;

      goldMine.postNotice("Congratulations!! you won the game.");
      isWon=true;
      //sem_post(sem_ptr);		
   }
   goldMine.drawMap();									
}
