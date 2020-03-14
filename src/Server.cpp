//    Battleship game assignment for MSU CSCI 366
//    Copyright (C) 2020    Mike P. Wittie
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include <iomanip>
#include "common.hpp"
#include "Server.hpp"

/**
 * Calculate the length of a file (helper function)
 *
 * @param file - the file whose length we want to query
 * @return length of the file in bytes
 */
int get_file_length(ifstream * file){
}


void Server::initialize(unsigned int board_size,
                        string p1_setup_board,
                        string p2_setup_board){
    char line[board_size];
    this->board_size = board_size;

    if(board_size != BOARD_SIZE){
        throw(ServerException("Wrong board size"));
    }

    try{
        this->p1_setup_board.open(p1_setup_board);
        if(!this->p1_setup_board.is_open()){
            throw(ServerException("bad file name"));
        }
        this->p1_setup_board.close();
    }catch(exception e){
        throw;
    }

    try{
        this->p2_setup_board.open(p2_setup_board);
        int p2Size = 0;
        if(!this->p2_setup_board.is_open()){
            throw(ServerException("bad file name"));
        }
        while(!this->p2_setup_board.eof()) {
            this->p2_setup_board >> line;
            if(!this->p2_setup_board.fail()){
                ++p2Size;
            }
            else{
                break;
            }
        }
        this->p2_setup_board.close();
    }
    catch(exception e) {
        throw(ServerException("Wrong number of players"));
    }
}


int Server::evaluate_shot(unsigned int player, unsigned int x, unsigned int y) {

    ifstream board;
    char line[10];

    if(x >= (board_size)){
        return OUT_OF_BOUNDS;
    }
    else if(y >= (board_size)){
        return OUT_OF_BOUNDS;
    }
    if(player == 1) {
        p2_setup_board.open("player_2.setup_board.txt");
        p2_setup_board.clear();
        p2_setup_board.seekg(0,ios::beg);
        int size = 0;
        while(p2_setup_board >> line) {
            if(size == y){
                for(int i = 0; i < sizeof(line); i++){
                    if(i== x){
                        char temp[6] = SHIPS;

                        for(int j = 0; j < sizeof(temp); j++){

                            if(line[x] == temp[j]){
                                return HIT;
                            }
                            else{
                                continue;
                            }
                        }
                        return MISS;
                    }
                }
            }
            size++;
            line[0] = '\0';
        }
        p2_setup_board.close();
    }
    else if(player == 2){
        p1_setup_board.open("player_1.setup_board.txt");
        p1_setup_board.clear();
        p1_setup_board.seekg(0,ios::beg);
        int size = 0;
        while(p1_setup_board >> line) {
            if(size == y){
                for(int i = 0; i < sizeof(line); i++){

                    if(i== x){
                        char temp[6] = SHIPS;

                        for(int j = 0; j < sizeof(temp); j++){

                            if(line[x] == temp[j]){
                                return HIT;
                            }
                            else{
                                continue;
                            }
                        }
                        return MISS;
                    }
                }
            }
            size++;
            line[0] = '\0';

        }
        p1_setup_board.close();
    }
    else{
        throw(ServerException("Bad player number"));
    }
    return OUT_OF_BOUNDS;
}


int Server::process_shot(unsigned int player) {
    ofstream output;

    if (player == 1) {
        output.open("player_1.result.json");
    } else if(player == 2){
        output.open("player_2.result.json");
    }
    else{
        throw(ServerException("Bad number"));
    }

    string line;
    int x = 0, y = 0;
    ifstream input;
    if (player == 1) {
        input.open("player_1.shot.json");
    } else {
        input.open("player_2.shot.json");
    }

    noskipws(input);
    istreambuf_iterator<char> head(input);
    istreambuf_iterator<char> tail;
    string data(head, tail);

    for (int i = 0; i < sizeof(data); i++) {
        if (data[i] == 'x') {
            x = data[i + 4] - '0';
            if (data[i + 5] == '0') {
                x = 10;
            }
        } else if ((data[i]) == 'y') {
            y = data[i + 4] - '0';
            if (data[i + 5] == '0') {
                y = 10;
            }
            break;
        }
    }
    int response = -2;
    if (input.is_open()) {
        response = evaluate_shot(player, x, y);
    }

    if(!output.good()){
        throw(ServerException("can't write"));
    }
    input.close();
    remove("player_1.shot.json");
    remove("player_2.shot.json");
    output << "{" << endl;
    if (response == HIT) {
        output << "    \"result\": 1\n";
        output << "}";
        output.close();
        return SHOT_FILE_PROCESSED;
    }
    else if(response == MISS){
        output << "    \"result\": -1\n";
        output << "}";
        output.close();
        return SHOT_FILE_PROCESSED;
    }
    else if(response == OUT_OF_BOUNDS){
        output << "    \"result\": 0\n";
        output << "}";
        output.close();
        return  SHOT_FILE_PROCESSED;
    }

    return NO_SHOT_FILE;

}
