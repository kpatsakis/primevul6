void close_tmpoutbuf(rzip_control *control)
{
	control->flags &= ~FLAG_TMP_OUTBUF;
	dealloc(control->tmp_outbuf);
	if (!BITS32)
		control->usable_ram = control->maxram += control->ramsize / 18;
}