/**
 * File              : process.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.02.24
 */
#ifndef PSEUDOS_PROCESS_H
#define PSEUDOS_PROCESS_H

#define PROC_NUM_MAX 10

#define PROC_STATE_READY 1
#define PROC_STATE_RUNNING 2
#define PROC_STATE_SENDING 3
#define PROC_STATE_RECEIVING 4
#define PROC_STATE_TERMINATE 5

#define IPC_SEND 4444
#define IPC_RECEIVE 7777777
#define IPC_TARGET_ANY (-333)
#define IPC_TARGET_NONE (-4444)

typedef struct {
	int a0;
	int a1;
	int a2;
	int a3;
	int a4;
	int a5;
	int a6;
	int a7;

	int s0;
	int s1;
	int s2;
	int s3;
	int s4;
	int s5;
	int s6;
	int s7;
	int s8;
	int s9;
	int s10;
	int s11;

	int t0;
	int t1;
	int t2;
	int t3;
	int t4;
	int t5;
	int t6;

	int ra;
	int sp;
	int gp;
	int tp;
} __attribute((packed)) StackFrame;

typedef struct {
	int source;
	int type;
	union {
		int integer;
		void* pointer;
	} ;
} Message;

typedef struct ProcessStruct{
	unsigned short pid; // 0 means end of processes
	char name[16];

	unsigned short state;
	void (* pc)();
	StackFrame regs;

	// IPC
	int p_sendto; // negative reserved for things like ANY
	int p_recvfrom;
	Message* p_msg;
	struct ProcessStruct* queue_sending; // a linked list of procs sending to this proc
	struct ProcessStruct* queue_sending_next;

	//unsigned int stacksize;
	//unsigned int ticks;
	//unsigned short priority;
	// to be continued ...
}Process;

typedef Process* ProcTable;

typedef struct ProcManagerStruct{
	ProcTable proc_table;
	unsigned short proc_number;
	unsigned short proc_running;
	unsigned short proc_max;
	void (* schedule) (struct ProcManagerStruct* pm);
	// find Process* using pid
	Process* (* find) (struct ProcManagerStruct* pm, unsigned short pid);
	// next process to run in schedule
	unsigned short (* get_next) (struct ProcManagerStruct* pm);
	int do_start;

}ProcManager;

void ProcManagerInit(ProcManager* pm, ProcTable pt);

int sendrec(int function, int src_dest, Message* msg, Process* proc);


#endif
