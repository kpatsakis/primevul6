internal_forkexec(int argc, char *argv[], Port *port)
{
	int			retry_count = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int			i;
	int			j;
	char		cmdLine[MAXPGPATH * 2];
	HANDLE		paramHandle;
	BackendParameters *param;
	SECURITY_ATTRIBUTES sa;
	char		paramHandleStr[32];
	win32_deadchild_waitinfo *childinfo;

	/* Make sure caller set up argv properly */
	Assert(argc >= 3);
	Assert(argv[argc] == NULL);
	Assert(strncmp(argv[1], "--fork", 6) == 0);
	Assert(argv[2] == NULL);

	/* Resume here if we need to retry */
retry:

	/* Set up shared memory for parameter passing */
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	paramHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
									&sa,
									PAGE_READWRITE,
									0,
									sizeof(BackendParameters),
									NULL);
	if (paramHandle == INVALID_HANDLE_VALUE)
	{
		ereport(LOG,
				(errmsg("could not create backend parameter file mapping: error code %lu",
						GetLastError())));
		return -1;
	}

	param = MapViewOfFile(paramHandle, FILE_MAP_WRITE, 0, 0, sizeof(BackendParameters));
	if (!param)
	{
		ereport(LOG,
				(errmsg("could not map backend parameter memory: error code %lu",
						GetLastError())));
		CloseHandle(paramHandle);
		return -1;
	}

	/* Insert temp file name after --fork argument */
#ifdef _WIN64
	sprintf(paramHandleStr, "%llu", (LONG_PTR) paramHandle);
#else
	sprintf(paramHandleStr, "%lu", (DWORD) paramHandle);
#endif
	argv[2] = paramHandleStr;

	/* Format the cmd line */
	cmdLine[sizeof(cmdLine) - 1] = '\0';
	cmdLine[sizeof(cmdLine) - 2] = '\0';
	snprintf(cmdLine, sizeof(cmdLine) - 1, "\"%s\"", postgres_exec_path);
	i = 0;
	while (argv[++i] != NULL)
	{
		j = strlen(cmdLine);
		snprintf(cmdLine + j, sizeof(cmdLine) - 1 - j, " \"%s\"", argv[i]);
	}
	if (cmdLine[sizeof(cmdLine) - 2] != '\0')
	{
		ereport(LOG,
				(errmsg("subprocess command line too long")));
		UnmapViewOfFile(param);
		CloseHandle(paramHandle);
		return -1;
	}

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	/*
	 * Create the subprocess in a suspended state. This will be resumed later,
	 * once we have written out the parameter file.
	 */
	if (!CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, CREATE_SUSPENDED,
					   NULL, NULL, &si, &pi))
	{
		ereport(LOG,
				(errmsg("CreateProcess() call failed: %m (error code %lu)",
						GetLastError())));
		UnmapViewOfFile(param);
		CloseHandle(paramHandle);
		return -1;
	}

	if (!save_backend_variables(param, port, pi.hProcess, pi.dwProcessId))
	{
		/*
		 * log made by save_backend_variables, but we have to clean up the
		 * mess with the half-started process
		 */
		if (!TerminateProcess(pi.hProcess, 255))
			ereport(LOG,
					(errmsg_internal("could not terminate unstarted process: error code %lu",
									 GetLastError())));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		UnmapViewOfFile(param);
		CloseHandle(paramHandle);
		return -1;				/* log made by save_backend_variables */
	}

	/* Drop the parameter shared memory that is now inherited to the backend */
	if (!UnmapViewOfFile(param))
		ereport(LOG,
				(errmsg("could not unmap view of backend parameter file: error code %lu",
						GetLastError())));
	if (!CloseHandle(paramHandle))
		ereport(LOG,
				(errmsg("could not close handle to backend parameter file: error code %lu",
						GetLastError())));

	/*
	 * Reserve the memory region used by our main shared memory segment before
	 * we resume the child process.  Normally this should succeed, but if ASLR
	 * is active then it might sometimes fail due to the stack or heap having
	 * gotten mapped into that range.  In that case, just terminate the
	 * process and retry.
	 */
	if (!pgwin32_ReserveSharedMemoryRegion(pi.hProcess))
	{
		/* pgwin32_ReserveSharedMemoryRegion already made a log entry */
		if (!TerminateProcess(pi.hProcess, 255))
			ereport(LOG,
					(errmsg_internal("could not terminate process that failed to reserve memory: error code %lu",
									 GetLastError())));
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		if (++retry_count < 100)
			goto retry;
		ereport(LOG,
				(errmsg("giving up after too many tries to reserve shared memory"),
				 errhint("This might be caused by ASLR or antivirus software.")));
		return -1;
	}

	/*
	 * Now that the backend variables are written out, we start the child
	 * thread so it can start initializing while we set up the rest of the
	 * parent state.
	 */
	if (ResumeThread(pi.hThread) == -1)
	{
		if (!TerminateProcess(pi.hProcess, 255))
		{
			ereport(LOG,
					(errmsg_internal("could not terminate unstartable process: error code %lu",
									 GetLastError())));
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return -1;
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		ereport(LOG,
				(errmsg_internal("could not resume thread of unstarted process: error code %lu",
								 GetLastError())));
		return -1;
	}

	/*
	 * Queue a waiter to signal when this child dies. The wait will be handled
	 * automatically by an operating system thread pool.
	 *
	 * Note: use malloc instead of palloc, since it needs to be thread-safe.
	 * Struct will be free():d from the callback function that runs on a
	 * different thread.
	 */
	childinfo = malloc(sizeof(win32_deadchild_waitinfo));
	if (!childinfo)
		ereport(FATAL,
				(errcode(ERRCODE_OUT_OF_MEMORY),
				 errmsg("out of memory")));

	childinfo->procHandle = pi.hProcess;
	childinfo->procId = pi.dwProcessId;

	if (!RegisterWaitForSingleObject(&childinfo->waitHandle,
									 pi.hProcess,
									 pgwin32_deadchild_callback,
									 childinfo,
									 INFINITE,
									 WT_EXECUTEONLYONCE | WT_EXECUTEINWAITTHREAD))
		ereport(FATAL,
				(errmsg_internal("could not register process for wait: error code %lu",
								 GetLastError())));

	/* Don't close pi.hProcess here - the wait thread needs access to it */

	CloseHandle(pi.hThread);

	return pi.dwProcessId;
}