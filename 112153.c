ExitPostmaster(int status)
{
#ifdef HAVE_PTHREAD_IS_THREADED_NP

	/*
	 * There is no known cause for a postmaster to become multithreaded after
	 * startup.  Recheck to account for the possibility of unknown causes.
	 * This message uses LOG level, because an unclean shutdown at this point
	 * would usually not look much different from a clean shutdown.
	 */
	if (pthread_is_threaded_np() != 0)
		ereport(LOG,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg_internal("postmaster became multithreaded"),
				 errdetail("Please report this to <%s>.", PACKAGE_BUGREPORT)));
#endif

	/* should cleanup shared memory and kill all backends */

	/*
	 * Not sure of the semantics here.  When the Postmaster dies, should the
	 * backends all be killed? probably not.
	 *
	 * MUST		-- vadim 05-10-1999
	 */

	proc_exit(status);
}