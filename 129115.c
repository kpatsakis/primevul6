static void fill_prstatus(struct elf_prstatus *prstatus,
		struct task_struct *p, long signr)
{
	prstatus->pr_info.si_signo = prstatus->pr_cursig = signr;
	prstatus->pr_sigpend = p->pending.signal.sig[0];
	prstatus->pr_sighold = p->blocked.sig[0];
	prstatus->pr_pid = p->pid;
	prstatus->pr_ppid = p->parent->pid;
	prstatus->pr_pgrp = process_group(p);
	prstatus->pr_sid = process_session(p);
	if (thread_group_leader(p)) {
		/*
		 * This is the record for the group leader.  Add in the
		 * cumulative times of previous dead threads.  This total
		 * won't include the time of each live thread whose state
		 * is included in the core dump.  The final total reported
		 * to our parent process when it calls wait4 will include
		 * those sums as well as the little bit more time it takes
		 * this and each other thread to finish dying after the
		 * core dump synchronization phase.
		 */
		cputime_to_timeval(cputime_add(p->utime, p->signal->utime),
				   &prstatus->pr_utime);
		cputime_to_timeval(cputime_add(p->stime, p->signal->stime),
				   &prstatus->pr_stime);
	} else {
		cputime_to_timeval(p->utime, &prstatus->pr_utime);
		cputime_to_timeval(p->stime, &prstatus->pr_stime);
	}
	cputime_to_timeval(p->signal->cutime, &prstatus->pr_cutime);
	cputime_to_timeval(p->signal->cstime, &prstatus->pr_cstime);
}