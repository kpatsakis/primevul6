DEFINE_TEST(test_read_format_zip)
{
	test_basic();
	test_info_zip_ux();
	test_extract_length_at_end();
	test_symlink();
}