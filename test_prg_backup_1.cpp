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

using namespace std;

struct GameBoard {
  int columns;
  int rows;
  unsigned char player;
  char board[0];
};

struct GameBoard *gb;
void movePlayer(GameBoard*, int& ,char ,Map&);
int main()
{
  	srand(time(NULL));
  	fstream myfile;
  	myfile.open("mymap.txt");
	string mapOrig="";
	string numOfGold;
	string line;
	int rows=0, columns=0;	
	
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
	int mapSize = mapOrig.length();
	
	const char *theMine= mapOrig.c_str();
	// create shared memory
	int fd = shm_open("/map_shm",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);	
	ftruncate(fd,mapSize);	
	gb = (GameBoard*)mmap(0,mapSize,PROT_WRITE,MAP_SHARED,fd,0);	
	gb->rows = rows;
	gb->columns = columns;
	gb->player = G_PLR0;

  	const char* ptr=theMine;
  	char* mp=gb->board;
  	//Convert the ASCII bytes into bit fields drawn from goldchase.h
 	while(*ptr!='\0')
  	{
   		if(*ptr==' ')      *mp=0;
    		else if(*ptr=='*') *mp=G_WALL; //A wall
    		++ptr;
    		++mp;
  	}
	
	
	// Assign Gold
		// Assign Fools gold
		int pos=0;		
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
		gb->board[pos] = G_GOLD;

	// Assign just 1 Player
		
			int playerPos = rand()%mapSize;
			while(gb->board[playerPos]!=0)
			{
				playerPos = rand()%mapSize;
			}
			gb->board[playerPos] = gb->player;
		
	
	// Draws the map
		Map goldMine(gb->board,gb->rows,gb->columns);
	 	goldMine.postNotice("This is a notice");
  	
	
	// Handle Keys			
	int a = 0;
	//cout<<G_WALL<<"\n";
	//cout<<G_FOOL<<"\n";
	//cout<<G_GOLD<<"\n";
	//cout<<gb->player<<"\n";		
	while((a=goldMine.getKey())!='Q')
	{
		int curIndex = playerPos;
		int curCol = playerPos%gb->columns;
		int curRow = playerPos/gb->columns;
		int prevEltCol=-1,prevEltRow=-1,nextEltCol=-1,nextEltRow=-1,downEltCol=-1,downEltRow=-1,upEltCol=-1,upEltRow=-1;
		int upEltPos=-1,downEltPos=-1;		
		switch(a)
		{
			case 'h': // left	
				//gb[playerPos-1] & G_FOOL : will return non zero, in all other andings 0 is returned
				//: only if gb[playerPos-1] == G_FOOL		
				movePlayer(gb,playerPos,'h', goldMine);				
				/*prevEltRow = (playerPos-1)/gb->columns;
				if( curRow==prevEltRow &&  (gb->board[playerPos-1]==0|| gb->board[playerPos-1] & G_FOOL || gb->board[playerPos-1] & G_GOLD))
				{				
					unsigned char x = gb->board[playerPos];
					if(!(x ^ (gb->player | G_FOOL)) || !(x ^ (gb->player | G_GOLD))) 
					{												
						gb->board[playerPos]=!(x ^ (gb->player|G_FOOL))?G_FOOL:G_GOLD;						
					} 
					else 
					{						
						gb->board[playerPos]=0;					
					}
					playerPos-=1;
					if(gb->board[playerPos]==0) 
					{
						gb->board[playerPos]=gb->player;
					} 
					else if(gb->board[playerPos] & G_FOOL ||  gb->board[playerPos] & G_GOLD) 
					{
						gb->board[playerPos]= (gb->board[playerPos] & G_FOOL)?(G_FOOL|gb->player):(G_GOLD|gb->player);
					}
					goldMine.drawMap();
				}*/	
				break;
			case 'l': // right
				movePlayer(gb,playerPos,'l', goldMine);
				/*				
				nextEltCol = (playerPos+1)%gb->columns;
				nextEltRow = (playerPos+1)/gb->columns;			
				if(curRow == nextEltRow && (gb->board[playerPos+1]==0 || gb->board[playerPos+1] & G_FOOL || gb->board[playerPos+1] & G_GOLD))
				{									
					unsigned char x = gb->board[playerPos];					
					if(!(x ^ (gb->player|G_FOOL)) || !(x ^ (gb->player|G_GOLD))) 
					{					
						gb->board[playerPos]=(x ^ (gb->player|G_FOOL))?G_FOOL:G_GOLD;						
					} 
					else 
					{
						gb->board[playerPos]=0;					
					}
					playerPos+=1;
					if(gb->board[playerPos]==0) 
					{
						gb->board[playerPos]=gb->player;
					} 
					else if(gb->board[playerPos] & G_FOOL || gb->board[playerPos] & G_GOLD) 
					{
						gb->board[playerPos]= (gb->board[playerPos] & G_FOOL)?(G_FOOL|gb->player):(G_GOLD|gb->player);
					}
					goldMine.drawMap();
				}*/					
				break;		
			case 'j': // down
				movePlayer(gb,playerPos,'j', goldMine);
				/*				
				downEltCol = (playerPos+gb->columns)%gb->columns;
				downEltPos = (playerPos+gb->columns);
								
				if(downEltPos<=mapSize && (gb->board[playerPos+gb->columns]==0 || gb->board[playerPos+gb->columns] & G_FOOL|| gb->board[playerPos+gb->columns] & G_GOLD))
				{				
					unsigned char x = gb->board[playerPos];					
					if(!(x ^ (gb->player|G_FOOL)) || !(x ^ (gb->player|G_GOLD))) 
					{					
						gb->board[playerPos]=(x ^ (gb->player|G_FOOL))?G_FOOL:G_GOLD;						
					} 
					else 
					{
						gb->board[playerPos]=0;					
					}
					playerPos+=gb->columns;
					if(gb->board[playerPos]==0) 
					{
						gb->board[playerPos]=gb->player;
					} 
					else if(gb->board[playerPos] & G_FOOL || gb->board[playerPos] & G_GOLD) 
					{
						gb->board[playerPos]= (gb->board[playerPos] & G_FOOL)?(G_FOOL|gb->player):(G_GOLD|gb->player);
					}
					goldMine.drawMap();
				}*/						
				break;	
			case 'k': // up
				movePlayer(gb,playerPos,'k', goldMine);
				//upEltCol = (playerPos-columns)%columns;
				/*				
				upEltPos = playerPos-gb->columns;
				
				if(upEltPos>=0 && (gb->board[playerPos-gb->columns]==0|| gb->board[playerPos-gb->columns] & G_FOOL || gb->board[playerPos-gb->columns] & G_GOLD))
				{					
					unsigned char x = gb->board[playerPos];					
					if(!(x ^ (gb->player|G_FOOL)) || !(x ^ (gb->player|G_GOLD))) 
					{					
						gb->board[playerPos]=(x ^ (gb->player|G_FOOL))?G_FOOL:G_GOLD;						
					} 
					else 
					{
						gb->board[playerPos]=0;					
					}
					playerPos-=gb->columns;
					if(gb->board[playerPos]==0) 
					{
						gb->board[playerPos]=gb->player;
					} 
					else if(gb->board[playerPos] & G_FOOL || gb->board[playerPos] & G_GOLD) 
					{
						gb->board[playerPos]= (gb->board[playerPos] & G_FOOL)?(G_FOOL|gb->player):(G_GOLD|gb->player);
					}
					goldMine.drawMap();						
				}
				*/				
				break;		
		}	
	}
}

void movePlayer(GameBoard *gbp, int &playerPos,char key, Map &goldMine)
{
	int newPlayerPos=-1,curRow = playerPos/gbp->columns,prevEltRow = -1, nextEltRow = -1;
	int mapSize = -1, upEltPos = -1, downEltPos=-1;	
	bool boundaryCondition= false;
	//cout<<"\t\t\tinside Move player\n";
	switch(key)
	{
		case 'h':// left
			//cout<<"\t\t\tinside left\n";
			newPlayerPos = playerPos-1;
			prevEltRow = (newPlayerPos)/gbp->columns;	
			boundaryCondition = (curRow==prevEltRow);			
			break;
		case 'l':// right
			newPlayerPos = playerPos+1;
			//cout<<"\t\t\tinside left\n";
			nextEltRow = (newPlayerPos)/gbp->columns;
			boundaryCondition = (curRow == nextEltRow);
			break;
		case 'j':// down
			newPlayerPos = playerPos+gbp->columns;

			mapSize = gbp->rows * gbp->columns;			
			downEltPos = playerPos+gb->columns;			
			boundaryCondition = (downEltPos<=mapSize);			
			break;
		case 'k':// up
			newPlayerPos = playerPos-gbp->columns;
			
		        upEltPos = playerPos-gb->columns;		
			boundaryCondition = (upEltPos>=0);			
			break;	
	}
	
				
	if(boundaryCondition && (gbp->board[newPlayerPos]==0|| gbp->board[newPlayerPos] & G_FOOL || gbp->board[newPlayerPos] & G_GOLD))
	{					
		unsigned char x = gbp->board[playerPos];					
		if(!(x ^ (gbp->player|G_FOOL)) || !(x ^ (gbp->player|G_GOLD))) 
		{					
			gbp->board[playerPos]=(x ^ (gbp->player|G_FOOL))?G_FOOL:G_GOLD;						
		} 
		else 
		{
			gbp->board[playerPos]=0;					
		}
		playerPos = newPlayerPos;
		if(gbp->board[playerPos]==0) 
		{
			gbp->board[playerPos]=gbp->player;
		} 
		else if(gbp->board[playerPos] & G_FOOL || gbp->board[playerPos] & G_GOLD) 
		{
			gbp->board[playerPos]= (gbp->board[playerPos] & G_FOOL)?(G_FOOL|gbp->player):(G_GOLD|gbp->player);
		}
		goldMine.drawMap();						
	}		
}
