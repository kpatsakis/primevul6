DEFINE_IDTENTRY_RAW(xenpv_exc_debug)
{
	/*
	 * There's no IST on Xen PV, but we still need to dispatch
	 * to the correct handler.
	 */
	if (user_mode(regs))
		noist_exc_debug(regs);
	else
		exc_debug(regs);
}