static void xen_end_context_switch(struct task_struct *next)
{
	xen_mc_flush();
	paravirt_end_context_switch(next);
}