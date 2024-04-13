compress_close(ds_file_t *file)
{
	ds_compress_file_t	*comp_file;
	ds_file_t		*dest_file;
	int			rc;

	comp_file = (ds_compress_file_t *) file->ptr;
	dest_file = comp_file->dest_file;

	/* Write the qpress file trailer */
	ds_write(dest_file, "ENDSENDS", 8);

	/* Supposedly the number of written bytes should be written as a
	"recovery information" in the file trailer, but in reality qpress
	always writes 8 zeros here. Let's do the same */

	write_uint64_le(dest_file, 0);

	rc = ds_close(dest_file);

	my_free(file);

	return rc;
}