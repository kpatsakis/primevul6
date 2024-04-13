static void event_status(struct timeval *wake)
{
#if !defined(HAVE_PTHREAD_H) && defined(WNOHANG)
	int argval;
#endif

	if (debug_flag == 0) {
		if (just_started) {
			radlog(L_INFO, "Ready to process requests.");
			just_started = FALSE;
		}
		return;
	}

	if (!wake) {
		radlog(L_INFO, "Ready to process requests.");

	} else if ((wake->tv_sec != 0) ||
		   (wake->tv_usec >= 100000)) {
		DEBUG("Waking up in %d.%01u seconds.",
		      (int) wake->tv_sec, (unsigned int) wake->tv_usec / 100000);
	}


	/*
	 *	FIXME: Put this somewhere else, where it isn't called
	 *	all of the time...
	 */

#if !defined(HAVE_PTHREAD_H) && defined(WNOHANG)
	/*
	 *	If there are no child threads, then there may
	 *	be child processes.  In that case, wait for
	 *	their exit status, and throw that exit status
	 *	away.  This helps get rid of zxombie children.
	 */
	while (waitpid(-1, &argval, WNOHANG) > 0) {
		/* do nothing */
	}
#endif

}