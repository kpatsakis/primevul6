verify_extract_length_at_end(struct archive *a, int seek_checks)
{
	struct archive_entry *ae;

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));

	assertEqualInt(archive_entry_is_encrypted(ae), 0);
	assertEqualIntA(a, archive_read_has_encrypted_entries(a), 0);
	assertEqualString("hello.txt", archive_entry_pathname(ae));
	if (seek_checks) {
		assertEqualInt(AE_IFREG | 0644, archive_entry_mode(ae));
		assert(archive_entry_size_is_set(ae));
		assertEqualInt(6, archive_entry_size(ae));
	} else {
		assert(!archive_entry_size_is_set(ae));
		assertEqualInt(0, archive_entry_size(ae));
	}

	if (archive_zlib_version() != NULL) {
		assertEqualIntA(a, ARCHIVE_OK, archive_read_extract(a, ae, 0));
		assertFileContents("hello\x0A", 6, "hello.txt");
	} else {
		assertEqualIntA(a, ARCHIVE_FAILED, archive_read_extract(a, ae, 0));
		assertEqualString(archive_error_string(a),
		    "Unsupported ZIP compression method (deflation)");
		assert(archive_errno(a) != 0);
	}

	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}