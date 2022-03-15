#include "query_funcs.h"


void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg){
    pqxx::work W(*C);

    std::string query;
    query = "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES ("  
            + to_string(team_id) + ", " + to_string(jersey_num) + ", " +  W.quote(first_name) + ", " + W.quote(last_name) + ", " 
            + to_string(mpg) + ", " + to_string(ppg) + ", " + to_string(rpg) + ", " + to_string(apg) + ", " 
            + to_string(spg) + ", " + to_string(bpg) + ");";

    W.exec(query);
    W.commit();
}

void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses){
    pqxx::work W(*C);

    std::string query;
    query = "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES(" 
            + W.quote(name) + ", " + to_string(state_id) + ", " + to_string(color_id) + ", " 
            + to_string(wins) + ", " + to_string(losses) + ");";

    W.exec(query);
    W.commit();
}


void add_state(connection *C, string name){
    pqxx::work W(*C);

    std::string query;
    query =  "INSERT INTO STATE (NAME) VALUES (" +  W.quote(name) + ");";

    W.exec(query);
    W.commit();
}


void add_color(connection *C, string name){
    pqxx::work W(*C);

    std::string query;
    query = "INSERT INTO COLOR (NAME) VALUES (" + W.quote(name) + ");";

    W.exec(query);
    W.commit();    
}

void check(bool &start, std::string &query){
    if(start == true){
        query += " WHERE ";
        start = false;
    }
    else
        query += " AND ";   
}

//show all attributes of each player with average statistics 
//that fall between the min and max (inclusive) for each enabled statistic
void query1(connection *C,
	        int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            ){
    std::string query = "SELECT * FROM PLAYER";
    bool start = true;

    if(use_mpg != 0){
        check(start, query);
        query += "MPG BETWEEN " + to_string(min_mpg) + " AND " + to_string(max_mpg);
    }

    if(use_ppg != 0){
        check(start, query);
        query += "PPG BETWEEN " + to_string(min_ppg) + " AND " + to_string(max_ppg);
    }

    if(use_rpg != 0){
        check(start, query);
        query += "RPG BETWEEN " + to_string(min_rpg) + " AND " + to_string(max_rpg);
    }

    if(use_apg != 0){
        check(start, query);
        query += "APG BETWEEN " + to_string(min_apg) + " AND " + to_string(max_apg);
    }

    if(use_bpg != 0){
        check(start, query);
        query += "SPG BETWEEN " + to_string(min_spg) + " AND " + to_string(max_spg);
    }

    if(use_spg != 0){
        check(start, query);
        query += "BPG BETWEEN " + to_string(min_bpg) + " AND " + to_string(max_bpg);
    }

    query += ";";

    pqxx::nontransaction N(*C);
    pqxx::result R(N.exec(query));

    std::cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << std::endl;
    for (pqxx::result::const_iterator it = R.begin(); it != R.end(); it++) {
        std::cout << it[0].as<int>() << " " << it[1].as<int>() << " " << it[2].as<int>() << " "
            << it[3].as<std::string>() << " " << it[4].as<std::string>() << " " 
            << it[5].as<int>() << " " << it[6].as<int>() << " " << it[7].as<int>() << " " << it[8].as<int>() << " "
            << fixed << setprecision(1) << it[9].as<double>() << " " << it[10].as<double>() << std::endl;
    }
}

//show the name of each team with the indicated uniform color
void query2(connection *C, string team_color){
    pqxx::work W(*C);
    std::string query = "SELECT TEAM.NAME FROM TEAM, COLOR WHERE COLOR.COLOR_ID = TEAM.COLOR_ID AND COLOR.NAME = "
                        + W.quote(team_color) + ";";
    W.commit();

    pqxx::nontransaction N(*C);
    pqxx::result R(N.exec(query));

    std::cout << "NAME" << std::endl;
    for(pqxx::result::const_iterator it = R.begin(); it!=R.end(); it++){
        std::cout << it[0].as<std::string>() << std::endl;
    } 
}

//show the first and last name of each player 
//that plays for the indicated team, ordered from highest to lowest ppg
void query3(connection *C, string team_name){
    pqxx::work W(*C);
    std::string query = "SELECT FIRST_NAME, LAST_NAME FROM PLAYER, TEAM WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.NAME = "
                        + W.quote(team_name) + " ORDER BY PPG DESC;";
    W.commit();

    pqxx::nontransaction N(*C);
    pqxx::result R(N.exec(query));

    std::cout << "FIRST_NAME LAST_NAME" << std::endl;
    for(pqxx::result::const_iterator it = R.begin(); it!=R.end(); it++){
        std::cout << it[0].as<std::string>() << " " << it[1].as<std::string>() << std::endl;
    }
}

//show first name, last name, and jersey number of each player 
//that plays in the indicated state and wears the indicated uniform color
void query4(connection *C, string team_state, string team_color){
    pqxx::work W(*C);
    std::string query = "SELECT FIRST_NAME, LAST_NAME, UNIFORM_NUM FROM PLAYER, STATE, COLOR, TEAM" 
                        " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID AND TEAM.STATE_ID = STATE.STATE_ID"
                        " AND STATE.NAME = " + W.quote(team_state) +
                        " AND COLOR.NAME = " + W.quote(team_color) + ";";
    W.commit();

    pqxx::nontransaction N(*C);
    pqxx::result R(N.exec(query));

    std::cout << "FIRST_NAME LAST_NAME UNIFORM_NUM" << std::endl;
    for(pqxx::result::const_iterator it = R.begin(); it!=R.end(); it++){
        std::cout << it[0].as<std::string>() << " " << it[1].as<std::string>() << " " << it[2].as<int>() << std::endl;
    }
}

//show first name and last name of each player, and team name and number of wins
//for each team that has won more than the indicated number of games
void query5(connection *C, int num_wins){
    pqxx::work W(*C);
    std::string query = "SELECT FIRST_NAME, LAST_NAME, NAME, WINS FROM PLAYER, TEAM WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID" 
                        " AND WINS > " + to_string(num_wins) + ";";
    W.commit();

    pqxx::nontransaction N(*C);
    pqxx::result R(N.exec(query));

    std::cout << "FIRST_NAME LAST_NAME TEAM_NAME WINS" << std::endl;
    for(result::const_iterator it = R.begin(); it!=R.end(); it++){
        std::cout << it[0].as<std::string>() << " " << it[1].as<std::string>() << " " << it[2].as<std::string>() << " " <<it[3].as<int>() << std::endl;
    }
}

