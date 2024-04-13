static void set_stdout(struct rzip_control *control)
{
	control->flags |= FLAG_STDOUT;
	control->outFILE = stdout;
	control->msgout = stderr;
	register_outputfile(control, control->msgout);
}