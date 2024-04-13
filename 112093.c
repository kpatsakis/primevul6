pq_init(void)
{
	int			socket_pos PG_USED_FOR_ASSERTS_ONLY;
	int			latch_pos PG_USED_FOR_ASSERTS_ONLY;

	/* initialize state variables */
	PqSendBufferSize = PQ_SEND_BUFFER_SIZE;
	PqSendBuffer = MemoryContextAlloc(TopMemoryContext, PqSendBufferSize);
	PqSendPointer = PqSendStart = PqRecvPointer = PqRecvLength = 0;
	PqCommBusy = false;
	PqCommReadingMsg = false;

	/* set up process-exit hook to close the socket */
	on_proc_exit(socket_close, 0);

	/*
	 * In backends (as soon as forked) we operate the underlying socket in
	 * nonblocking mode and use latches to implement blocking semantics if
	 * needed. That allows us to provide safely interruptible reads and
	 * writes.
	 *
	 * Use COMMERROR on failure, because ERROR would try to send the error to
	 * the client, which might require changing the mode again, leading to
	 * infinite recursion.
	 */
#ifndef WIN32
	if (!pg_set_noblock(MyProcPort->sock))
		ereport(COMMERROR,
				(errmsg("could not set socket to nonblocking mode: %m")));
#endif

	FeBeWaitSet = CreateWaitEventSet(TopMemoryContext, 3);
	socket_pos = AddWaitEventToSet(FeBeWaitSet, WL_SOCKET_WRITEABLE,
								   MyProcPort->sock, NULL, NULL);
	latch_pos = AddWaitEventToSet(FeBeWaitSet, WL_LATCH_SET, PGINVALID_SOCKET,
								  MyLatch, NULL);
	AddWaitEventToSet(FeBeWaitSet, WL_POSTMASTER_DEATH, PGINVALID_SOCKET,
					  NULL, NULL);

	/*
	 * The event positions match the order we added them, but let's sanity
	 * check them to be sure.
	 */
	Assert(socket_pos == FeBeWaitSetSocketPos);
	Assert(latch_pos == FeBeWaitSetLatchPos);
}