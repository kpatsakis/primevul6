caproc(struct privsep *ps, struct privsep_proc *p)
{
	return (proc_run(ps, p, procs, nitems(procs), ca_run, NULL));
}