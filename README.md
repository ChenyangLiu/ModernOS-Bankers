# ModernOS - Bankers
It's one of the experiment in OS course, Tsinghua University. It solves the bank teller service problem based on mutex and semaphore.
## Input
CNO, EnterTime, NeedTime

example:

	1 1 10  
	2 5 2  
	3 6 3  
	4 6 4  
	5 3 4

## Output
CNO, EnterTime, ServeTime, ExitTime, TNO

example:

	CNo. 2 Enter: 5 Serve: 5 Exit: 7 TNo. 3  
	CNo. 5 Enter: 3 Serve: 3 Exit: 7 TNo. 2  
	CNo. 3 Enter: 6 Serve: 7 Exit: 10 TNo. 2  
	CNo. 4 Enter: 6 Serve: 7 Exit: 11 TNo. 3  
	CNo. 1 Enter: 1 Serve: 1 Exit: 11 TNo. 1

## Version
V1:'theBanker_v1.cpp'  
It uses an int time to sycn.  
It should crush when lots of customers thread created.  
And it has the same running time when the input is different.

V2:'theBanker_v2.cpp'  
It include events to control time lapse.  
Use different time when having different input.  
Using Sleep(0) to switch process.  
But there are less than 10% probabilities  getting wrong answer or being crushed, since a process can't wake instantly when the semaphore is released in Win.  
Another reason is that it's too diffcult to add time factor in IPC for me.
