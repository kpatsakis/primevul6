resetSignalHandlersAndMask() {
	sigset_t signal_set;
	int ret;
	
	sigemptyset(&signal_set);
	do {
		ret = sigprocmask(SIG_SETMASK, &signal_set, NULL);
	} while (ret == -1 && errno == EINTR);
	
	struct sigaction action;
	action.sa_handler = SIG_DFL;
	action.sa_flags   = SA_RESTART;
	sigemptyset(&action.sa_mask);
	sigaction(SIGHUP,  &action, NULL);
	sigaction(SIGINT,  &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGILL,  &action, NULL);
	sigaction(SIGTRAP, &action, NULL);
	sigaction(SIGABRT, &action, NULL);
	#ifdef SIGEMT
		sigaction(SIGEMT,  &action, NULL);
	#endif
	sigaction(SIGFPE,  &action, NULL);
	sigaction(SIGBUS,  &action, NULL);
	sigaction(SIGSEGV, &action, NULL);
	sigaction(SIGSYS,  &action, NULL);
	sigaction(SIGPIPE, &action, NULL);
	sigaction(SIGALRM, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGURG,  &action, NULL);
	sigaction(SIGSTOP, &action, NULL);
	sigaction(SIGTSTP, &action, NULL);
	sigaction(SIGCONT, &action, NULL);
	sigaction(SIGCHLD, &action, NULL);
	#ifdef SIGINFO
		sigaction(SIGINFO, &action, NULL);
	#endif
	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);
}