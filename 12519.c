cdf_unpack_header(cdf_header_t *h, char *buf)
{
	size_t i;
	size_t len = 0;

	CDF_UNPACK(h->h_magic);
	CDF_UNPACKA(h->h_uuid);
	CDF_UNPACK(h->h_revision);
	CDF_UNPACK(h->h_version);
	CDF_UNPACK(h->h_byte_order);
	CDF_UNPACK(h->h_sec_size_p2);
	CDF_UNPACK(h->h_short_sec_size_p2);
	CDF_UNPACKA(h->h_unused0);
	CDF_UNPACK(h->h_num_sectors_in_sat);
	CDF_UNPACK(h->h_secid_first_directory);
	CDF_UNPACKA(h->h_unused1);
	CDF_UNPACK(h->h_min_size_standard_stream);
	CDF_UNPACK(h->h_secid_first_sector_in_short_sat);
	CDF_UNPACK(h->h_num_sectors_in_short_sat);
	CDF_UNPACK(h->h_secid_first_sector_in_master_sat);
	CDF_UNPACK(h->h_num_sectors_in_master_sat);
	for (i = 0; i < __arraycount(h->h_master_sat); i++)
		CDF_UNPACK(h->h_master_sat[i]);
}