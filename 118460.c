Bool gf_m2ts_probe_data(const u8 *data, u32 size)
{
	size /= 188;
	size *= 188;
	return gf_m2ts_probe_buffer((char *) data, size);
}