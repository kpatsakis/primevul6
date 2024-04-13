static void sighandler(int sig __UNUSED__)
{
	signal(sig, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	print_err("Interrupted\n");
	fatal_exit(&local_control);
}