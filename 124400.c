bool flush_tmpoutbuf(rzip_control *control)
{
	if (!TEST_ONLY) {
		print_maxverbose("Dumping buffer to physical file.\n");
		if (STDOUT) {
			if (unlikely(!fwrite_stdout(control, control->tmp_outbuf, control->out_len)))
				return false;
		} else {
			if (unlikely(!write_fdout(control, control->tmp_outbuf, control->out_len)))
				return false;
		}
	}
	control->out_relofs += control->out_len;
	control->out_ofs = control->out_len = 0;
	return true;
}