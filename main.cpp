#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <istream>
#include <sstream>
#include <fstream>
#include <limits>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <algorithm>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
int x = 0;
const int READ = 0; // variable para pipe
const int WRITE = 1; // variable para pipe

void sigUSR(int sig){
    if(sig == SIGUSR1){
        cout << "Papi, Comence a realizar el comando, en un par de segundos estara listo" << endl;
        sleep(1);
        kill(getppid(),SIGUSR2);
    }else if(sig == SIGUSR2){
        cout << "Okey, mi muchacho, esperando a que lo termines... " << endl;
    }
}
int Input_Command(string instruction, long long &maxim, vector<vector<string>> &All){
        vector<string> command;
        string Chain1;
        if(instruction == "exit"){
            exit(0);
        }else if(instruction.size() == 0 ){
            All.clear();   
            cout << "shell$";     
            return 1;
        }
        stringstream S1(instruction);
        while (getline(S1 , Chain1 , '|'))
        {
            string Chain2;
            stringstream S2(Chain1); 
            while(getline(S2, Chain2 , ' ')){ // como                 estas
                string chars = "\"";
                Chain2.erase(remove_if(Chain2.begin(), Chain2.end(),[&chars](const char &c) {return chars.find(c) != string::npos;}),Chain2.end());
                command.push_back(Chain2);
            }
            command.erase(std::remove(command.begin(), command.end(), ""), command.end());

            long long m = command.size();
            maxim = max(m, maxim);
            All.push_back(command);
            command.clear();
        }
        return 0;
}

void Command_without_pipe(vector<vector<string>> All){
    char *ArgsCommand[All[0].size() + 1];
    int i = 0;
    for(i; i < All[0].size(); i++){
        ArgsCommand[i] = strdup(All[0][i].c_str());
    }
    ArgsCommand[i] = NULL;
    int pid = fork();
    if(pid == 0){        
        signal(SIGUSR1,sigUSR);    
        kill(getppid(),SIGUSR2);
        sleep(1);
        //pront = true;
        int exeret = execvp(ArgsCommand[0],ArgsCommand);
        exit(EXIT_FAILURE);
    }else if(pid < 0){      
        cout << "ERROR AL CREAR HIJO";
    }
    else{
        signal(SIGUSR2,sigUSR);
        kill(pid,SIGUSR1);
        wait(NULL);
    }
}

void sig_handler(int sig){      // manejador de signals
    cout << " Decia continuar la ejecución de la Shell: (Y/N)" << endl;    
    char Answer;
    while(1){
        x = 0;
        cout << "Coloque su respuesta : ";
        cin >> Answer;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(Answer == 'N'){
            printf("Adios\n");
            exit(0);
        }else if(Answer == 'Y'){
            cin.clear();
            int fdflags;
            fdflags = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, fdflags | O_NONBLOCK);
            while (getchar()!=EOF){
            };
            fcntl(STDIN_FILENO, F_SETFL, fdflags);
            break;
        }
    }
}



int main()
{
    signal(SIGINT,sig_handler);    
    signal(SIGUSR1,sigUSR);    
    struct rusage start,end;
    long int usertime, systime;
    long maxrss = 0;            
    vector<vector<string> > All;
    string instruction;                                  
    bool pront = true;
    cout << "Shell$";
    ofstream archivo;
    long long maxim;
    bool dates = false; // Flag of the recurses, if is true. write , else close
    while (getline(cin,instruction)){ 
        maxim = 0;
        int ret = Input_Command(instruction,maxim,All);
        if(ret == 1){
            instruction.clear();
            continue;
        }   
        getrusage(RUSAGE_CHILDREN , &start);
        // Case only one command

// ----------------------- CHECK IF IT IS "usorecursos start 'archivo'" ---------------------------------//

        // Caso para abrir el archivo recursos.log, es decir == 3 para pdoer verifficar que sea ese comandoapra comenzar la escritura de datos
        // si el tamano es 2 es para cortar la escirutra 
       
        if(All[0].size() == 3){
            string L = All[0][0] + " " + All[0][1];// + " " + All[0][2];
            if(L == "usorecursos start"){
                dates = true;
                string nombreArchivo = All[0][2];
		        // OPEN FILE
		        archivo.open(nombreArchivo.c_str(), fstream::out);
		        // WRITE IN THE DILE
		        archivo << "comando" << '\t' << "tuser" << '\t'<< "tsys" << '\t' << "maxrss" << '\t' << endl;
                All.clear();
                cout << "Shell$";   
                continue;
            }   
        }else if(All[0].size() == 2 && dates == true){
            string L = All[0][0] + " " + All[0][1];
            if(L == "usorecursos stop"){
                dates = false;
                archivo.close();
                cout << "Shell$";   
                All.clear();
                continue;
            }
        }

// -------------------  CASO DE UNA LIENA DE COMANDO COMO ls - la sin necesidad de pipe's ----------------------- //

        if( All.size() <= 1){
            Command_without_pipe(All);
        }else{     
            
    // ----------------- FIND PIPE IN THE COMMAND LINE ----------------------//
                    // -- DIVIDE COMMANDO FOR THE PIPES --//

            char *ArgsCommand[All.size()][maxim + 1];
            for(int i = 0; i < All.size(); i++){
                for (int j = 0; j < All[i].size(); j++)
                {
                    ArgsCommand[i][j] = strdup(All[i][j].c_str()); 
                   // cout << ArgsCommand[i][j] << " ";
                }
                //cout <<  endl;
                ArgsCommand[i][All[i].size()] = NULL;
            }

        //---------------  INICIALIZED PIPES -----------------------//

            long long allpipes  = All.size() - 1;
            int Pipes[allpipes][2];
            for(int k = 0; k < allpipes; k++)  pipe(Pipes[k]);
                    
                    // CHILDREN PROCESS FOR THE PIPES
            int count  = 0;


        // ----------------------- FIRST PROCESS -----------------// 
            //pid_t pid1 = fork();   
            //kill(pid1,SIGUSR1);
            if(fork() == 0){
                //signal(SIGUSR2,sigUSR);
                dup2(Pipes[0][WRITE],WRITE); // OPEN CHANNEL WRITE
                close(Pipes[0][READ]);       // CLOSE CHANNEL READ
                int path = 1;
                while(path < allpipes){        // CLOSE OTHER PIPE TAHT NOT UTILIZED
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }
                //kill(getppid(),SIGUSR1);
                execvp(ArgsCommand[0][0],ArgsCommand[0]); // EXECUTING COMMAND
                printf("error1");           // ERROR 
            }

        //------------------  PROCESS 2 OR MORE ---------------------//

            if(allpipes > 1){

                while(count < allpipes - 1){
                    if(fork()==0){
                        dup2(Pipes[count][READ],READ); // OPEN CHANNEL READ
                        dup2(Pipes[count + 1][WRITE],WRITE); // OPEN CHANNEL WRITE
                        for(int i = 0; i < allpipes; i++){      // CLOSE OTHER PIPE TAHT NOT UTILIZED
                            if(i == count)
                                close(Pipes[i][WRITE]);
                            else if(i == count + 1) 
                                close(Pipes[i][READ]);
                            else{
                                close(Pipes[i][READ]);
                                close(Pipes[i][WRITE]);
                            }
                        }
                    execvp(ArgsCommand[count + 1][0],ArgsCommand[count + 1]); // EXECUTING COMMAND
                    printf("error2");   // ERROR 
                }
                count++;
                }
            }

    // ----------------------------------- FINAL PROCESS ----------------------------------// 
            //pid_t pid3 = fork();
            if(fork()==0){
                dup2(Pipes[count][READ],READ);      // READ LAST PIPE
                close(Pipes[count][WRITE]);         // CLOSE CHANNEL WRITE OF THE LAST PIPE
                int path = 0;

                while(path < allpipes){         // CLOSE THER PIPES
                    if(path == count){
                        path++;
                        continue;
                    }
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }
                execvp(ArgsCommand[count + 1][0],ArgsCommand[count + 1]);   // EXECUTE COMMAND
                printf("error3");           // ERROR
            }
            for (int i = 0; i < allpipes; i++){
                close(Pipes[i][WRITE]);
                close(Pipes[i][READ]);                
            }

            for(int l = 0 ; l < All.size(); l++) wait(NULL); // WAIT CHILDREN
        }


        getrusage(RUSAGE_CHILDREN , &end);              // GET USER AND SYSTEM TIME, AND MAXRRS
        usertime = (end.ru_utime.tv_sec - start.ru_utime.tv_sec)*1000000 + end.ru_utime.tv_usec - start.ru_utime.tv_usec;
        systime = (end.ru_stime.tv_sec - start.ru_stime.tv_sec)*1000000 + end.ru_stime.tv_usec - start.ru_stime.tv_usec;
        string comando = "";


        //  ---------------  GET THE COMMAND LINE ENTERED ------------------//

        for ( int l = 0; l < All.size(); l++){
            for (int m = 0; m < All[l].size(); m++) comando += All[l][m] + " ";
            
        }
        
        archivo << comando << '\t' << usertime << '\t'<< systime << '\t' << end.ru_maxrss << '\t' << endl;

        All.clear();   
        instruction.clear();
        cout << "Shell$";     
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
for(i; i < All.size(); i++){pid1
    for (int j = 0; j < All[i].size(); j++)
    {
        cout << ArgsCommand[i][j] << " ";// = strdup(All[i][j].c_str()); 
    }
    cout << endl; //ArgsCommand[i]
}*/
        /*//instruction = "ls";
        //if(pront) cout << "shell$";
        //pront = false;
        string Chain1;
        if(instruction == "exit"){
            return 0;
        }else if(instruction.size() == 0 ){
            pront = true;
            All.clear();   
            //pront = true;  
            //cout << endl;
            instruction.clear();   
            cout << "$";     
            continue;
        }
        stringstream S1(instruction);
        while (getline(S1 , Chain1 , '|'))
        {
            string Chain2;
            stringstream S2(Chain1); 
            while(getline(S2, Chain2 , ' ')){ // como                 estas
                command.push_back(Chain2);
            }
            command.erase(std::remove(command.begin(), command.end(), ""), command.end());
            long long m = command.size();
            maxim = max(m, maxim);
            All.push_back(command);
            command.clear();
        }*/