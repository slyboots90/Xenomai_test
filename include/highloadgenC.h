#ifndef HIGHLOADGENC_H_
#define HIGHLOADGENC_H_

class highloadgenC {
	
	public:
		highloadgenC();
		highloadgenC(void *t);
		virtual ~highloadgenC();
		void setThreadId(void *t);
		long getThreadId();
		void showThreadId();
		
	private:
		long threadId;

};


#endif
