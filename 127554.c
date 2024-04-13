runShellCommand(const StaticString &command) {
	pid_t pid = fork();
	if (pid == 0) {
		resetSignalHandlersAndMask();
		disableMallocDebugging();
		closeAllFileDescriptors(2);
		execlp("/bin/sh", "/bin/sh", "-c", command.data(), (char * const) 0);
		_exit(1);
	} else if (pid == -1) {
		return -1;
	} else {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			return -1;
		} else {
			return status;
		}
	}
}