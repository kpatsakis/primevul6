removeDirTree(const string &path) {
	this_thread::disable_interruption di;
	this_thread::disable_syscall_interruption dsi;
	const char *c_path = path.c_str();
	pid_t pid;

	pid = syscalls::fork();
	if (pid == 0) {
		resetSignalHandlersAndMask();
		disableMallocDebugging();
		int devnull = open("/dev/null", O_RDONLY);
		if (devnull != -1) {
			dup2(devnull, 2);
		}
		closeAllFileDescriptors(2);
		execlp("chmod", "chmod", "-R", "u+rwx", c_path, (char * const) 0);
		perror("Cannot execute chmod");
		_exit(1);

	} else if (pid == -1) {
		int e = errno;
		throw SystemException("Cannot fork a new process", e);

	} else {
		this_thread::restore_interruption ri(di);
		this_thread::restore_syscall_interruption rsi(dsi);
		syscalls::waitpid(pid, NULL, 0);
	}

	pid = syscalls::fork();
	if (pid == 0) {
		resetSignalHandlersAndMask();
		disableMallocDebugging();
		closeAllFileDescriptors(2);
		execlp("rm", "rm", "-rf", c_path, (char * const) 0);
		perror("Cannot execute rm");
		_exit(1);

	} else if (pid == -1) {
		int e = errno;
		throw SystemException("Cannot fork a new process", e);

	} else {
		this_thread::restore_interruption ri(di);
		this_thread::restore_syscall_interruption rsi(dsi);
		int status;
		if (syscalls::waitpid(pid, &status, 0) == -1 || status != 0) {
			throw RuntimeException("Cannot remove directory '" + path + "'");
		}
	}
}