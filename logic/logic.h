#ifndef ROBOTLOGIC
#define ROBOTLOGIC
#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include "types.h"
#include <fstream>

using namespace types;

namespace robotlogic
{
    std::vector<std::vector<cell>> transpose_map(const std::vector<std::vector<cell>>& map);
    
    class Robot{
    private:
        typedef std::vector<std::vector<cell>> Map;
        int x, y;
        bool gotBox;
        Map room;
        int radiusOfView;
    public:
        Robot(int startx, int starty, int rad, Map map) : x(startx), y(starty), room(map), radiusOfView(rad), gotBox(false) {
            if(check_cell(x,y) != EMPTY && check_cell(x,y) != EXIT) throw LogicException("Invalid robot placement");
        }
        Robot() = default;
        void change_rad_of_view(int nrad) {radiusOfView = nrad;}
        int get_rad_of_view() {return radiusOfView;}
        bool go(dir direction);
        bool pick(dir direction);
        bool drop(dir direction);
        std::vector<cell> look(dir direction);
        cell check_cell(int cellx, int celly);
        void print_room(){
            for(int ity = 0; ity <= room[0].size()+1; ity++){
                for(int itx = 0; itx <= room.size()+1; itx++){
                    int iy = ity-1;
                    int ix = itx-1;
                    if(iy == -1 || ix == -1 || iy == room[0].size() || ix == room.size()){
                        std::cout << "█"; 
                        continue;
                    }
                    if(x == ix && y == room[0].size()-1-iy) 
                        std::cout << "#";
                    else{
                        if(room[ix][room[0].size()-1-iy] == 0) std::cout << " ";
                        if(room[ix][room[0].size()-1-iy] == 1) std::cout << "█";
                        if(room[ix][room[0].size()-1-iy] == 2) std::cout << "B";
                        if(room[ix][room[0].size()-1-iy] == 3) std::cout << "E";
                    } 
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        cell current_cell() {return room[x][y];}
        bool is_exit() {return current_cell() == EXIT;}
    };

    std::vector<std::vector<types::cell>> get_map(std::string filename,  int& sx, int& sy);
}


#endif