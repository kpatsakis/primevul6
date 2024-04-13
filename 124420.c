static void release_hashes(rzip_control *control)
{
	memset(control->salt_pass, 0, PASS_LEN);
	memset(control->hash, 0, SALT_LEN);
	munlock(control->salt_pass, PASS_LEN);
	munlock(control->hash, HASH_LEN);
	dealloc(control->salt_pass);
	dealloc(control->hash);
}