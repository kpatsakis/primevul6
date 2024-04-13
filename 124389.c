static bool get_magic(rzip_control *control, char *magic)
{
	int encrypted, md5, i;
	i64 expected_size;
	uint32_t v;

	if (unlikely(strncmp(magic, "LRZI", 4)))
		failure_return(("Not an lrzip file\n"), false);

	memcpy(&control->major_version, &magic[4], 1);
	memcpy(&control->minor_version, &magic[5], 1);

	print_verbose("Detected lrzip version %d.%d file.\n", control->major_version, control->minor_version);
	if (control->major_version > LRZIP_MAJOR_VERSION ||
	    (control->major_version == LRZIP_MAJOR_VERSION && control->minor_version > LRZIP_MINOR_VERSION))
		print_output("Attempting to work with file produced by newer lrzip version %d.%d file.\n", control->major_version, control->minor_version);

	/* Support the convoluted way we described size in versions < 0.40 */
	if (control->major_version == 0 && control->minor_version < 4) {
		memcpy(&v, &magic[6], 4);
		expected_size = ntohl(v);
		memcpy(&v, &magic[10], 4);
		expected_size |= ((i64)ntohl(v)) << 32;
	} else {
		memcpy(&expected_size, &magic[6], 8);
		expected_size = le64toh(expected_size);
	}
	control->st_size = expected_size;
	if (control->major_version == 0 && control->minor_version < 6)
		control->eof = 1;

	/* restore LZMA compression flags only if stored */
	if ((int) magic[16]) {
		for (i = 0; i < 5; i++)
			control->lzma_properties[i] = magic[i + 16];
		/* Cludge to allow us to read possibly corrupted archives */
		if (!control->lzma_properties[0])
			control->lzma_properties[0] = 93;
	}

	/* Whether this archive contains md5 data at the end or not */
	md5 = magic[21];
	if (md5 && MD5_RELIABLE) {
		if (md5 == 1)
			control->flags |= FLAG_MD5;
		else
			print_verbose("Unknown hash, falling back to CRC\n");
	}
	encrypted = magic[22];
	if (encrypted) {
		if (encrypted == 1)
			control->flags |= FLAG_ENCRYPT;
		else
			failure_return(("Unknown encryption\n"), false);
		/* In encrypted files, the size field is used to store the salt
		 * instead and the size is unknown, just like a STDOUT chunked
		 * file */
		memcpy(&control->salt, &magic[6], 8);
		control->st_size = expected_size = 0;
		control->encloops = enc_loops(control->salt[0], control->salt[1]);
		print_maxverbose("Encryption hash loops %lld\n", control->encloops);
	} else if (ENCRYPT) {
		print_output("Asked to decrypt a non-encrypted archive. Bypassing decryption.\n");
		control->flags &= ~FLAG_ENCRYPT;
	}
	return true;
}