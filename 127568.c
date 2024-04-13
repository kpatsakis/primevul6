prestartWebApps(const ResourceLocator &locator, const string &ruby,
	const vector<string> &prestartURLs)
{
	/* Apache calls the initialization routines twice during startup, and
	 * as a result it starts two helper servers, where the first one exits
	 * after a short idle period. We want any prespawning requests to reach
	 * the second helper server, so we sleep for a short period before
	 * executing the prespawning scripts.
	 */
	syscalls::sleep(2);
	
	this_thread::disable_interruption di;
	this_thread::disable_syscall_interruption dsi;
	vector<string>::const_iterator it;
	string prespawnScript = locator.getHelperScriptsDir() + "/prespawn";
	
	it = prestartURLs.begin();
	while (it != prestartURLs.end() && !this_thread::interruption_requested()) {
		if (it->empty()) {
			it++;
			continue;
		}
		
		pid_t pid;
		
		pid = fork();
		if (pid == 0) {
			long max_fds, i;
			int e;
			
			// Close all unnecessary file descriptors.
			max_fds = sysconf(_SC_OPEN_MAX);
			for (i = 3; i < max_fds; i++) {
				syscalls::close(i);
			}
			
			execlp(ruby.c_str(),
				ruby.c_str(),
				prespawnScript.c_str(),
				it->c_str(),
				(char *) 0);
			e = errno;
			fprintf(stderr, "Cannot execute '%s %s': %s (%d)\n",
				prespawnScript.c_str(), it->c_str(),
				strerror(e), e);
			fflush(stderr);
			_exit(1);
		} else if (pid == -1) {
			perror("fork()");
		} else {
			try {
				this_thread::restore_interruption si(di);
				this_thread::restore_syscall_interruption ssi(dsi);
				syscalls::waitpid(pid, NULL, 0);
			} catch (const thread_interrupted &) {
				syscalls::kill(SIGKILL, pid);
				syscalls::waitpid(pid, NULL, 0);
				throw;
			}
		}
		
		this_thread::restore_interruption si(di);
		this_thread::restore_syscall_interruption ssi(dsi);
		syscalls::sleep(1);
		it++;
	}
}