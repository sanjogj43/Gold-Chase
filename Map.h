#ifndef MAP_H
#define MAP_H

#include<ncurses.h>
#include<panel.h>
#include "Screen.h"

/////
// The Map class uses the Screen class to paint a map
/////
class Map {
  public:
    Map(const char* mapmem, int l, int w);
    void drawMap();
    void postNotice(const char* msg);
    int getKey();
  private:
    //Screen theMap;
    const char* mapmem;
    int rows;
    int cols;
    int callsToPostNotice;
    int callsToDrawMap; 
};

#endif //MAP_H
