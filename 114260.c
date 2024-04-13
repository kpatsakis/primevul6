u64 gf_isom_get_file_size(GF_ISOFile *the_file)
{
	if (!the_file) return 0;
	if (the_file->movieFileMap) return gf_bs_get_size(the_file->movieFileMap->bs);
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (the_file->editFileMap) return gf_bs_get_size(the_file->editFileMap->bs);
#endif
	return 0;
}