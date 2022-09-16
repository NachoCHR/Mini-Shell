#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>


const int READ = 0; // variable para pipe
const int WRITE = 1; // variable para pipe

using namespace std;
int main()
{
    vector<vector<string> > All;
    vector<string> command;
    //cin >> cadena;
    //cout << cadena;
    string instruction;
    bool pront = true;
    //cout << "$";
    long long maxim;
    while (getline(cin,instruction)){ 
        maxim = 0;
        if(pront) cout << "$";
        pront = false;
        string Chain1;
        if(instruction == "exit"){
            return 0;
        }else if(instruction == "\n"){
            pront = true;
            continue;
        }
        stringstream S1(instruction);
        while (getline(S1 , Chain1 , '|'))
        {
            string Chain2;
            stringstream S2(Chain1); 
            while(getline(S2, Chain2 , ' ')){
                command.push_back(Chain2);
            }
            long long m = command.size();
            maxim = max(m, maxim);
            All.push_back(command);
            command.clear();
        }
        // Case only one command 
        if( All.size() <= 1){
            int pid = fork();
            if(pid == 0){
                char *ArgsCommand[All[0].size() + 1];
                int i = 0;
                for(i; i < All[0].size(); i++){
                    ArgsCommand[i] = strdup(All[0][i].c_str());
                }
                ArgsCommand[i] = NULL;
                execvp(ArgsCommand[0],ArgsCommand);
                exit(EXIT_FAILURE);
            }else if(pid < 0){
                cout << "ERROR";
                // ERROR;
            }
            else{
                wait(NULL);
            }
        }else{      // Case find pipe
            int i = 0;
            char *ArgsCommand[All.size()][maxim];
            for(i; i < All.size(); i++){
                for (int j = 0; j < All[i].size(); j++)
                {
                    ArgsCommand[i][j] = strdup(All[i][j].c_str()); 
                }
                ArgsCommand[i][All[i].size() + 1] = NULL;
            }
        }
        All.clear();   
        pront = true;         
    }
    return 0;
}
/*for (int l =  0; l < All.size(); l++){
    for(auto &x : All[l]){
        cout << x << endl;
    }
    cout << endl;
}*/

