verify_info_zip_ux(struct archive *a, int seek_checks)
{
	struct archive_entry *ae;
	char *buff[128];

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("file1", archive_entry_pathname(ae));
	assertEqualInt(1300668680, archive_entry_mtime(ae));
	assertEqualInt(18, archive_entry_size(ae));
	assertEqualInt(archive_entry_is_encrypted(ae), 0);
	assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
	if (seek_checks)
		assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
	failure("zip reader should read Info-ZIP New Unix Extra Field");
	assertEqualInt(1001, archive_entry_uid(ae));
	assertEqualInt(1001, archive_entry_gid(ae));
	if (archive_zlib_version() != NULL) {
		failure("archive_read_data() returns number of bytes read");
		assertEqualInt(18, archive_read_data(a, buff, 19));
		assertEqualMem(buff, "hello\nhello\nhello\n", 18);
	} else {
		assertEqualInt(ARCHIVE_FAILED, archive_read_data(a, buff, 19));
		assertEqualString(archive_error_string(a),
		    "Unsupported ZIP compression method (deflation)");
		assert(archive_errno(a) != 0);
	}
	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));

	/* Verify the number of files read. */
	failure("the archive file has just one file");
	assertEqualInt(1, archive_file_count(a));

	assertEqualIntA(a, ARCHIVE_FILTER_NONE, archive_filter_code(a, 0));
	assertEqualIntA(a, ARCHIVE_FORMAT_ZIP, archive_format(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualInt(ARCHIVE_OK, archive_read_free(a));
}