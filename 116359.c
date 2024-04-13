is_afio_large(const char *h, size_t len)
{
	if (len < afiol_header_size)
		return (0);
	if (h[afiol_ino_m_offset] != 'm'
	    || h[afiol_mtime_n_offset] != 'n'
	    || h[afiol_xsize_s_offset] != 's'
	    || h[afiol_filesize_c_offset] != ':')
		return (0);
	if (!is_hex(h + afiol_dev_offset, afiol_ino_m_offset - afiol_dev_offset))
		return (0);
	if (!is_hex(h + afiol_mode_offset, afiol_mtime_n_offset - afiol_mode_offset))
		return (0);
	if (!is_hex(h + afiol_namesize_offset, afiol_xsize_s_offset - afiol_namesize_offset))
		return (0);
	if (!is_hex(h + afiol_filesize_offset, afiol_filesize_size))
		return (0);
	return (1);
}