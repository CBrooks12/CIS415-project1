/**
* Created by Chris Brooks
* CBrooks4
* CIS 415 Project 1 part 2
* Worked with Garett Roberts, Howard Lin, Jack Brockway, and Randy Chen
*
*/
#include "p1fxns.h"
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>

int sig = 0;


void set_signal(){
    sig = 1;
}


char* getEnd(char * arg, int start, int end)
{

    int n = end-start;
    char *p = (char *)malloc(sizeof(char)*n); //remember to free
    int i = 0;
    if (p != NULL) {
        for (i = 0; i < n; i++)
            p[i] = arg[start+i];
            
    }
    return p;

}
int main(int argc, char* argv[]){

    char *errbuff;
    int nprocesses, nprocessors;
    //int position;
    struct timeval end;
    struct timeval start;

    errbuff = NULL;
    nprocesses = 0;
    nprocessors = 0;
    if(argc < 2){
    	errbuff = "usage: ./thv1 [--number=<nprocesses>] [--processors=<nprocessors>] --command='command'\n";
    	p1putstr(2, errbuff);
        _exit(0);
    } 
    int x = 0;
    int y = 0;
    char *z;
    int garett = 1024;
    
    char ** buffer = (char**)malloc(sizeof(char*)*garett);
    char* command[garett];
    int i = 0;
    for(i = 0; i < garett; i++){
        buffer[i] = malloc(sizeof(command));
        if(buffer[i] == NULL){
            errbuff = "malloc err";
            p1perror(2, errbuff);
            return -1;
        }
    }
    while(argc >= 2){
       --argc;
       //fprintf(stderr,"%s\n", argv[argc]);
       if(p1strneq(argv[argc],"--number",8) != 0){
            x = p1strchr(argv[argc], '=');
            y = p1strlen(argv[argc]);
            z = getEnd(argv[argc],x+1,y+1);
            nprocesses = p1atoi(z);
            free(z);
            //fprintf(stderr,"%d\n",nprocesses);
        } else if(p1strneq(argv[argc],"--processors",12)!= 0){
            x = p1strchr(argv[argc], '=');
            y = p1strlen(argv[argc]);
            z = getEnd(argv[argc],x+1,y+1);
            nprocessors = p1atoi(z);
            free(z);
        } else if(p1strneq(argv[argc],"--command",9)!= 0){
            x = p1strchr(argv[argc], '=');
            y = p1strlen(argv[argc]);
            z = getEnd(argv[argc],x+1,y+1);
            x = 0;
            i = 0;
            while(x != -1){
                x = p1getword(z, x, buffer[i]);
                if(x==-1)
                    break;
                //fprintf(stderr,"x = %d word %s\n",i,buffer[i]);
                command[i] = buffer[i];
                //fprintf(stderr,"command %s\n",command[i]);
                i++;
            }
            free(z);
        }
    }

    if(nprocesses == 0 && (nprocesses = p1atoi(getenv("TH_NPROCESSORS")))){
    	errbuff = "TH_NPROCESSES err";
    	p1putstr(2, errbuff);
    	_exit(0);
    }
    if(nprocessors == 0 && (nprocesses = p1atoi(getenv("TH_NPROCESSORS")))){
    	errbuff = "TH_NPROCESSORS err";
    	p1putstr(2, errbuff);
        _exit(0);
    }
    
    //Noting the current time (start)
    gettimeofday(&start, NULL); 
    signal(SIGUSR1, set_signal);
    pid_t pid[nprocesses];
	for( i = 0; i < nprocesses; i++){
		pid[i] = fork();
		if(pid[i] < 0){
			errbuff = "failed to create child from fork";
    	    p1putstr(2, errbuff);
			exit(1);
			
		}
		else if(pid[i] > 0){
		}	
		else{
			p1putstr(2, "\0");
			while(!sig){
                pause();
            }
			execvp(command[0], command);
			//exit(1);

		}
	}
	for (i = 0; i < nprocesses; i++) {
        kill(pid[i], SIGUSR1);
        kill(pid[i], SIGSTOP);
        kill(pid[i], SIGCONT);
    }
	for (i = 0; i < nprocesses; i++) {
        waitpid(pid[i], 0, 0);
    }
	


    gettimeofday(&end, NULL); 
    float time_elapsed;
    time_elapsed = (((end.tv_sec - start.tv_sec)*1000000L + end.tv_usec) - start.tv_usec)/1000000.0;
    unsigned int whole = (unsigned int) (time_elapsed);
    unsigned int decimals = (unsigned int) ((time_elapsed - whole) * 1000);
    p1putstr(1,"The elapsed time to execute ");
    p1putint(1, nprocesses);
    p1putstr(1, " copies of ");
    p1putstr(1, *command);
    p1putstr(1, " on ");
    p1putint(1, nprocessors);
    p1putstr(1, " processors is ");
    p1putint(1, whole);
    p1putstr(1,".");
    if(decimals < 10){ //single digit with precision
        p1putstr(1, "00");
    } else if(decimals < 100){ // double digit with precision
        p1putstr(1, "0");
    }
    p1putint(1, decimals);
    p1putstr(1, " seconds\n");


    //free memory
    for(i = 0; i < garett; i++){
        free(buffer[i]);
    }
    free(buffer);
    
    
	//fprintf(stderr, "The elapsed time to execute %d copies of \"%s\" on %d processors is %7.3fsec\n",nprocesses,nprocessors,end-start);
    //errbuff = "done";
    //p1putstr(2, errbuff);
    return 0;
}




