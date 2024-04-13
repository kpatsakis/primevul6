write_uint64_le(ds_file_t *file, ulonglong n)
{
	char tmp[8];

	int8store(tmp, n);
	return ds_write(file, tmp, sizeof(tmp));
}