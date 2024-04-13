maybe_start_bgworkers(void)
{
#define MAX_BGWORKERS_TO_LAUNCH 100
	int			num_launched = 0;
	TimestampTz now = 0;
	slist_mutable_iter iter;

	/*
	 * During crash recovery, we have no need to be called until the state
	 * transition out of recovery.
	 */
	if (FatalError)
	{
		StartWorkerNeeded = false;
		HaveCrashedWorker = false;
		return;
	}

	/* Don't need to be called again unless we find a reason for it below */
	StartWorkerNeeded = false;
	HaveCrashedWorker = false;

	slist_foreach_modify(iter, &BackgroundWorkerList)
	{
		RegisteredBgWorker *rw;

		rw = slist_container(RegisteredBgWorker, rw_lnode, iter.cur);

		/* ignore if already running */
		if (rw->rw_pid != 0)
			continue;

		/* if marked for death, clean up and remove from list */
		if (rw->rw_terminate)
		{
			ForgetBackgroundWorker(&iter);
			continue;
		}

		/*
		 * If this worker has crashed previously, maybe it needs to be
		 * restarted (unless on registration it specified it doesn't want to
		 * be restarted at all).  Check how long ago did a crash last happen.
		 * If the last crash is too recent, don't start it right away; let it
		 * be restarted once enough time has passed.
		 */
		if (rw->rw_crashed_at != 0)
		{
			if (rw->rw_worker.bgw_restart_time == BGW_NEVER_RESTART)
			{
				int			notify_pid;

				notify_pid = rw->rw_worker.bgw_notify_pid;

				ForgetBackgroundWorker(&iter);

				/* Report worker is gone now. */
				if (notify_pid != 0)
					kill(notify_pid, SIGUSR1);

				continue;
			}

			/* read system time only when needed */
			if (now == 0)
				now = GetCurrentTimestamp();

			if (!TimestampDifferenceExceeds(rw->rw_crashed_at, now,
											rw->rw_worker.bgw_restart_time * 1000))
			{
				/* Set flag to remember that we have workers to start later */
				HaveCrashedWorker = true;
				continue;
			}
		}

		if (bgworker_should_start_now(rw->rw_worker.bgw_start_time))
		{
			/* reset crash time before trying to start worker */
			rw->rw_crashed_at = 0;

			/*
			 * Try to start the worker.
			 *
			 * On failure, give up processing workers for now, but set
			 * StartWorkerNeeded so we'll come back here on the next iteration
			 * of ServerLoop to try again.  (We don't want to wait, because
			 * there might be additional ready-to-run workers.)  We could set
			 * HaveCrashedWorker as well, since this worker is now marked
			 * crashed, but there's no need because the next run of this
			 * function will do that.
			 */
			if (!do_start_bgworker(rw))
			{
				StartWorkerNeeded = true;
				return;
			}

			/*
			 * If we've launched as many workers as allowed, quit, but have
			 * ServerLoop call us again to look for additional ready-to-run
			 * workers.  There might not be any, but we'll find out the next
			 * time we run.
			 */
			if (++num_launched >= MAX_BGWORKERS_TO_LAUNCH)
			{
				StartWorkerNeeded = true;
				return;
			}
		}
	}
}