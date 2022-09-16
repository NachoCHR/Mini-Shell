#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <istream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
//#include <bits/stdc++.h>


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
            command.erase(std::remove(command.begin(), command.end(), ""), command.end());
            long long m = command.size();
            maxim = max(m, maxim);
            All.push_back(command);
            command.clear();
        }
        // Case only one command 
        if( All.size() <= 1){
            char *ArgsCommand[All[0].size() + 1];
            int i = 0;
            for(i; i < All[0].size(); i++){
                ArgsCommand[i] = strdup(All[0][i].c_str());
            }
            ArgsCommand[i] = NULL;
            int pid = fork();
            if(pid == 0){        
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
            char *ArgsCommand[All.size()][maxim + 1];
            for(int i = 0; i < All.size(); i++){
                for (int j = 0; j < All[i].size(); j++)
                {
                    ArgsCommand[i][j] = strdup(All[i][j].c_str()); 
                    cout << ArgsCommand[i][j] << " ";
                }
                ArgsCommand[i][All[i].size()] = NULL;
            }
            // CREATE PIPES
            long long allpipes  = All.size() - 1;
            int Pipes[allpipes][2];
            //vector<vector<int>> Pipes(All.size() - 1,n) ;
            for(int k = 0; k < allpipes; k++)  pipe(Pipes[k]);
                    
                    // CHILDREN PROCESS FOR THE PIPES
            int count  = 0;
            if(allpipes == 1){
                if(fork() == 0){
                    close(Pipes[count][READ]);
                    dup2(Pipes[count][WRITE], WRITE);
                    execvp(ArgsCommand[0][0], ArgsCommand[0]);
                }else{
                    close(Pipes[count][READ]);
                    dup2(Pipe[cout][WRITE]);
                    execvp(ArgsCommand)
                }
            }else{
            // PROCESs 1
            int count = 0;
            if(fork() == 0){
                dup2(Pipes[count][WRITE],WRITE);
                close(Pipes[count][READ]); // abrir canal de lectura
                // CLOSE OTHERS PIPES
                int path = 1;
                while(path < allpipes){
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                }
                execvp(ArgsCommand[0][0],ArgsCommand[0]); 
                printf("error1");  
                // PROCESO 2
            }
            count++;
            if(allpipes > 1){
                cout << "Entre" << endl;
                while(count < allpipes - 1){
                    if(fork()==0){
                        dup2(Pipes[count - 1][READ],READ); // abrir canal de lectura
                        dup2(Pipes[count][WRITE],WRITE); // abrir canal de escritura
                        for(int i = 0; i < allpipes; i++){
                            if(i == count - 1)
                                close(Pipes[i][WRITE]);
                            else if(i == count) 
                                close(Pipes[i][READ]);
                            else{
                                close(Pipes[i][READ]);
                                close(Pipes[i][WRITE]);
                            }
                        }
                        execvp(ArgsCommand[count][0],ArgsCommand[count]); 
                        printf("error2");
                    }
                    count++;
                }
            }
            if(fork()==0){
                // PROCESO 3
                dup2(Pipes[count][READ],READ); // abrir canal de lectura
                close(Pipes[count][WRITE]);
                int path = 0;
                //el proceso 3 no utiliza el pipe1
                while(path < allpipes){
                    if(path == count) continue;
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                }
                cout << ArgsCommand[1] << endl;
                execvp(ArgsCommand[count][0],ArgsCommand[count]); 
                printf("error3");
            }
            //cout << "count : " << count << endl;
            for (int i = 0; i < allpipes; i++){
                close(Pipes[i][WRITE]);
                close(Pipes[i][READ]);                
            }
            for(int l = 0 ; l < allpipes; l++) wait(NULL);

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
/*
for(i; i < All.size(); i++){
    for (int j = 0; j < All[i].size(); j++)
    {
        cout << ArgsCommand[i][j] << " ";// = strdup(All[i][j].c_str()); 
    }
    cout << endl; //ArgsCommand[i]
}*/
