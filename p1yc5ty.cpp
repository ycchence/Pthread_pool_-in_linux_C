#include <stdio.h>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstdio>
#include <sys/mman.h>
#include<iostream>


int number=1024;  // the number of integers



typedef void (*al) (void *data);
int x[4096], n1[2048], n2[2048], n3=0, n4=1, shift=0,shift2=0;
int counter=0;

struct work{
	al alg;
	void *data;
	struct work *link;	
};
struct pool{        
        int n;
	pthread_mutex_t m;
	pthread_cond_t c;
	struct work* table;
	bool s;
	pthread_t *id;
};	

static struct pool *p = 0;

void* go(void *ddata)
{
	struct work *now;
	while(1)
	{	
		pthread_mutex_lock(&(p -> m));
		if(p -> s == 1 && p -> table == 0)
		{
			pthread_cond_wait(&(p -> c), &(p -> m));
		}
		if(p -> s == 0 && p -> table == 0)		
		{
			pthread_mutex_unlock(&(p -> m));
			pthread_exit(0);
		}
		now = p -> table;
		p -> table = p -> table -> link;
		pthread_mutex_unlock(&(p -> m));
		(now -> alg)(now -> data);
		free(now);
		now = 0;
	}
}



void startpool(int nn){
	p = (struct pool*) malloc(sizeof(struct pool));
	pthread_mutex_init(&(p -> m), 0);
	pthread_cond_init(&(p -> c), 0);
	p -> table = 0;
	p -> s = 1;
	p -> n = nn;
	p -> id = (pthread_t*)malloc(sizeof(pthread_t)*p -> n);	
        for(int i = 0; i < p -> n; i++)
	    pthread_create(&(p -> id[i]), 0, go, 0);
}


	
void que(al aalg)
{
	struct work* add = (struct work*) malloc(sizeof(struct work));
	add -> alg = aalg;
	add -> link = 0;			
	pthread_mutex_lock(&(p -> m));
	struct work* next = p -> table;
	if(next == 0)
		p -> table = add;
	else
	{
		while(next -> link != 0)
                    next = next -> link;
		next -> link = add;
	}
	pthread_mutex_unlock(&(p -> m));
	pthread_cond_signal(&(p -> c));
}


void algor(void *ddata)
{
        pthread_mutex_lock(&(p -> m));
        for(int id=0; id<n4;id++){
            n1[id]=x[shift];
            shift=shift+1;
        }
        for(int jd=0; jd<n4;jd++){
            n2[jd]=x[shift];
             shift=shift+1;
        }
        int i=0,j=0;
        for(int k=0; k<n4*2; k++){
            if(i==n4){
                x[shift2]=n2[j];
                shift2++;
                j++;
            }
            else if(j==n4){
                x[shift2]=n1[i];
                shift2++;
                i++;
            }
            else{
                if(n1[i]<n2[j]){
                    x[shift2]=n1[i];
                    shift2++;
                    i++;
                 }    
                else{
                    x[shift2]=n2[j];
                    shift2++;
                    j++;
                 }                  
            }
            
        }
           
     counter++;
     if(counter==number)
     {
     n4=n4*2;
     number=number/2;
     counter=0;
     shift=0;
     shift2=0;}

        pthread_mutex_unlock(&(p -> m));  
      
}


int main()
{
    int  N_array=number;
    for (int k=1;k<4096; ++k) {
        x[k]=rand() % 2000 -1000;
    }
   /* FILE *ff;                          This program failed to open a file.
   ff=fopen("4096.txt","r");
    if(ff==NULL)
         printf("fail\n");
    k=0;
    while(fscanf(ff,"%d",x[k])!=EOF);
        k++;
    number= k;*/
    number=number/2;
    startpool(number);
    while (n3<N_array/2-1){
        for(int i2 = 0; i2 < number; i2++)
	{
	    que(algor);
	}
        n3++;  
    }
    for (int ans=0; ans<N_array; ans++){
        printf("%d ",x[ans]);
        if (ans%10==0)
             printf("\n");
    }
    return 0;
}
