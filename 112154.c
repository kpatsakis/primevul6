canAcceptConnections(int backend_type)
{
	CAC_state	result = CAC_OK;

	/*
	 * Can't start backends when in startup/shutdown/inconsistent recovery
	 * state.  We treat autovac workers the same as user backends for this
	 * purpose.  However, bgworkers are excluded from this test; we expect
	 * bgworker_should_start_now() decided whether the DB state allows them.
	 */
	if (pmState != PM_RUN && pmState != PM_HOT_STANDBY &&
		backend_type != BACKEND_TYPE_BGWORKER)
	{
		if (Shutdown > NoShutdown)
			return CAC_SHUTDOWN;	/* shutdown is pending */
		else if (!FatalError && pmState == PM_STARTUP)
			return CAC_STARTUP; /* normal startup */
		else if (!FatalError && pmState == PM_RECOVERY)
			return CAC_NOTCONSISTENT;	/* not yet at consistent recovery
										 * state */
		else
			return CAC_RECOVERY;	/* else must be crash recovery */
	}

	/*
	 * "Smart shutdown" restrictions are applied only to normal connections,
	 * not to autovac workers or bgworkers.  When only superusers can connect,
	 * we return CAC_SUPERUSER to indicate that superuserness must be checked
	 * later.  Note that neither CAC_OK nor CAC_SUPERUSER can safely be
	 * returned until we have checked for too many children.
	 */
	if (connsAllowed != ALLOW_ALL_CONNS &&
		backend_type == BACKEND_TYPE_NORMAL)
	{
		if (connsAllowed == ALLOW_SUPERUSER_CONNS)
			result = CAC_SUPERUSER; /* allow superusers only */
		else
			return CAC_SHUTDOWN;	/* shutdown is pending */
	}

	/*
	 * Don't start too many children.
	 *
	 * We allow more connections here than we can have backends because some
	 * might still be authenticating; they might fail auth, or some existing
	 * backend might exit before the auth cycle is completed.  The exact
	 * MaxBackends limit is enforced when a new backend tries to join the
	 * shared-inval backend array.
	 *
	 * The limit here must match the sizes of the per-child-process arrays;
	 * see comments for MaxLivePostmasterChildren().
	 */
	if (CountChildren(BACKEND_TYPE_ALL) >= MaxLivePostmasterChildren())
		result = CAC_TOOMANY;

	return result;
}