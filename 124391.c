static void show_summary(void)
{
	/* OK, if verbosity set, print summary of options selected */
	if (!INFO) {
		if (!TEST_ONLY)
			print_verbose("The following options are in effect for this %s.\n",
				      DECOMPRESS ? "DECOMPRESSION" : "COMPRESSION");
		print_verbose("Threading is %s. Number of CPUs detected: %d\n", control->threads > 1? "ENABLED" : "DISABLED",
			      control->threads);
		print_verbose("Detected %lld bytes ram\n", control->ramsize);
		print_verbose("Compression level %d\n", control->compression_level);
		print_verbose("Nice Value: %d\n", control->nice_val);
		print_verbose("Show Progress\n");
		print_maxverbose("Max ");
		print_verbose("Verbose\n");
		if (FORCE_REPLACE)
			print_verbose("Overwrite Files\n");
		if (!KEEP_FILES)
			print_verbose("Remove input files on completion\n");
		if (control->outdir)
			print_verbose("Output Directory Specified: %s\n", control->outdir);
		else if (control->outname)
			print_verbose("Output Filename Specified: %s\n", control->outname);
		if (TEST_ONLY)
			print_verbose("Test file integrity\n");
		if (control->tmpdir)
			print_verbose("Temporary Directory set as: %s\n", control->tmpdir);

		/* show compression options */
		if (!DECOMPRESS && !TEST_ONLY) {
			print_verbose("Compression mode is: ");
			if (LZMA_COMPRESS)
				print_verbose("LZMA. LZ4 Compressibility testing %s\n", (LZ4_TEST? "enabled" : "disabled"));
			else if (LZO_COMPRESS)
				print_verbose("LZO\n");
			else if (BZIP2_COMPRESS)
				print_verbose("BZIP2. LZ4 Compressibility testing %s\n", (LZ4_TEST? "enabled" : "disabled"));
			else if (ZLIB_COMPRESS)
				print_verbose("GZIP\n");
			else if (ZPAQ_COMPRESS)
				print_verbose("ZPAQ. LZ4 Compressibility testing %s\n", (LZ4_TEST? "enabled" : "disabled"));
			else if (NO_COMPRESS)
				print_verbose("RZIP pre-processing only\n");
			if (control->window)
				print_verbose("Compression Window: %lld = %lldMB\n", control->window, control->window * 100ull);
			/* show heuristically computed window size */
			if (!control->window && !UNLIMITED) {
				i64 temp_chunk, temp_window;

				if (STDOUT || STDIN)
					temp_chunk = control->maxram;
				else
					temp_chunk = control->ramsize * 2 / 3;
				temp_window = temp_chunk / (100 * 1024 * 1024);
				print_verbose("Heuristically Computed Compression Window: %lld = %lldMB\n", temp_window, temp_window * 100ull);
			}
			if (UNLIMITED)
				print_verbose("Using Unlimited Window size\n");
		}
		if (!DECOMPRESS && !TEST_ONLY)
			print_maxverbose("Storage time in seconds %lld\n", control->secs);
		if (ENCRYPT)
			print_maxverbose("Encryption hash loops %lld\n", control->encloops);
	}
}