/**
* Created by Chris Brooks
* CBrooks4
* CIS 415 Project 1 part 3
* Worked with Garett Roberts, Howard Lin, Jack Brockway, and Randy Chen
*
*/
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "p1fxns.h"

typedef struct node Node;
typedef struct queue Queue;

int setsignal = 0;
int child_dead = 0;
Queue *queue;      
Queue *buffqueue; 
int processors_ = 0;

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
struct node{
	int status;
	pid_t pid;
	Node *next;
};

typedef struct queue{
	Node *tail;
	Node *head;
	int size;
}Queue;

Queue* queue_create()
{
	Queue *newqueue;
	newqueue = (Queue *)malloc(sizeof(Queue));
	if(newqueue == NULL){
		return NULL;
	}

	newqueue->head = NULL;
	newqueue->tail = NULL; 
	newqueue->size = 0;
	return newqueue;

}	
void enqueue(Queue *queue, pid_t childpid, int status)
{
	Node *newnode = (Node *)malloc(sizeof(Node));
	if(newnode == NULL)
		return;

	newnode->pid    = childpid;
	newnode->status = status;
	
	if(queue->size == 0){
		queue->head = newnode;
		queue->tail = newnode;
	} else {
	queue->tail->next      = newnode;
	queue->tail            = newnode;
	queue->tail->next      = NULL;
	}

	queue->size = queue->size + 1;           
} 

Node* dequeue(Queue *queue)
{
	Node *tempnode;

	if(queue->size == 0){
		return NULL;
	}
	
	tempnode    = queue->head;
	queue->head = queue->head->next;
	queue->size = queue->size - 1;
	return tempnode;
}			

int queue_empty(Queue *queue)
{
	if(queue->size == 0){
		return 1;
	}
	return 0;
}

void set_signal()
{
	setsignal = 1;
}

pid_t discard[100];
int aIndex = 0;

void alarm_handl()
{	
	int i;
	for(i = 0; i < processors_; i++){
		if(!queue_empty(queue)){
			Node *tempnode = dequeue(queue);
			if(tempnode->status == 0){
				tempnode->status = 1;
				kill(tempnode->pid,SIGUSR1);
			}
			else if(tempnode->status == 1){
				kill(tempnode->pid,SIGCONT);
			}
			enqueue(buffqueue,tempnode->pid,tempnode->status);
			free(tempnode);
		}
	}
	int q;
	for(i = 0; i < buffqueue->size; i++){
		int flag = 0;
		Node *tempnode = dequeue(buffqueue);
		for(q = 0; q < aIndex; q++){
			if(discard[q] == tempnode->pid){
				free(tempnode);
				flag = 1;
			}
			kill(tempnode->pid,SIGSTOP);
		}
		if(!flag){
			enqueue(queue,tempnode->pid,1);
			free(tempnode);
		}
	}
}
void sigchild()
{
	int status;
	pid_t pid;
	aIndex = 0;	

	while((pid = waitpid(-1,&status,WNOHANG)) > 0){
		if(WIFEXITED(status)){
			discard[aIndex] = pid;
			aIndex++;
		}
	}
}

int main(int argc, char *argv[])
{
	char *errbuff;
	int nprocesses, nprocessors;
	queue     = queue_create();
	buffqueue = queue_create();
	errbuff     = NULL;
	nprocesses  = -1;
	nprocessors = -1;

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
	if(argc <= 1){
			errbuff = "usage: ./thv3 [--number=<nprocesses>] [--processors=<nproccessors>] --command='command'\n"; 
			p1putstr(2, errbuff);
			_exit(EXIT_FAILURE);
	}	
	
	while(argc >= 2){
       --argc;
       if(p1strneq(argv[argc],"--number",8) != 0){
            x = p1strchr(argv[argc], '=');
            y = p1strlen(argv[argc]);
            z = getEnd(argv[argc],x+1,y+1);
            nprocesses = p1atoi(z);
            free(z);
        } else if(p1strneq(argv[argc],"--processors",12)!= 0){
            x = p1strchr(argv[argc], '=');
            y = p1strlen(argv[argc]);
            z = getEnd(argv[argc],x+1,y+1);
            nprocessors = p1atoi(z);
            processors_ = nprocessors;
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
                command[i] = buffer[i];
                i++;
            }
            command[i]=NULL;
            free(z);
        }
    }
	if(nprocesses == -1 && (getenv("TH_NPROCESSES")) == NULL){
		errbuff = "TH_NPROCESSES NOT SET\n";
		p1putstr(2, errbuff);
		_exit(EXIT_FAILURE);

	}else if(nprocesses == -1){
		nprocesses = atoi(getenv("TH_NPROCESSES"));
	}

	if(nprocessors == -1 && (getenv("TH_NPROCESSORS")) == NULL){
		errbuff = "TH_NPROCESSORS NOT SET\n";
		p1putstr(2, errbuff);	
		_exit(EXIT_FAILURE);

	}else if(nprocessors == -1){
		nprocessors = atoi(getenv("TH_NPROCESSORS"));
	}

	struct timeval start,end;
	struct itimerval runtime;
    pid_t child[100];

	runtime.it_value.tv_sec     = 0;
	runtime.it_value.tv_usec    = 250000;
	runtime.it_interval         = runtime.it_value;
	
	signal(SIGUSR1,set_signal);	
	signal(SIGALRM,alarm_handl);
	signal(SIGCHLD,sigchild);

	for(i = 0; i < nprocesses; i++){
	    child[i] = fork();
	    if(child[i] == 0)
	    {
            while(!setsignal){
                pause();
            };
            if(execvp(command[0],command) == -1){	
                errbuff = "exevp() failed";
                p1perror(2,errbuff);
                _exit(EXIT_FAILURE);
            }
            _exit(EXIT_SUCCESS);
	    }
	    else if(child[i] == -1)
	    {
        	errbuff = "fork() failed";
			p1perror(2,errbuff);
			_exit(EXIT_FAILURE);
	    }
	    else
	    {
	        //??
	    }	
	}
	for(i = 0; i < nprocesses; i++){
		enqueue(queue,child[i],0);
	}
    gettimeofday(&start, NULL);

	if(setitimer(ITIMER_REAL,&runtime,NULL) != 0){
		errbuff = "setitimer() failed";
		p1perror(2,errbuff);
		_exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < nprocesses; i++){
		wait(child+i);
	}
	
	while(!queue_empty(queue)){
		free(dequeue(queue));
	}
	
	while(!queue_empty(buffqueue)){
		free(dequeue(buffqueue));
	}		

	free(queue);
	free(buffqueue);
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
	for(i = 0; i < garett; i++){
        free(buffer[i]);
    }
    free(buffer);
	_exit(EXIT_SUCCESS);
}
