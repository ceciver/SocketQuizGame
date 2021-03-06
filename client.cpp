#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>


void loggin(void);
void mainscene(void);
void game(void);
void scoreboard(void);
void help(void);
void loadData(void); // load questions and scoreboard
void sendDataToServer(void); // After finishing the game I should send my username and score to the server to update the scoreboard

enum SCENESTATE {
    LOGININ,
    MAINSCENE,
    GAME,
    SCOREBOARD,
    HELP,
    EXIT
};

struct QUESTION {
    std::string question;
    std::vector<std::string> answers;
    int indexOfCorrectans;
};

std::vector<QUESTION> questionsDB;
std::vector<std::pair<int,std::string>> scoreboardDB;

std::string playername;
int score;
SCENESTATE state;


int main() {
   loadData();
   state = LOGININ;

    while(true) {
        system("clear");

        switch(state) {
            case LOGININ:
                loggin();
                break;

            case MAINSCENE:
                mainscene();
                break;

            case GAME:
                game();
                break;

            case SCOREBOARD:
                scoreboard();
                break;

            case HELP:
                help();
                break;

            case EXIT: 
                return 0;

        }
    }
    return 0;
}

void loggin() {

    printf("\n\t\t########################################");
    printf("\n\t\t########################################");
    printf("\n\t\t########################################");
    printf("\n\t\t############ WELCOME TO ################");
    printf("\n\t\t############ FST QUIZ   ################");
    printf("\n\t\t############ GAME :)    ################");
    printf("\n\t\t########################################");
    printf("\n\t\t########################################");
    printf("\n\t\t########################################");

    printf("\n");
    printf("\n\t\t > ENTER YOUR NICKNAME: ");
    std::cin>>playername;
    state = MAINSCENE;
}

void mainscene() {

    printf("\n\t\t########################################");
    printf("\n\t\t\t  Hello %s\n",playername.c_str()) ;
    printf("\n\t\t########################################");
    printf("\n\t\t### > Enter S to start the game      ###");
    printf("\n\t\t### > Enter V to view the scoreboard ###");
    printf("\n\t\t### > Enter H for help               ###");
    printf("\n\t\t### > Enter Q to quit                ###");
    printf("\n\t\t########################################\n\n");
    printf("\n\t\tEnter your choice: ");

    char choice;
    scanf("%c",&choice);
    choice = toupper(choice);

    switch(choice) {
        case 'S': 
            state = GAME;
            break;
        case 'V': 
            state = SCOREBOARD;
            break;
        case 'H': 
            state = HELP;
            break;
        case 'Q': 
            state = EXIT; break;
    }

}

void game() {
    for(auto quest: questionsDB) {
        system("clear");
        printf("\n\n\t\tYour score: %d",score);
        printf("\n\n\t\t%s",quest.question.c_str());
        int idx = 1;
        for(auto ans: quest.answers) {
            printf("\n\t\t > %d %s",idx++, ans.c_str());
        }
        int choice;

        printf("\n\n\t\tEnter your choice: ");
        scanf("%d",&choice);

        score += (choice == quest.indexOfCorrectans);

    }
    scoreboardDB.push_back({score,playername});
    sendDataToServer();
    state = SCOREBOARD;

}

void scoreboard() {

    system("clear");
    printf("\n\t\t########################################");
    printf("\n\t\t############ FINAL      ################");
    printf("\n\t\t############ SCOREBOARD ################");
    printf("\n\t\t########################################\n\n");

    sort(scoreboardDB.rbegin(), scoreboardDB.rend());
    for(auto p: scoreboardDB) {
        printf("\n\t\t %s -> %d\n",p.second.c_str(), p.first);
    }

    printf("\n\t\t########################################");
    printf("\n\t\t############## FST    ##################");
    printf("\n\t\t############## SETTAT ##################");
    printf("\n\t\t########################################\n");
    char choice;
    printf("\n\n\t\tEnter b to back: ");
    scanf("%c",&choice);
    scanf("%c",&choice);
    state = MAINSCENE;
}

void help() {

    printf("\n\n\t\tIf you face any technical issues you can contact one of the developers!!!");
    printf("\n\t\t#####################################################");
    printf("\n\t\t############ > IBRAHIM EL MOUNTASSER ################");
    printf("\n\t\t############ > MOHAMED AMINE FAROUQ  ################");
    printf("\n\t\t############ > OMAR BOUNIF           ################");
    printf("\n\t\t############ > NOUREDDINE ELLAJA     ################");
    printf("\n\t\t#####################################################");
    char choice;
    printf("\n\n\t\tEnter b to back: ");
    scanf("%c",&choice);
    scanf("%c",&choice);
    state = MAINSCENE;
    
}

void loadData() {

    using namespace std;
    ifstream filein("database/questions.db");

    bool isQ = true;
    QUESTION quest;

    for (string line; getline(filein, line); ) {
        if(isQ) {
            quest.question = line;
        }else {
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                    istream_iterator<string>(),
                    back_inserter(tokens));

            quest.indexOfCorrectans = stoi(tokens.back());
            tokens.pop_back();
            quest.answers = tokens;
            questionsDB.push_back(quest);
        }
        isQ = !isQ;
    }

    filein.close();

    ifstream file2in("database/scoreboard.db");

    for (string line; getline(file2in, line); ) {
            vector<string> tokens;
            istringstream iss(line);
            copy(istream_iterator<string>(iss),
                    istream_iterator<string>(),
                    back_inserter(tokens));

            int score = stoi(tokens.front());
            string name = tokens.back();
            scoreboardDB.push_back({score,name});
    }

    file2in.close();
}


void sendDataToServer() {

    std::string packet = std::to_string(score) + " " + playername ;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;


    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(server_socket,5);


    int client_socket = accept(server_socket,NULL,NULL);


    send(client_socket, packet.c_str(), sizeof(packet), 0);

    close(server_socket);

}


