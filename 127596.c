getFileDescriptorLimit() {
	long long sysconfResult = sysconf(_SC_OPEN_MAX);
	
	struct rlimit rl;
	long long rlimitResult;
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
		rlimitResult = 0;
	} else {
		rlimitResult = (long long) rl.rlim_max;
	}
	
	long result;
	if (sysconfResult > rlimitResult) {
		result = sysconfResult;
	} else {
		result = rlimitResult;
	}
	if (result < 0) {
		// Both calls returned errors.
		result = 9999;
	} else if (result < 2) {
		// The calls reported broken values.
		result = 2;
	}
	return result;
}