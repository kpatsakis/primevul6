static int exif_process_string_raw(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 */
	if (byte_count) {
		(*result) = safe_emalloc(byte_count, 1, 1);
		memcpy(*result, value, byte_count);
		(*result)[byte_count] = '\0';
		return byte_count+1;
	}
	return 0;
}