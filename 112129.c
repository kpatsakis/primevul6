save_backend_variables(BackendParameters *param, Port *port,
					   HANDLE childProcess, pid_t childPid)
#endif
{
	memcpy(&param->port, port, sizeof(Port));
	if (!write_inheritable_socket(&param->portsocket, port->sock, childPid))
		return false;

	strlcpy(param->DataDir, DataDir, MAXPGPATH);

	memcpy(&param->ListenSocket, &ListenSocket, sizeof(ListenSocket));

	param->MyCancelKey = MyCancelKey;
	param->MyPMChildSlot = MyPMChildSlot;

#ifdef WIN32
	param->ShmemProtectiveRegion = ShmemProtectiveRegion;
#endif
	param->UsedShmemSegID = UsedShmemSegID;
	param->UsedShmemSegAddr = UsedShmemSegAddr;

	param->ShmemLock = ShmemLock;
	param->ShmemVariableCache = ShmemVariableCache;
	param->ShmemBackendArray = ShmemBackendArray;

#ifndef HAVE_SPINLOCKS
	param->SpinlockSemaArray = SpinlockSemaArray;
#endif
	param->NamedLWLockTrancheRequests = NamedLWLockTrancheRequests;
	param->NamedLWLockTrancheArray = NamedLWLockTrancheArray;
	param->MainLWLockArray = MainLWLockArray;
	param->ProcStructLock = ProcStructLock;
	param->ProcGlobal = ProcGlobal;
	param->AuxiliaryProcs = AuxiliaryProcs;
	param->PreparedXactProcs = PreparedXactProcs;
	param->PMSignalState = PMSignalState;
	if (!write_inheritable_socket(&param->pgStatSock, pgStatSock, childPid))
		return false;

	param->PostmasterPid = PostmasterPid;
	param->PgStartTime = PgStartTime;
	param->PgReloadTime = PgReloadTime;
	param->first_syslogger_file_time = first_syslogger_file_time;

	param->redirection_done = redirection_done;
	param->IsBinaryUpgrade = IsBinaryUpgrade;
	param->query_id_enabled = query_id_enabled;
	param->max_safe_fds = max_safe_fds;

	param->MaxBackends = MaxBackends;

#ifdef WIN32
	param->PostmasterHandle = PostmasterHandle;
	if (!write_duplicated_handle(&param->initial_signal_pipe,
								 pgwin32_create_signal_listener(childPid),
								 childProcess))
		return false;
#else
	memcpy(&param->postmaster_alive_fds, &postmaster_alive_fds,
		   sizeof(postmaster_alive_fds));
#endif

	memcpy(&param->syslogPipe, &syslogPipe, sizeof(syslogPipe));

	strlcpy(param->my_exec_path, my_exec_path, MAXPGPATH);

	strlcpy(param->pkglib_path, pkglib_path, MAXPGPATH);

	return true;
}