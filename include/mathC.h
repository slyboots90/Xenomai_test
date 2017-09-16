#ifndef MATHC_H_
#define MATHC_H_

#include <math.h>
#include <iostream>
#include <vector>
#include "highloadgenC.h"

using namespace std;

class mathC : public highloadgenC {
	
	public:
		mathC(void *t);
		virtual ~mathC();
		void trigonometry(void);
		void permutation(void);
		void createArray(void);
		
	private:
		vector <int> vint;
				

};


#endif
