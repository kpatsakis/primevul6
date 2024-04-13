void clear_tmpinbuf(rzip_control *control)
{
	control->in_len = control->in_ofs = 0;
}