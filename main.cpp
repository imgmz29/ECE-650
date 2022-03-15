#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <pqxx/pqxx>

#include "query_funcs.h"
#include "exerciser.h"

void createTable(std::string create_file, pqxx::connection *C){
    std::string query;
    std::string line;
    std::ifstream file;
    file.open(create_file.c_str(), std::ifstream::in);

    while(std::getline(file, line))
        query = query + line;
    
    file.close();

    pqxx::work W(*C);
    W.exec(query);
    W.commit();
}

void dropTable(pqxx::connection *C){
    std::string query;
    
    query = "DROP TABLE IF EXISTS PLAYER CASCADE;";
    pqxx::work W1(*C);
    W1.exec(query);
    W1.commit();

    query = "DROP TABLE IF EXISTS TEAM CASCADE;";
    pqxx::work W2(*C);
    W2.exec(query);
    W2.commit();

    query = "DROP TABLE IF EXISTS STATE CASCADE;";
    pqxx::work W3(*C);
    W3.exec(query);
    W3.commit();

    query = "DROP TABLE IF EXISTS COLOR CASCADE;";
    pqxx::work W4(*C);
    W4.exec(query);
    W4.commit();
}

void do_add_state(std::string state_file, pqxx::connection *C){
    std::string line;
    std::string name;
    int state_id;
    std::ifstream file;
    file.open(state_file.c_str(), std::ifstream::in);

    while(std::getline(file, line)){
        std::stringstream ss;
	    ss << line;
	    ss >> state_id >> name;
	    add_state(C, name);
    }
    
    file.close();
}

void do_add_color(std::string color_file, pqxx::connection *C){
    std::string line;
    std::string name;
    int color_id;
    std::ifstream file;
    file.open(color_file.c_str(), std::ifstream::in);

    while(std::getline(file, line)){
        std::stringstream ss;
	    ss << line;
	    ss >> color_id >> name;
	    add_color(C, name);
    }
    
    file.close();
}

void do_add_team(std::string team_file, pqxx::connection *C){
    std::string line;
    std::string name;
    int team_id, state_id, color_id, wins, losses;
    std::ifstream file;
    file.open(team_file.c_str(), std::ifstream::in);

    while(std::getline(file, line)){
        stringstream ss;
	    ss << line;
	    ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
        add_team(C, name, state_id, color_id, wins, losses);
    }

    file.close();
}

void do_add_player(std::string player_file, pqxx::connection *C){
    std::string line;
    std::string first_name, last_name;
    int player_id, team_id, uniform_num, mpg, ppg, rpg, apg;
    double spg, bpg;
    std::ifstream file;
    file.open(player_file.c_str(), std::ifstream::in);

    while(std::getline(file, line)){
        stringstream ss;
	    ss << line;
	    ss >> player_id >> team_id >> uniform_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg;
	    add_player(C, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
    }

    file.close();
}

int main(int argc, char *argv[]){
    pqxx::connection *C;

    try{
        C = new connection("dbname=acc_bball user=postgres password=passw0rd");

        dropTable(C);
        createTable("create_table.sql", C);
        do_add_state("state.txt", C);
        do_add_color("color.txt", C);
        do_add_team("team.txt", C);
        do_add_player("player.txt", C);

        exercise(C);

        C->disconnect();
    }
    catch (const std::exception &e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}