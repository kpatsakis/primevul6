cdf_read_sector_chain(const cdf_info_t *info, const cdf_header_t *h,
    const cdf_sat_t *sat, const cdf_sat_t *ssat, const cdf_stream_t *sst,
    cdf_secid_t sid, size_t len, cdf_stream_t *scn)
{

	if (len < h->h_min_size_standard_stream && sst->sst_tab != NULL)
		return cdf_read_short_sector_chain(h, ssat, sst, sid, len,
		    scn);
	else
		return cdf_read_long_sector_chain(info, h, sat, sid, len, scn);
}