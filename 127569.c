asyncFork() {
	#if defined(__linux__)
		#if defined(SYS_fork)
			return (pid_t) syscall(SYS_fork);
		#else
			return syscall(SYS_clone, SIGCHLD, 0, 0, 0, 0);
		#endif
	#elif defined(__APPLE__)
		return __fork();
	#else
		return fork();
	#endif
}