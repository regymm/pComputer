/**
 * File              : process.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.03.28
 */
#ifndef PSEUDOS_PROCESS_H
#define PSEUDOS_PROCESS_H

#define PROC_NUM_MAX 256

//#define PROC_STATE_READY 0b000000
#define PROC_STATE_RUNNING 0b000000
#define PROC_STATE_SENDING 0b000010
#define PROC_STATE_RECEIVING 0b000100
#define PROC_STATE_TERMINATE 0b010000
#define PROC_STATE_READY 0b100000
#define PROC_STATE_UNDEF 0b111111

#define IPC_SEND 34
#define IPC_RECEIVE 37
#define IPC_BOTH 340
#define IPC_TARGET_ANY (-363)
#define IPC_TARGET_NONE (-364)

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
	int type; // type is dumb, to be removed
	int function;
	int source;
	int param[8]; // param[1] for first parameter, etc. 
	//int type;
	//union {
		//int integer;
		//void* pointer;
	//} ;
} Message;

typedef struct ProcessStruct{
	short pid; // minus no such process
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

// global singleton ProcManager -- procmanager
// functions inside use procmanager directly
typedef struct ProcManagerStruct{
	// process hash table, use pid as index
	Process proc_table[PROC_NUM_MAX];
	unsigned short proc_number;
	unsigned short proc_running;
	unsigned short proc_max;
	// main scheduler
	void (* schedule) ();
	// Process* and pid conversion
	Process* (* pid2proc) (short pid);
	short (* proc2pid) (Process* proc);
	// next process to run in schedule
	short (* get_next) ();
	int do_start;

	void (* block) (Process* proc);
	void (* unblock) (Process* proc);

}ProcManager;

void ProcManagerInit();

//int sendrec(int function, int src_dest, Message* msg, Process* proc);
int sendrec(int function, int src_dest, Message* msg);

// TODO: a unified syscall for all functions, with sendrec the
// most basic one
int sendrec_syscall(int function, int src_dest, Message* msg);

// interface asm function for procs -- user(and kernel-side)
// runs in user mode(if the cpu has), uses ecall
// after this "returns"(may block some time before return), 
//  the IPC has done
// and literally the only syscall in pseudos
extern int syscall_asm(int function, int src_dest, Message* msg);

// TODO: this shouldn't be public
short _getavailpid();


#endif
