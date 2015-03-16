#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<iostream>
#include<stdexcept>

#include"goldchase.h"
#include"Screen.h"
#include"Map.h"

using namespace std;

//Initialize the object and draw the map
Map::Map(const char* mmem, int rows, int cols) 
{
  //cout<<"In map";
  this->callsToPostNotice=0;
  this->callsToDrawMap=0;
  this->rows = rows;
  this->cols = cols;
  this->mapmem = mmem; // might need to creat a new instance all together.

  write(21, &this->rows, sizeof(this->rows));
  write(21, &this->cols, sizeof(this->cols));
  write(21, this->mapmem, this->rows*this->cols);
}

int Map::getKey()
{
  char character;
  write(21,&callsToPostNotice,sizeof(callsToPostNotice));
  write(21,&callsToDrawMap,sizeof(callsToDrawMap));
  write(21,mapmem,this->rows*this->cols);
  read(20,&character,sizeof(character));
  return character;
}

void Map::postNotice(const char* msg)
{
  this->callsToPostNotice++; 
  //cout<<this->callsToPostNotice<<"\n";
}


//Draw and refresh map from memory array
void Map::drawMap()
{
  this->callsToDrawMap++;
}
