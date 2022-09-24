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
#include <sys/wait.h>
#include <algorithm>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

const int READ = 0; // variable para pipe
const int WRITE = 1; // variable para pipe

void sig_handler(int sig){      // manejador de signals
    cout << " Decia continuar la ejecución de la Shell: (Y/N)" << endl;    
    char Answer;
    while(1){
        cout << "Coloque su respuesta : ";
        cin >> Answer;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(Answer == 'N'){
            printf("Adios");
            exit(0);
        }else if(Answer == 'Y'){
            cin.clear();
            int fdflags;
            fdflags = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, fdflags | O_NONBLOCK);
            printf("ERROR");
            while (getchar()!=EOF){
            };
            printf("ERROR");
            fcntl(STDIN_FILENO, F_SETFL, fdflags);
            break;
        }
    }
}
void sigUSR(int sig){
    if(sig == SIGUSR1){
        cout << "Estoy aca " ;
        exit(0);
    }
}

int main()
{
    /*struct sigaction sa;
    sa.sa_handler = sig_handler;
    int k = 0;
    sigaction(SIGINT, &sa, NULL);*/
    signal(SIGINT,sig_handler);
    signal(SIGUSR1, sigUSR);
    
    int pid=getpid();      //Process ID of itself
    kill(pid,SIGUSR1);       

    struct rusage start,end;
    long int usertime, systime;
    long maxrss = 0;            
    vector<vector<string> > All;
    vector<string> command;
    string instruction;                                  
    //vector<vector<string> > All;
    bool pront = true;
    cout << "$";
    ofstream archivo;
    long long maxim;
    //archivo.clear();
    bool dates = false; // Flag of the recurses, if is true. write , else close
    //while(1){
      //  cout << "Ingrese Comando" << endl;
    while (getline(cin,instruction)){ 
        cout << "comando : " << instruction<< endl;
        maxim = 0;
        //instruction = "ls";
        //if(pront) cout << "$";
        pront = false;
        string Chain1;
        if(instruction == "exit"){
            return 0;
        }else if(instruction.size() == 0 ){
            pront = true;
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
        }
        getrusage(RUSAGE_SELF , &start);
        // Case only one command

        // Caso para abrir el archivo recursos.log, es decir == 3 para pdoer verifficar que sea ese comandoapra comenzar la escritura de datos
        // si el tamano es 2 es para cortar la escirutra 
        if(All[0].size() == 3){
            string L = All[0][0] + " " + All[0][1] + " " + All[0][2];
            if(L == "usorecursos start recursos.log"){
                dates = true;
                string nombreArchivo = "recursos.log";
		                            // OPEN FILE
		        archivo.open(nombreArchivo.c_str(), fstream::out);
		                        // WRITE IN THE DILE
		        archivo << "comando" << '\t' << "tuser" << '\t'<< "tsys" << '\t' << "maxrss" << '\t' << endl;
                All.clear();
                cout << endl;
                cout << "$";   
                continue;
            }
        }else if(All[0].size() == 2 && dates == true){
            string L = All[0][0] + " " + All[0][1];
            if(L == "usorecursos stop" && dates == true){
                archivo.close();
                cout << endl;
                cout << "$";   
                All.clear();
                continue;
            }
        }

// -------------------  CASO DE UNA LIENA DE COMANDO COMO ls - la sin necesidad de pipe's ----------------------- //

        if( All.size() <= 1){
            cout << "Cai aca : " << instruction << endl;
            char *ArgsCommand[All[0].size() + 1];
            int i = 0;
            for(i; i < All[0].size(); i++){
                ArgsCommand[i] = strdup(All[0][i].c_str());
            }
            ArgsCommand[i] = NULL;
            int pid = fork();
            if(pid == 0){        
                pront = true;
                int exeret = execvp(ArgsCommand[0],ArgsCommand);
                
                //fprintf(stderr, "%s\n", explain_execvp(ArgsCommand[0],ArgsCommand));
                exit(EXIT_FAILURE);
            }else if(pid < 0){      
                cout << "ERROR";
                // ERROR;
            }
            else{
                wait(NULL);

            }
        }else{     
            
            
            
    // ----------------- FIND PIPE IN THE COMMAND LINE ----------------------//
                    // -- DIVIDE COMMANDO FOR THE PIPES --//

            char *ArgsCommand[All.size()][maxim + 1];
            for(int i = 0; i < All.size(); i++){
                for (int j = 0; j < All[i].size(); j++)
                {
                    ArgsCommand[i][j] = strdup(All[i][j].c_str()); 
                    //cout << ArgsCommand[i][j] << " ";
                }
                ArgsCommand[i][All[i].size()] = NULL;
            }

        //---------------  INICIALIZED PIPES -----------------------//

            long long allpipes  = All.size() - 1;
            int Pipes[allpipes][2];
            for(int k = 0; k < allpipes; k++)  pipe(Pipes[k]);
                    
                    // CHILDREN PROCESS FOR THE PIPES
            int count  = 0;


        // ----------------------- FIRST PROCESS -----------------//               
            if(fork() == 0){
                dup2(Pipes[0][WRITE],WRITE); // OPEN CHANNEL WRITE
                close(Pipes[0][READ]);       // CLOSE CHANNEL READ
                int path = 1;
                while(path < allpipes){        // CLOSE OTHER PIPE TAHT NOT UTILIZED
                    close(Pipes[path][READ]);
                    close(Pipes[path][WRITE]);
                    path++;
                }
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

        getrusage(RUSAGE_SELF , &end);              // GET USER AND SYSTEM TIME, AND MAXRRS
        usertime = (end.ru_utime.tv_sec - start.ru_utime.tv_sec)*1000000 + end.ru_utime.tv_usec - start.ru_utime.tv_usec;
        systime = (end.ru_stime.tv_sec - start.ru_stime.tv_sec)*1000000 + end.ru_stime.tv_usec - start.ru_stime.tv_usec;
        string comando = "";


        //  ---------------  GET THE COMMAND LINE ENTERED ------------------//
        for ( int l = 0; l < All.size(); l++){
            for (int m = 0; m < All[l].size(); m++){
                comando += All[l][m] + " ";
            }
        }
        archivo << comando << '\t' << usertime << '\t'<< systime << '\t' << end.ru_maxrss << '\t' << endl;

        All.clear();   
        //pront = true;  
        cout << endl;
        instruction.clear();
     
        cout << "$";     
        //break;
    }

  //}
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
