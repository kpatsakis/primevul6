test_extract_length_at_end(void)
{
	const char *refname = "test_read_format_zip_length_at_end.zip";
	char *p;
	size_t s;
	struct archive *a;

	extract_reference_file(refname);

	/* Verify extraction with seeking reader. */
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 10240));
	verify_extract_length_at_end(a, 1);

	/* Verify extraction with streaming reader. */
	p = slurpfile(&s, refname);
	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_filter_all(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_all(a));
	assertEqualIntA(a, ARCHIVE_OK, read_open_memory(a, p, s, 108));
	verify_extract_length_at_end(a, 0);
	free(p);
}