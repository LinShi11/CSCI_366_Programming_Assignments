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

#include "common.hpp"
#include "Client.hpp"

Client::~Client() {
    remove(board_name.c_str());
}


void Client::initialize(unsigned int player, unsigned int board_size){

    std::string out_string;
    std::stringstream ss;
    ss << player;
    out_string = ss.str();
    ofstream board_name("player_" + out_string + ".action_board.json");
    string coord = "{\n    \"board\": [\n";

    for(int i = 0; i < board_size; i++){
        coord += "        [\n";
        for(int j = 0; j < board_size; j++){
            if(j != board_size-1){
                coord += "            0,\n";
            }
            else{
                coord += "            0\n";
            }
        }
        if(i != board_size-1) {
            coord += "        ],\n";
        }
        else {
            coord += "        ]\n";
        }
    }
    coord += "    ]\n";
    coord += "}";
    board_name << coord;
    board_name.close();
    this->board_size = board_size;
    if(player == 2){
        initialized = true;
    }
    this->player = 1;
}


void Client::fire(unsigned int x, unsigned int y) {
    ofstream output;

    int temp = this->player%2;
    if(temp==1){
        output.open("player_1.shot.json");
    }
    else{
        output.open("player_2.shot.json");
    }

    string coords = "{\n    \"x\": "+to_string(x)+",\n    \"y\": "+to_string(y)+"\n}";
    output << coords;

}


bool Client::result_available() {
    ifstream input;
    int temp = this->player%2;
    if(temp==1){
        input.open("player_1.result.json");
    }
    else{
        input.open("player_2.result.json");
    }

    string result_str = "{\n"
                        "    \"result\": "+to_string(1)+"\n"
                                                             "}";
    if(input.is_open()){
        return true;
    }

}


int Client::get_result() {
    ifstream input;
    int temp = this->player%2;
    if(temp==1){
        input.open("player_1.result.json");
    }
    else{
        input.open("player_2.result.json");
    }
    noskipws(input);
    istreambuf_iterator<char> head(input);
    istreambuf_iterator<char> tail;
    string data(head, tail);
    char result;

    for(int i =0; i < sizeof(data); i++){
        if(data[i] == 'r'){

            result = data[i+9] ;
            break;
        }
    }
    input.close();
    remove("player_1.result.json");
    remove("player_2.result.json");
    if(result == '1'){
        return HIT;
    }
    else if (result == '-'){
        return MISS;
    }
    else if (result == '0'){
        return OUT_OF_BOUNDS;
    }
    else{
        throw(ClientException("Bad result"));
    }
}



void Client::update_action_board(int result, unsigned int x, unsigned int y) {
    ofstream board_name;
    int temp = this->player%2;
    if(temp==1){
        board_name.open("player_1.action_board.json");
    }
    else{
        board_name.open("player_2.action_board.json");
    }

    string coord = "{\n    \"board\": [\n";

    for(int i = 0; i < board_size; i++){
        coord += "        [\n";
        for(int j = 0; j < board_size; j++){
            if(i == y and j == x){
                coord += "            "+ to_string(result) + ",\n";
            }
            else {
                if (j != board_size - 1) {
                    coord += "            0,\n";
                } else {
                    coord += "            0\n";
                }
            }
        }
        if(i != board_size-1) {
            coord += "        ],\n";
        }
        else {
            coord += "        ]\n";
        }
    }

    coord += "    ]\n";
    coord += "}";
    board_name << coord;
    board_name.close();

}


string Client::render_action_board(){
    string result;
    string file;
    int temp = this->player%2;
    if(temp==1){
        file = "player_1.action_board.json";
    }
    else{
        file = "player_2.action_board.json";
    }
    for(int i =0; i < file.length(); i++){
        char x = file.at(i);
        result += to_string(int(x));
    }
    result += "\n";
    return result;
}