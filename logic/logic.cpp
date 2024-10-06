#include "logic.h"
namespace robotlogic{

    cell Robot::check_cell(int cellx, int celly){
        if(cellx > room.size()-1) return UNDEF;
        if(cellx < 0) return UNDEF;
        if(celly > room[cellx].size()-1) return UNDEF;
        if(celly < 0) return UNDEF;
        return room[cellx][celly];
    }

    bool Robot::go(dir direction){
        cell nextCell;
        switch (direction)
        {
        case LEFT:
            nextCell = check_cell(x-1, y);
            if(nextCell == WALL || nextCell == BOX || nextCell == UNDEF) return false;
            x--;
            break;
        case RIGHT:
            nextCell = check_cell(x+1, y);
            if(nextCell == WALL || nextCell == BOX || nextCell == UNDEF) return false;
            x++;
            break;
        case UP:
            nextCell = check_cell(x, y+1);
            if(nextCell == WALL || nextCell == BOX || nextCell == UNDEF) return false;
            y++;
            break;
        case DOWN:
            nextCell = check_cell(x, y-1);
            if(nextCell == WALL || nextCell == BOX || nextCell == UNDEF) return false;
            y--;
            break;
        default:
            break;
        }
        return true;
    }
    bool Robot::pick(dir direction)
    {
        cell nextCell;
        if(gotBox) return false;
        switch (direction)
        {
        case LEFT:
            nextCell = check_cell(x-1, y);
            if(nextCell != BOX) return false;
            room[x-1][y] = EMPTY;
            break;
        case RIGHT:
            nextCell = check_cell(x+1, y);
            if(nextCell != BOX) return false;
            room[x+1][y] = EMPTY;
            break;
        case UP:
            nextCell = check_cell(x, y+1);
            if(nextCell != BOX) return false;
            room[x][y+1] = EMPTY;
            break;
        case DOWN:
            nextCell = check_cell(x, y-1);
            if(nextCell != BOX) return false;
            room[x][y-1] = EMPTY;
            break;
        default:
            break;
        }
        gotBox = true;
        return true;
    }
    bool Robot::drop(dir direction)
    {
        cell nextCell;
        if(!gotBox) return false;
        switch (direction)
        {
        case LEFT:
            nextCell = check_cell(x-1, y);
            if(nextCell != EMPTY) return false;
            room[x-1][y] = BOX;
            break;
        case RIGHT:
            nextCell = check_cell(x+1, y);
            if(nextCell != EMPTY) return false;
            room[x+1][y] = BOX;
            break;
        case UP:
            nextCell = check_cell(x, y+1);
            if(nextCell != EMPTY) return false;
            room[x][y+1] = BOX;
            break;
        case DOWN:
            nextCell = check_cell(x, y-1);
            if(nextCell != EMPTY) return false;
            room[x][y-1] = BOX;
            break;
        default:
            break;
        }
        gotBox = false;
        return true;
    }
    std::vector<cell> Robot::look(dir direction)
    {
        std::vector<cell> cells;
        int initx = x;
        int inity = y;
        int rad = 0;
        switch (direction)
        {
        case LEFT:
            initx--;
            while(check_cell(initx, inity) != UNDEF){
                rad++;
                if(rad > radiusOfView) {cells.push_back(UNDEF); break;}
                cells.push_back(room[initx][inity]);
                if(room[initx][inity] == WALL || room[initx][inity] == BOX) break;
                initx--;
            }
            break;
        case RIGHT:
            initx++;
            while(check_cell(initx, inity) != UNDEF){
                rad++;
                if(rad > radiusOfView) {cells.push_back(UNDEF); break;}
                cells.push_back(room[initx][inity]);
                if(room[initx][inity] == WALL || room[initx][inity] == BOX) break;
                initx++;
            }
            break;
        case UP:
            inity++;
            while(check_cell(initx, inity) != UNDEF){
                rad++;
                if(rad > radiusOfView) {cells.push_back(UNDEF); break;}
                cells.push_back(room[initx][inity]);
                if(room[initx][inity] == WALL || room[initx][inity] == BOX) break;
                inity++;
            }
            break;
        case DOWN:
            inity--;
            while(check_cell(initx, inity) != UNDEF){
                rad++;
                if(rad > radiusOfView) {cells.push_back(UNDEF); break;}
                cells.push_back(room[initx][inity]);
                if(room[initx][inity] == WALL || room[initx][inity] == BOX) break;
                inity--;
            }
            break;
        default:
            break;
        }
        return cells;
    }
    std::vector<std::vector<cell>> transpose_map(const std::vector<std::vector<cell>> &map)
    {
        int n = map.size(), m = map[0].size(); // n x m
        // x < n; y < m in init
        std::vector<std::vector<cell>> trmap(m, std::vector<cell>(n)); //m x n
        for(int x = 0; x < n; x++){
            for(int y = 0; y < m; y++){
                trmap[y][x] = map[n-1-x][y];
            }
        }
        return trmap;
    }
    std::vector<std::vector<types::cell>> get_map(std::string filename, int &sx, int &sy)
    {
        std::fstream mapfile(filename);
        std::string s, startingx, startingy;
        bool tox = true;
        std::getline(mapfile, s);
        for(const auto &it : s){
            if(it == ' ') {tox = false; continue;}
            if(tox) startingx.push_back(it);
            else startingy.push_back(it);
        }
        sx = std::stoi(startingx);
        sy = std::stoi(startingy);
        std::vector<std::vector<cell>> rawmap;
        while(std::getline(mapfile, s)){
            rawmap.push_back(std::vector<cell>());
            for(auto it : s){
                if(it == ' ') it = '1';
                rawmap[rawmap.size()-1].push_back((cell)(it - '0'));
            }
            for(auto &it : rawmap){
                while(it.size() < rawmap[rawmap.size()-1].size()){
                    it.push_back(WALL);
                }
            }
        }
        // for(const auto& x : rawmap){
        //     for(const auto &dot : x){
        //         std::cout << dot;
        //     }
        //     std::cout << "\n";
        // }
        // std::cout << "\n";
        return transpose_map(rawmap);
    }
}