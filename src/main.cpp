#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sched.h>


#include "../include/highloadgenC.h"
#include "../include/sortC.h"
#include "../include/timeStampC.h"
#include "../include/mathC.h"
#include "../include/pwmC.h"
#include "../include/gpioC.h"

using namespace std;

/* Number of High Load threads */
#define NUM_OF_HL_THREADS 4
/* Number of High Priority threads */
#define NUM_OF_HP_THREADS 1
/* Number of meassures */
#define NUM_OF_MEASURE 1

pthread_mutex_t mutex;
timeStampC *measurePtr;

long getTotalThreadTimeMs(pthread_t id);
long getThreadTimeMs(const clockid_t clockid);
void *bubbleSortThread(void *thread_id);
void *insertionSortThread(void *thread_id);
void *pwmThread(void *thread_id);
void *gpioThread(void *thread_id);
void *permutationsThread(void *thread_id);
void *trigonometryThread(void *thread_id);

void setMeasurePtr(timeStampC *measure) {
	measurePtr = measure;
}

void getStamp(char* description) {
	if(measurePtr != 0) {
		pthread_mutex_lock(&mutex);
		measurePtr->getTimeStamp(description);
		pthread_mutex_unlock(&mutex);
	} else {
		cout << "Main::getStamp:: Measure pointer not exist" << endl;
	}
}

void *ThreadsFunction(void *thread_id) {
	long t_id = (long)thread_id;
	switch ( t_id ) {
		case 0: {
			bubbleSortThread(thread_id);
			break;
		} case 1: {
			insertionSortThread(thread_id);
			break;
		} case 2: {
			trigonometryThread(thread_id);
			break;
		} case 3: {
			permutationsThread(thread_id);
			break;
		} default: {
			permutationsThread(thread_id);
			break;
		}
	}
	pthread_exit((void*)thread_id); 
}

int main (int argc, char** argv) {
	
	int rc;
	int i;

	long 	processtime = 0;
	long 	lastprocesstime = 0;

	/* Threads priority init */
	int 	sched_policy = SCHED_FIFO;	
	int 	priomax = sched_get_priority_max(sched_policy);
	int 	priomin = sched_get_priority_min(sched_policy);	
	
	/* Init Mutex */
	pthread_mutex_init(&mutex, NULL);	

	/* Thread init */
	pthread_t 	threadsHL[NUM_OF_HL_THREADS];
	pthread_attr_t 	attrHL;
	pthread_t 	threadsHP[NUM_OF_HP_THREADS];
	pthread_attr_t 	attrHP;	
	void 		*status;
	pthread_attr_init(&attrHL);
	pthread_attr_init(&attrHP);

	/* Set Threat as jonable */
	pthread_attr_setdetachstate(&attrHL, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setdetachstate(&attrHP, PTHREAD_CREATE_JOINABLE);

	/* Sched struct init 
	struct sched_param spHL;
	struct sched_param spHP;
	pthread_attr_setinheritsched(&attrHL, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attrHL, sched_policy);
	pthread_attr_setinheritsched(&attrHP, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attrHP, sched_policy);
	spHL.sched_priority = priomin;
	spHP.sched_priority = priomax;
	pthread_attr_setschedparam(&attrHL, &spHL);
	pthread_attr_setschedparam(&attrHP, &spHP); */

	/* Time Measure Init */
	timeStampC measure;
	setMeasurePtr(&measure);
	measure.createOutputFile();
	srand(time(NULL));

	for(int m=1; m < NUM_OF_MEASURE+1; m++) {
		
		measure.setMeasureId(m);
		measure.getTimeStamp("Start");

		/* Creating load threads for CPU */ 
		for(i = 0; i < NUM_OF_HL_THREADS; i++) {
			rc = pthread_create(&threadsHL[i], &attrHL, ThreadsFunction, (void *)i);
			if(rc) {
				cout << "Main:: Cannot create thread " << rc << endl;
				exit(-1);
			}
		}

		/* Creating threads with high priority */
		rc = pthread_create(&threadsHP[0], &attrHP, gpioThread,  (void *)0);
		if(rc) {
			cout << "Main:: Cannot create thread " << rc << endl;
			exit(-1);
		}

		cout << "Main:: Threads has been successfully created "<< endl;

		/* Free attribute and wait for the other threads */
		pthread_attr_destroy(&attrHL);
		pthread_attr_destroy(&attrHP);
		
		/* Waiting for complete main thread */
		rc = pthread_join(threadsHP[0], &status);
		if(rc) {
			cout << "Main:: Error: unable to join," << rc << endl;
			exit(-1);
		}
		cout << "Main:: completed thread id :" << 0 ;
		cout << "  exiting with status :" << status << endl;
	
		/* Canceling other thread for load */
		for(i = 0; i < NUM_OF_HL_THREADS; i++ ) {
			pthread_cancel(threadsHL[i]);
			cout << "Main:: Total Thread " << i << " time: " << getTotalThreadTimeMs(threadsHL[i]) << endl;
			rc = pthread_join(threadsHL[i], &status);
			if(rc) {
				cout << "Main:: Error: unable to join," << rc << endl;
				exit(-1);
			}
					
		}

		processtime = getThreadTimeMs(CLOCK_PROCESS_CPUTIME_ID) - lastprocesstime;
		lastprocesstime = processtime;
		cout << "Main:: Process time: " <<  processtime << " ms" << endl;

		measure.getTimeStamp("Finish");
		measure.getTotalDiffTime();
		measure.saveAllTimeStamps();

	}

	measure.closeOutputFile();

	cout << "Main:: program exiting" << endl;
	pthread_exit(NULL);
	return EXIT_SUCCESS;
}

long getThreadTimeMs(const clockid_t clockid) {
	struct timespec time;
	if(clock_gettime(clockid, &time)) {
		cout << "Main:: Error getting time" << endl;
		return 0;
	} else {
		return (time.tv_sec * 1000) + (time.tv_nsec / 1000000 );
	}	
}

long getTotalThreadTimeMs(pthread_t id) {
	clockid_t clock;
	pthread_getcpuclockid(id, &clock);
	return getThreadTimeMs(clock);
}

void endThread(void* thread_id) {
	cout << "Main:: Total Thread " << (long)thread_id << " time: " << getThreadTimeMs(CLOCK_THREAD_CPUTIME_ID) << " ms" << endl;
}

void *pwmThread(void *thread_id) {
	rpiPWM1 pwm(1000.0, 256, 80.0, rpiPWM1::MSMODE);
	double dcycle = 0.0;
	while(dcycle < 99.99) {
		pwm.setDutyCycle(dcycle);
		dcycle += 1;
		usleep(200);
		getStamp("PWM");
	}
	endThread(thread_id);
	pthread_exit((void*)thread_id); 
}

void *gpioThread(void *thread_id) {
	gpioC test(7);
	if(test.gpioPWMInit(measurePtr, 10, 50)) {
		test.gpioPWMStart();
	}

	endThread(thread_id);
	pthread_exit((void*)thread_id); 
}

void *bubbleSortThread(void *thread_id) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);	
	sortC sort(thread_id);
	while(1) {
		sort.setTabSize();
		sort.fillTab();
		sort.bubbleSort();	
	}
	pthread_exit((void*)thread_id); 
	return NULL;	
}

void *insertionSortThread(void *thread_id) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	sortC sort(thread_id);
	while(1) {
		sort.setTabSize();
		sort.fillTab();
		sort.insertionSort();
	}
	pthread_exit((void*)thread_id); 
	return NULL;	
}

void *permutationsThread(void *thread_id) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	mathC perm(thread_id);
	while(1) {
		perm.createArray();
		perm.permutation();
	}
	pthread_exit((void*)thread_id); 
	return NULL;
}

void *trigonometryThread(void *thread_id) {
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	mathC sintan(thread_id);
	while(1) {
		sintan.trigonometry();
	}
	pthread_exit((void*)thread_id); 
	return NULL;
}
