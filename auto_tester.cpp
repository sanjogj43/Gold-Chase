#include<iostream>
#include<fstream>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include"goldchase.h"

using namespace std;
struct Child {
  int PID;
  int toGame; 
  int fromGame; 
  int rows;
  int cols;
  char *mapmem;
};

class TestCase {
  protected:
    Child firstChild;
    Child secondChild;
  public:
    // 
    

    TestCase(const char* maptext)
    {
      int result = mkfifo("mymap.txt",S_IWUSR|S_IRUSR);
      int driverToGame1[2];
      int gameToDriver1[2];
      int driverToGame2[2];
      int gameToDriver2[2]; 
      
      pipe(driverToGame1);//3,4
      pipe(gameToDriver1);//5,6
      pipe(driverToGame2);//3,4
      pipe(gameToDriver2);//5,6 
      
      firstChild.PID = fork();
      if(firstChild.PID == 0)
      {
         dup2(driverToGame1[0], 20); // 20 to read from instead of 3 
         dup2(gameToDriver1[1], 21); // 21 to write to instead of 6

         close(driverToGame1[0]); // 3 to read from; close all of em
         close(driverToGame1[1]); // 4 to write to
         close(gameToDriver1[0]); // 5 to read from
         close(gameToDriver1[1]); // 6 to write to

         execlp("./test_prg","./test_prg",NULL);
      }
      else
      {
      	  // parent
          close(driverToGame1[0]); 
          close(gameToDriver1[1]);
          firstChild.toGame=driverToGame1[1];
          firstChild.fromGame=gameToDriver1[0];    
        //  run_map_constructor(firstChild);
          int fd=open("mymap.txt",O_WRONLY,S_IRUSR | S_IWUSR); //up mymap.txt O_WRONLY          
          write(fd, maptext, strlen(maptext));
          close(fd);

	 run_map_constructor(firstChild);
      }	
      
      secondChild.PID = fork();
      if(secondChild.PID == 0)
      {
         dup2(driverToGame2[0], 20); // 20 to read from instead of 3 
         dup2(gameToDriver2[1], 21); // 21 to write to instead of 6

         close(driverToGame2[0]); // 3 to read from; close all of em
         close(driverToGame2[1]); // 4 to write to
         close(gameToDriver2[0]); // 5 to read from
         close(gameToDriver2[1]); // 6 to write to

         execlp("./test_prg","./test_prg",NULL);
      }
      else
      {
          close(driverToGame2[0]); // close the read poit  
          close(gameToDriver2[1]);
          secondChild.toGame=driverToGame2[1];
          secondChild.fromGame=gameToDriver2[0];
	  run_map_constructor(secondChild); 
      }
      unlink("mymap.txt");
    }
    virtual void run_test()=0;//pure virtual function

    //read the three writes coming from child's map ctor
    void run_map_constructor(Child &chld)
    {
      read(chld.fromGame,&chld.rows,sizeof(chld.rows));
      read(chld.fromGame,&chld.cols,sizeof(chld.cols));
      chld.mapmem= new char[chld.rows*chld.cols]; 
      read(chld.fromGame,chld.mapmem,chld.rows*chld.cols);
    }

    void write_character_getKey(Child &chld,char c,int &callsPostNotice,int &callsDrawMap)
    {
    	write(chld.toGame, &c,sizeof(char));
        read(chld.fromGame,&callsPostNotice,sizeof(callsPostNotice));
  	read(chld.fromGame,&callsDrawMap,sizeof(callsDrawMap));
	chld.mapmem= new char[chld.rows*firstChild.cols];
  	read(chld.fromGame, chld.mapmem,chld.rows*chld.cols);
    }

    void close_game_descriptors()
    {
    	close(this->firstChild.toGame);
	close(this->secondChild.toGame);
	close(this->firstChild.fromGame);
	close(this->secondChild.fromGame);
    } 	
    
    void display_test_results(int test_case,int check)
    {
     	switch(test_case)
	{
		case 1:
			cout<<endl<<"Test case 1 : Player should not move on to the wall.";
			break;
		case 2:
			cout<<endl<<"Test case 2 : Player should not move off the edge.";
			break;
		case 3:
			cout<<endl<<"Test case 3 : Player should not on to other player.";
			break;
		case 4:
			cout<<endl<<"Test case 4 : Player should move in valid case.";
			if(check==0) check = -1;
			else	check = 0;
							
			break;
		case 5:
			cout<<endl<<"Test case 5 : Player should move on Gold.";
			break;
		case 6:
			cout<<endl<<"Test case 6 : Player should move to Gold and exit.";			
			break;
		default:
			break;	
	}	
	if(check==0)
	{
		cout<<endl<<"Woohoo!! Passed."<<endl;	
	}
	else
	{
		cout<<endl<<"Sorry!! Failed."<<endl;		
	}
    }
};

class FirstTest : public TestCase {
  public:
    FirstTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {
	char *initMem = this->firstChild.mapmem;        
	char c = 'h'; 
	int callsPostNotice,callsDrawMap,callsPostNotice1,callsDrawMap1;
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);

  	int check = memcmp(this->firstChild.mapmem,initMem,this->firstChild.rows*this->secondChild.cols);
	display_test_results(1,check);
	
        c='Q';        
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);              
        write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	close_game_descriptors();
    }
};

class SecondTest : public TestCase {
  public:
    SecondTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {
        
	char c = 'k'; 
	int callsPostNotice,callsDrawMap,callsPostNotice1,callsDrawMap1; 
        char *initMem = this->firstChild.mapmem;
        write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);
  	int check = memcmp(this->firstChild.mapmem,initMem,this->firstChild.rows*this->secondChild.cols);

	display_test_results(2,check);
        c='Q';
        write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap); 
        write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	close_game_descriptors();   
    }
};

class ThirdTest : public TestCase {
  public:
    ThirdTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {        
	char c = 's'; 
	int callsPostNotice,callsDrawMap,callsPostNotice1,callsDrawMap1;; 
        write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);        
	        
	c = 't';  
        write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	// get first child pos
	   int size = this->secondChild.rows*this->secondChild.cols;
	   int child1Pos=0,child2Pos=0;
           for(int i=0;i<size;i++)
	   {
           	if(this->secondChild.mapmem[i]==G_PLR0)
		{			
			child1Pos = i;
	    
		}
		if(this->secondChild.mapmem[i]==G_PLR1)
		{			
			child2Pos = i;		
		}	
           }
	   int left,right;
	   if(child1Pos<child2Pos)
	   {
	       left = child1Pos;
	       right = child2Pos;
		c = 'l';
	   }
	   else
	   {
		left = child2Pos;
		right = child1Pos;
		c = 'h'; 
	   } 	     
	// move left child to right's pos
	   char *initMem;	
	   for(int i = left;i<right-1;i++)
	   {			
        	write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);       	
	    }

	    c = ' ';	
	    write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);
	    initMem = firstChild.mapmem;
            write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap); 
	    // update map
	    c = ' ';	
	    write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);
	    //  
	         	
  	int check = memcmp(firstChild.mapmem,initMem,firstChild.rows*firstChild.cols);
        display_test_results(3,check);
        c='Q'; 
        write_character_getKey(this->firstChild,c,callsPostNotice1,callsDrawMap1);
        write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
        close_game_descriptors();
    }
};


class FourthTest : public TestCase {
  public:
    FourthTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {
	// quit Player 2
	char c = 'Q';
	int callsPostNotice1,callsDrawMap1,callsPostNotice,callsDrawMap;
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	// get map                
	c = ' '; 	
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);	
	
	   int size = this->firstChild.rows*this->firstChild.cols;
	   int child1Pos=0,child2Pos=0;
           for(int i=0;i<size;i++)
	   {
           	if(this->firstChild.mapmem[i]==G_PLR0)
		{			
			if(this->firstChild.mapmem[i+1]==0)
			{
				c = 'l';
				break; 			
			}
			else
			{
				c = 'h';
				break;			
			}	
		}	
           }
         
	char *initMem = this->firstChild.mapmem;
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);
  	int check = memcmp(this->firstChild.mapmem,initMem,this->firstChild.rows*this->secondChild.cols);
	
	display_test_results(4,check);

        c='Q';
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	
	close_game_descriptors();
    }
};


class FifthTest : public TestCase {
  public:
    FifthTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {
	// quit Player 2
	char c = 'Q';
	int callsPostNotice1,callsDrawMap1,callsPostNotice,callsDrawMap;
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	// get map                
	c = ' '; 	
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);	
	
	   int size = this->firstChild.rows*this->firstChild.cols;
	   int child1Pos=-1,goldPos=-1;
           for(int i=0;i<size;i++)
	   {
           	if(this->firstChild.mapmem[i]==G_PLR0)
		{			
			child1Pos = i;	
		}
		if(this->firstChild.mapmem[i]==G_GOLD)
		{
			goldPos = i;
		}	
           }

	   int left,right;
	   if(child1Pos<goldPos)
	   {
	       left = child1Pos;
	       right = goldPos;
		c = 'l';
	   }
	   else
	   {
		left = goldPos;
		right = child1Pos;
		c = 'h'; 
	   } 	     
	// move left child to right's pos
	   char *initMem;
	   int i=0;	
	   for(int i = left;i<right-1;i++)
	   {			 
        	write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);         
	   }
	    int c1 = ' ';	
	    write_character_getKey(firstChild,c1,callsPostNotice,callsDrawMap);
	    initMem = firstChild.mapmem;
            write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap); 
	    // update map
	    c1 = ' ';	
	    write_character_getKey(firstChild,c1,callsPostNotice,callsDrawMap);
	    //
	int check=-1;	
	if(c == 'l' && int(firstChild.mapmem[right]) == 65 || c == 'h' && int(firstChild.mapmem[left])== 65)
	{
		check = 0;	
	}
	else
	{
		check = -1;
	} 	
	display_test_results(5,check);

        c='Q';
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	
	close_game_descriptors();
    }
};


class SixthTest : public TestCase {
  public:
    SixthTest(const char *s):TestCase(s)
    {
    }
    void run_test()
    {
	// quit Player 2
	char c = 'Q';
	int callsPostNotice1,callsDrawMap1,callsPostNotice,callsDrawMap;
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	// get map                
	c = ' '; 	
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);	
	
	   int size = this->firstChild.rows*this->firstChild.cols;
	   int child1Pos=-1,goldPos=-1;
           for(int i=0;i<size;i++)
	   {
           	if(this->firstChild.mapmem[i]==G_PLR0)
		{			
			child1Pos = i;	
		}
		if(this->firstChild.mapmem[i]==G_GOLD)
		{
			goldPos = i;
		}	
           }

	   int left,right;
	   if(child1Pos<goldPos)
	   {
	       left = child1Pos;
	       right = goldPos;
		c = 'l';
	   }
	   else
	   {
		left = goldPos;
		right = child1Pos;
		c = 'h'; 
	   } 	     
	// move left child to right's pos
	   char *initMem;	
	   int i=0;	
	   for(int i = left;i<right-1;i++)
	   {	
        	write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);      
	   }
	    
            write_character_getKey(firstChild,c,callsPostNotice,callsDrawMap);
	    int c1 = ' ';	
	    write_character_getKey(firstChild,c1,callsPostNotice,callsDrawMap);
	    
	int check=-1;
	int pos= -1;
	if(c == 'l') pos = right;
	else pos = left;            		
	if(int(firstChild.mapmem[pos]) == 65)
	{
	     c1 = 'k';	
             write_character_getKey(firstChild,c1,callsPostNotice,callsDrawMap);
             c1 = ' ';	
	     write_character_getKey(firstChild,c1,callsPostNotice,callsDrawMap);	     
		if(int(firstChild.mapmem[pos]) == 0)
		{
			check = 0;		
		}
	}
	display_test_results(6,check);

        c='Q';
	write_character_getKey(this->firstChild,c,callsPostNotice,callsDrawMap);
	write_character_getKey(this->secondChild,c,callsPostNotice1,callsDrawMap1);
	
	close_game_descriptors();
    }
};



int main()
{
	string s = "1\n* * * *\n"; 
        const char *s1= s.c_str();
        FirstTest ft(s1);
	ft.run_test();	
	
	SecondTest st(s1);
	st.run_test();	
	
 	s = "0\n*           *\n";
	s1= s.c_str();
	ThirdTest th(s1);
	th.run_test();				
	
	s = "0\n*           *\n";
	s1= s.c_str();
	FourthTest fr(s1);
	fr.run_test();		
	
	      
	s =  "1\n*           *\n";
	s1= s.c_str();
	FifthTest ff(s1);
	ff.run_test();
			
	s =  "1\n*           *\n";
	s1= s.c_str();
	SixthTest ss(s1);
	ss.run_test();
	
	return 0;
}


//write(firstPlayer.toGame, "fdsafdsa",...);
