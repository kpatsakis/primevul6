DEFINE_TEST(test_read_format_zip_xz_multi_blockread)
{
	const char *refname = "test_read_format_zip_xz_multi.zipx";
	struct archive *a;
	struct archive_entry *ae;

	extract_reference_file(refname);

	assert((a = archive_read_new()) != NULL);
	assertEqualIntA(a, ARCHIVE_OK, archive_read_support_format_zip(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_open_filename(a, refname, 37));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 2.0 (xz)", archive_format_name(a));
	assertEqualString("bash.bashrc", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one_using_blocks(a, 12, 0xF751B8C9));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 2.0 (xz)", archive_format_name(a));
	assertEqualString("pacman.conf", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one_using_blocks(a, 13, 0xB20B7F88));

	assertEqualIntA(a, ARCHIVE_OK, archive_read_next_header(a, &ae));
	assertEqualString("ZIP 2.0 (xz)", archive_format_name(a));
	assertEqualString("profile", archive_entry_pathname(ae));
	assertEqualIntA(a, 0, extract_one_using_blocks(a, 14, 0x2329F054));

	assertEqualIntA(a, ARCHIVE_EOF, archive_read_next_header(a, &ae));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_close(a));
	assertEqualIntA(a, ARCHIVE_OK, archive_read_free(a));
}