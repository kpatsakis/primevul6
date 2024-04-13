DEFINE_TEST(test_read_format_zip_ppmd_multi)
{
	const char *refname = "test_read_format_zip_ppmd8_multi.zipx";
	struct archive *a;
	struct archive_entry *ae;

	extract_reference_file(refname);

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 37));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 6.3 (ppmd-1)", archive_format_name(a));
	assertEqualString("smartd.conf", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one(a, ae, 0x8DD7379E));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 6.3 (ppmd-1)", archive_format_name(a));
	assertEqualString("ts.conf", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one(a, ae, 0x7AE59B31));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 6.3 (ppmd-1)", archive_format_name(a));
	assertEqualString("vimrc", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one(a, ae, 0xBA8E3BAA));

	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}