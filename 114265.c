u32 gf_isom_probe_file(const char *fileName)
{
	return gf_isom_probe_file_range(fileName, 0, 0);
}