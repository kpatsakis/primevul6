cdf_read_summary_info(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, const cdf_sat_t *ssat, const cdf_stream_t *sst,
    const cdf_dir_t *dir, cdf_stream_t *scn)
{
	size_t i;
	const cdf_directory_t *d;
	static const char name[] = "\05SummaryInformation";

	for (i = dir->dir_len; i > 0; i--)
		if (dir->dir_tab[i - 1].d_type == CDF_DIR_TYPE_USER_STREAM &&
		    cdf_namecmp(name, dir->dir_tab[i - 1].d_name, sizeof(name))
		    == 0)
			break;

	if (i == 0) {
		DPRINTF(("Cannot find summary information section\n"));
		errno = ESRCH;
		return -1;
	}
	d = &dir->dir_tab[i - 1];
	return cdf_read_sector_chain(info, h, sat, ssat, sst,
	    d->d_stream_first_sector, d->d_size, scn);
}