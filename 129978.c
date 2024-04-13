mono_blob_entry_hash (const char* str)
{
	guint len, h;
	const char *end;
	len = mono_metadata_decode_blob_size (str, &str);
	if (len > 0) {
		end = str + len;
		h = *str;
		for (str += 1; str < end; str++)
			h = (h << 5) - h + *str;
		return h;
	} else {
		return 0;
	}
}