executeCommand(pid_t pid, char **argv)
{
    (void) pid;
    if (argv != 0 && argv[0] != 0) {
	char *child_cwd = ProcGetCWD(pid);

	if (fork() == 0) {
	    if (child_cwd) {
		IGNORE_RC(chdir(child_cwd));	/* We don't care if this fails */
	    }
	    execvp(argv[0], argv);
	    exit(EXIT_FAILURE);
	}
	free(child_cwd);
    }
}