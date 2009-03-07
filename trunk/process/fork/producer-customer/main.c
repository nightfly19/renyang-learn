#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int Queue[11];
int Counter;
void Clear_Queue(){
	int i;
	for( i = 0;i<11;i++){
		Queue[i]=0;
	}
}

int random_at_range ( int lower, int upper)
{
	return rand ( ) % (upper-lower+1) + lower;
}

void Producer(){
	int i;
	Counter=random_at_range(1,10);
	Clear_Queue();
	for(i=0;i<=Counter;i++){
		Queue[i] = random_at_range(1,100);
	}
}

void Customer(){
	int i;
	if(Counter==0){
		printf("Queue is null\n");
		return;
	}
	for(i=0;i<=Counter;i++){
		printf("Queue[%d]=%d\n",i,Queue[i]);
	}
}

int main()
{
	srand ( (unsigned) time ( NULL ) );
	int pid;
	pid = fork();
	if(pid == 0){
		while(1){
			Producer();
			printf("excute Producer\n");
			sleep(1);
		}
		exit(1);
	}
	else if(pid >0){
		while(1){
			Customer();
			printf("excute customer\n");
			sleep(1);
		}
	}
}
