unlink_external_pid_file(int status, Datum arg)
{
	if (external_pid_file)
		unlink(external_pid_file);
}