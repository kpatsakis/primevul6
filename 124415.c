void close_tmpinbuf(rzip_control *control)
{
	control->flags &= ~FLAG_TMP_INBUF;
	dealloc(control->tmp_inbuf);
	if (!BITS32)
		control->usable_ram = control->maxram += control->ramsize / 18;
}