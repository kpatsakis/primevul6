static bool get_hash(rzip_control *control, int make_hash)
{
	char *passphrase, *testphrase;
	struct termios termios_p;
	int prompt = control->passphrase == NULL;

	passphrase = calloc(PASS_LEN, 1);
	testphrase = calloc(PASS_LEN, 1);
	control->salt_pass = calloc(PASS_LEN, 1);
	control->hash = calloc(HASH_LEN, 1);
	if (unlikely(!passphrase || !testphrase || !control->salt_pass || !control->hash)) {
		fatal("Failed to calloc encrypt buffers in compress_file\n");
		dealloc(testphrase);
		dealloc(passphrase);
		return false;
	}
	mlock(passphrase, PASS_LEN);
	mlock(testphrase, PASS_LEN);
	mlock(control->salt_pass, PASS_LEN);
	mlock(control->hash, HASH_LEN);

	if (control->pass_cb) {
		control->pass_cb(control->pass_data, passphrase, PASS_LEN - SALT_LEN);
		if (!passphrase[0]) {
			fatal("Supplied password was null!");
			munlock(passphrase, PASS_LEN);
			munlock(testphrase, PASS_LEN);
			dealloc(testphrase);
			dealloc(passphrase);
			release_hashes(control);
			return false;
		}
		control->salt_pass_len = strlen(passphrase) + SALT_LEN;
	} else {
		/* Disable stdin echo to screen */
		tcgetattr(fileno(stdin), &termios_p);
		termios_p.c_lflag &= ~ECHO;
		tcsetattr(fileno(stdin), 0, &termios_p);
retry_pass:
		if (prompt)
			print_output("Enter passphrase: ");
		control->salt_pass_len = get_pass(control, passphrase) + SALT_LEN;
		if (prompt)
			print_output("\n");
		if (make_hash) {
			if (prompt)
				print_output("Re-enter passphrase: ");
			get_pass(control, testphrase);
			if (prompt)
				print_output("\n");
			if (strcmp(passphrase, testphrase)) {
				print_output("Passwords do not match. Try again.\n");
				goto retry_pass;
			}
		}
		termios_p.c_lflag |= ECHO;
		tcsetattr(fileno(stdin), 0, &termios_p);
		memset(testphrase, 0, PASS_LEN);
	}
	memcpy(control->salt_pass, control->salt, SALT_LEN);
	memcpy(control->salt_pass + SALT_LEN, passphrase, PASS_LEN - SALT_LEN);
	lrz_stretch(control);
	memset(passphrase, 0, PASS_LEN);
	munlock(passphrase, PASS_LEN);
	munlock(testphrase, PASS_LEN);
	dealloc(testphrase);
	dealloc(passphrase);
	return true;
}