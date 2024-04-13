static int de_thread(struct task_struct *tsk)
{
	struct signal_struct *sig = tsk->signal;
	struct sighand_struct *newsighand, *oldsighand = tsk->sighand;
	spinlock_t *lock = &oldsighand->siglock;
	struct task_struct *leader = NULL;
	int count;

	/*
	 * Tell all the sighand listeners that this sighand has
	 * been detached. The signalfd_detach() function grabs the
	 * sighand lock, if signal listeners are present on the sighand.
	 */
	signalfd_detach(tsk);

	/*
	 * If we don't share sighandlers, then we aren't sharing anything
	 * and we can just re-use it all.
	 */
	if (atomic_read(&oldsighand->count) <= 1) {
		BUG_ON(atomic_read(&sig->count) != 1);
		exit_itimers(sig);
		return 0;
	}

	newsighand = kmem_cache_alloc(sighand_cachep, GFP_KERNEL);
	if (!newsighand)
		return -ENOMEM;

	if (thread_group_empty(tsk))
		goto no_thread_group;

	/*
	 * Kill all other threads in the thread group.
	 * We must hold tasklist_lock to call zap_other_threads.
	 */
	read_lock(&tasklist_lock);
	spin_lock_irq(lock);
	if (sig->flags & SIGNAL_GROUP_EXIT) {
		/*
		 * Another group action in progress, just
		 * return so that the signal is processed.
		 */
		spin_unlock_irq(lock);
		read_unlock(&tasklist_lock);
		kmem_cache_free(sighand_cachep, newsighand);
		return -EAGAIN;
	}

	/*
	 * child_reaper ignores SIGKILL, change it now.
	 * Reparenting needs write_lock on tasklist_lock,
	 * so it is safe to do it under read_lock.
	 */
	if (unlikely(tsk->group_leader == child_reaper(tsk)))
		tsk->nsproxy->pid_ns->child_reaper = tsk;

	zap_other_threads(tsk);
	read_unlock(&tasklist_lock);

	/*
	 * Account for the thread group leader hanging around:
	 */
	count = 1;
	if (!thread_group_leader(tsk)) {
		count = 2;
		/*
		 * The SIGALRM timer survives the exec, but needs to point
		 * at us as the new group leader now.  We have a race with
		 * a timer firing now getting the old leader, so we need to
		 * synchronize with any firing (by calling del_timer_sync)
		 * before we can safely let the old group leader die.
		 */
		sig->tsk = tsk;
		spin_unlock_irq(lock);
		if (hrtimer_cancel(&sig->real_timer))
			hrtimer_restart(&sig->real_timer);
		spin_lock_irq(lock);
	}
	while (atomic_read(&sig->count) > count) {
		sig->group_exit_task = tsk;
		sig->notify_count = count;
		__set_current_state(TASK_UNINTERRUPTIBLE);
		spin_unlock_irq(lock);
		schedule();
		spin_lock_irq(lock);
	}
	sig->group_exit_task = NULL;
	sig->notify_count = 0;
	spin_unlock_irq(lock);

	/*
	 * At this point all other threads have exited, all we have to
	 * do is to wait for the thread group leader to become inactive,
	 * and to assume its PID:
	 */
	if (!thread_group_leader(tsk)) {
		/*
		 * Wait for the thread group leader to be a zombie.
		 * It should already be zombie at this point, most
		 * of the time.
		 */
		leader = tsk->group_leader;
		while (leader->exit_state != EXIT_ZOMBIE)
			yield();

		/*
		 * The only record we have of the real-time age of a
		 * process, regardless of execs it's done, is start_time.
		 * All the past CPU time is accumulated in signal_struct
		 * from sister threads now dead.  But in this non-leader
		 * exec, nothing survives from the original leader thread,
		 * whose birth marks the true age of this process now.
		 * When we take on its identity by switching to its PID, we
		 * also take its birthdate (always earlier than our own).
		 */
		tsk->start_time = leader->start_time;

		write_lock_irq(&tasklist_lock);

		BUG_ON(leader->tgid != tsk->tgid);
		BUG_ON(tsk->pid == tsk->tgid);
		/*
		 * An exec() starts a new thread group with the
		 * TGID of the previous thread group. Rehash the
		 * two threads with a switched PID, and release
		 * the former thread group leader:
		 */

		/* Become a process group leader with the old leader's pid.
		 * The old leader becomes a thread of the this thread group.
		 * Note: The old leader also uses this pid until release_task
		 *       is called.  Odd but simple and correct.
		 */
		detach_pid(tsk, PIDTYPE_PID);
		tsk->pid = leader->pid;
		attach_pid(tsk, PIDTYPE_PID,  find_pid(tsk->pid));
		transfer_pid(leader, tsk, PIDTYPE_PGID);
		transfer_pid(leader, tsk, PIDTYPE_SID);
		list_replace_rcu(&leader->tasks, &tsk->tasks);

		tsk->group_leader = tsk;
		leader->group_leader = tsk;

		tsk->exit_signal = SIGCHLD;

		BUG_ON(leader->exit_state != EXIT_ZOMBIE);
		leader->exit_state = EXIT_DEAD;

		write_unlock_irq(&tasklist_lock);
        }

	/*
	 * There may be one thread left which is just exiting,
	 * but it's safe to stop telling the group to kill themselves.
	 */
	sig->flags = 0;

no_thread_group:
	exit_itimers(sig);
	if (leader)
		release_task(leader);

	BUG_ON(atomic_read(&sig->count) != 1);

	if (atomic_read(&oldsighand->count) == 1) {
		/*
		 * Now that we nuked the rest of the thread group,
		 * it turns out we are not sharing sighand any more either.
		 * So we can just keep it.
		 */
		kmem_cache_free(sighand_cachep, newsighand);
	} else {
		/*
		 * Move our state over to newsighand and switch it in.
		 */
		atomic_set(&newsighand->count, 1);
		memcpy(newsighand->action, oldsighand->action,
		       sizeof(newsighand->action));

		write_lock_irq(&tasklist_lock);
		spin_lock(&oldsighand->siglock);
		spin_lock_nested(&newsighand->siglock, SINGLE_DEPTH_NESTING);

		rcu_assign_pointer(tsk->sighand, newsighand);
		recalc_sigpending();

		spin_unlock(&newsighand->siglock);
		spin_unlock(&oldsighand->siglock);
		write_unlock_irq(&tasklist_lock);

		__cleanup_sighand(oldsighand);
	}

	BUG_ON(!thread_group_leader(tsk));
	return 0;
}