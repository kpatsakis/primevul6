socket_close(int code, Datum arg)
{
	/* Nothing to do in a standalone backend, where MyProcPort is NULL. */
	if (MyProcPort != NULL)
	{
#ifdef ENABLE_GSS
		/*
		 * Shutdown GSSAPI layer.  This section does nothing when interrupting
		 * BackendInitialize(), because pg_GSS_recvauth() makes first use of
		 * "ctx" and "cred".
		 *
		 * Note that we don't bother to free MyProcPort->gss, since we're
		 * about to exit anyway.
		 */
		if (MyProcPort->gss)
		{
			OM_uint32	min_s;

			if (MyProcPort->gss->ctx != GSS_C_NO_CONTEXT)
				gss_delete_sec_context(&min_s, &MyProcPort->gss->ctx, NULL);

			if (MyProcPort->gss->cred != GSS_C_NO_CREDENTIAL)
				gss_release_cred(&min_s, &MyProcPort->gss->cred);
		}
#endif							/* ENABLE_GSS */

		/*
		 * Cleanly shut down SSL layer.  Nowhere else does a postmaster child
		 * call this, so this is safe when interrupting BackendInitialize().
		 */
		secure_close(MyProcPort);

		/*
		 * Formerly we did an explicit close() here, but it seems better to
		 * leave the socket open until the process dies.  This allows clients
		 * to perform a "synchronous close" if they care --- wait till the
		 * transport layer reports connection closure, and you can be sure the
		 * backend has exited.
		 *
		 * We do set sock to PGINVALID_SOCKET to prevent any further I/O,
		 * though.
		 */
		MyProcPort->sock = PGINVALID_SOCKET;
	}
}