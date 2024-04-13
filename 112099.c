processCancelRequest(Port *port, void *pkt)
{
	CancelRequestPacket *canc = (CancelRequestPacket *) pkt;
	int			backendPID;
	int32		cancelAuthCode;
	Backend    *bp;

#ifndef EXEC_BACKEND
	dlist_iter	iter;
#else
	int			i;
#endif

	backendPID = (int) pg_ntoh32(canc->backendPID);
	cancelAuthCode = (int32) pg_ntoh32(canc->cancelAuthCode);

	/*
	 * See if we have a matching backend.  In the EXEC_BACKEND case, we can no
	 * longer access the postmaster's own backend list, and must rely on the
	 * duplicate array in shared memory.
	 */
#ifndef EXEC_BACKEND
	dlist_foreach(iter, &BackendList)
	{
		bp = dlist_container(Backend, elem, iter.cur);
#else
	for (i = MaxLivePostmasterChildren() - 1; i >= 0; i--)
	{
		bp = (Backend *) &ShmemBackendArray[i];
#endif
		if (bp->pid == backendPID)
		{
			if (bp->cancel_key == cancelAuthCode)
			{
				/* Found a match; signal that backend to cancel current op */
				ereport(DEBUG2,
						(errmsg_internal("processing cancel request: sending SIGINT to process %d",
										 backendPID)));
				signal_child(bp->pid, SIGINT);
			}
			else
				/* Right PID, wrong key: no way, Jose */
				ereport(LOG,
						(errmsg("wrong key in cancel request for process %d",
								backendPID)));
			return;
		}
#ifndef EXEC_BACKEND			/* make GNU Emacs 26.1 see brace balance */
	}
#else
	}
#endif

	/* No matching backend */
	ereport(LOG,
			(errmsg("PID %d in cancel request did not match any process",
					backendPID)));
}