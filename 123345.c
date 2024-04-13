compress_open(ds_ctxt_t *ctxt, const char *path, MY_STAT *mystat)
{
	ds_compress_ctxt_t	*comp_ctxt;
	ds_ctxt_t		*dest_ctxt;
 	ds_file_t		*dest_file;
	char			new_name[FN_REFLEN];
	size_t			name_len;
	ds_file_t		*file;
	ds_compress_file_t	*comp_file;

	xb_ad(ctxt->pipe_ctxt != NULL);
	dest_ctxt = ctxt->pipe_ctxt;

	comp_ctxt = (ds_compress_ctxt_t *) ctxt->ptr;

	/* Append the .qp extension to the filename */
	fn_format(new_name, path, "", ".qp", MYF(MY_APPEND_EXT));

	dest_file = ds_open(dest_ctxt, new_name, mystat);
	if (dest_file == NULL) {
		return NULL;
	}

	/* Write the qpress archive header */
	if (ds_write(dest_file, "qpress10", 8) ||
	    write_uint64_le(dest_file, COMPRESS_CHUNK_SIZE)) {
		goto err;
	}

	/* We are going to create a one-file "flat" (i.e. with no
	subdirectories) archive. So strip the directory part from the path and
	remove the '.qp' suffix. */
	fn_format(new_name, path, "", "", MYF(MY_REPLACE_DIR));

	/* Write the qpress file header */
	name_len = strlen(new_name);
	if (ds_write(dest_file, "F", 1) ||
	    write_uint32_le(dest_file, (uint)name_len) ||
	    /* we want to write the terminating \0 as well */
	    ds_write(dest_file, new_name, name_len + 1)) {
		goto err;
	}

	file = (ds_file_t *) my_malloc(sizeof(ds_file_t) +
				       sizeof(ds_compress_file_t),
				       MYF(MY_FAE));
	comp_file = (ds_compress_file_t *) (file + 1);
	comp_file->dest_file = dest_file;
	comp_file->comp_ctxt = comp_ctxt;
	comp_file->bytes_processed = 0;

	file->ptr = comp_file;
	file->path = dest_file->path;

	return file;

err:
	ds_close(dest_file);
	return NULL;
}