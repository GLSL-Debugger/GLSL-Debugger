#ifndef _OSX_PTRACE_DEFS_H_
#define _OSX_PTRACE_DEFS_H_

enum ptracereq {
	PTRACE_TRACEME = 0,		/* 0, by tracee to begin tracing */
	PTRACE_CHILDDONE = 0,	/* 0, tracee is done with his half */
	PTRACE_PEEKTEXT,		/* 1, read word from text segment */
	PTRACE_PEEKDATA,		/* 2, read word from data segment */
	PTRACE_PEEKUSER,		/* 3, read word from user struct */
	PTRACE_POKETEXT,		/* 4, write word into text segment */
	PTRACE_POKEDATA,		/* 5, write word into data segment */
	PTRACE_POKEUSER,		/* 6, write word into user struct */
	PTRACE_CONT,			/* 7, continue process */
	PTRACE_KILL,			/* 8, terminate process */
	PTRACE_SINGLESTEP,		/* 9, single step process */
	PTRACE_ATTACH,			/* 10, attach to an existing process */
	PTRACE_DETACH,			/* 11, detach from a process */
	PTRACE_SIGEXC,			/* 12, signals as exceptions for current process */
	PTRACE_THUPDATE,		/* 13, signal for thread */
	PTRACE_ATTACHEXC		/* 14, attach to running process with signals as exceptions */
};

#endif
