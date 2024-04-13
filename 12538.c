cdf_unpack_dir(cdf_directory_t *d, char *buf)
{
	size_t len = 0;

	CDF_UNPACKA(d->d_name);
	CDF_UNPACK(d->d_namelen);
	CDF_UNPACK(d->d_type);
	CDF_UNPACK(d->d_color);
	CDF_UNPACK(d->d_left_child);
	CDF_UNPACK(d->d_right_child);
	CDF_UNPACK(d->d_storage);
	CDF_UNPACKA(d->d_storage_uuid);
	CDF_UNPACK(d->d_flags);
	CDF_UNPACK(d->d_created);
	CDF_UNPACK(d->d_modified);
	CDF_UNPACK(d->d_stream_first_sector);
	CDF_UNPACK(d->d_size);
	CDF_UNPACK(d->d_unused0);
}