write_uint32_le(ds_file_t *file, ulong n)
{
	char tmp[4];

	int4store(tmp, n);
	return ds_write(file, tmp, sizeof(tmp));
}