

#include "../include/highloadgenC.h"

#include <math.h>
#include <iostream>

using namespace std;

highloadgenC::highloadgenC() {

}

highloadgenC::highloadgenC(void *t) {
	setThreadId(t);
}

highloadgenC::~highloadgenC() {

}

void highloadgenC::setThreadId(void *t) {
	threadId = (long)t;
}

long highloadgenC::getThreadId() {
	return threadId; 
}

void highloadgenC::showThreadId() {
	cout << "highloadgenC::showThreadId:: " << threadId << endl;
}

