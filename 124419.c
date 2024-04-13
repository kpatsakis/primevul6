static bool open_tmpinbuf(rzip_control *control)
{
	control->flags |= FLAG_TMP_INBUF;
	control->in_maxlen = control->maxram;
	control->tmp_inbuf = malloc(control->maxram + control->page_size);
	if (unlikely(!control->tmp_inbuf))
		fatal_return(("Failed to malloc tmp_inbuf in open_tmpinbuf\n"), false);
	return true;
}