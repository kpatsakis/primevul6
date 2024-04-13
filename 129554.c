archive_read_format_zip_has_encrypted_entries(struct archive_read *_a)
{
	if (_a && _a->format) {
		struct zip * zip = (struct zip *)_a->format->data;
		if (zip) {
			return zip->has_encrypted_entries;
		}
	}
	return ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW;
}