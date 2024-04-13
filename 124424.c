static void usage(bool compat)
{
	print_output("lrz%s version %s\n", compat ? "" : "ip", PACKAGE_VERSION);
	print_output("Copyright (C) Con Kolivas 2006-2021\n");
	print_output("Based on rzip ");
	print_output("Copyright (C) Andrew Tridgell 1998-2003\n\n");
	print_output("Usage: lrz%s [options] <file...>\n", compat ? "" : "ip");
	print_output("General options:\n");
	if (compat) {
		print_output("	-c, --stdout		output to STDOUT\n");
		print_output("	-C, --check		check integrity of file written on decompression\n");
	} else
		print_output("	-c, -C, --check		check integrity of file written on decompression\n");
	print_output("	-d, --decompress	decompress\n");
	print_output("	-e, --encrypt[=password] password protected sha512/aes128 encryption on compression\n");
	print_output("	-h, -?, --help		show help\n");
	print_output("	-H, --hash		display md5 hash integrity information\n");
	print_output("	-i, --info		show compressed file information\n");
	if (compat) {
		print_output("	-L, --license		display software version and license\n");
		print_output("	-P, --progress		show compression progress\n");
	} else
		print_output("	-q, --quiet		don't show compression progress\n");
	print_output("	-r, --recursive		operate recursively on directories\n");
	print_output("	-t, --test		test compressed file integrity\n");
	print_output("	-v[v%s], --verbose	Increase verbosity\n", compat ? "v" : "");
	print_output("	-V, --version		show version\n");
	print_output("Options affecting output:\n");
	if (!compat)
		print_output("	-D, --delete		delete existing files\n");
	print_output("	-f, --force		force overwrite of any existing files\n");
	if (compat)
		print_output("	-k, --keep		don't delete source files on de/compression\n");
	print_output("	-K, --keep-broken	keep broken or damaged output files\n");
	print_output("	-o, --outfile filename	specify the output file name and/or path\n");
	print_output("	-O, --outdir directory	specify the output directory when -o is not used\n");
	print_output("	-S, --suffix suffix	specify compressed suffix (default '.lrz')\n");
	print_output("Options affecting compression:\n");
	print_output("	--lzma			lzma compression (default)\n");
	print_output("	-b, --bzip2		bzip2 compression\n");
	print_output("	-g, --gzip		gzip compression using zlib\n");
	print_output("	-l, --lzo		lzo compression (ultra fast)\n");
	print_output("	-n, --no-compress	no backend compression - prepare for other compressor\n");
	print_output("	-z, --zpaq		zpaq compression (best, extreme compression, extremely slow)\n");
	print_output("Low level options:\n");
	if (compat) {
		print_output("	-1 .. -9		set lzma/bzip2/gzip compression level (1-9, default 7)\n");
		print_output("	--fast			alias for -1\n");
		print_output("	--best			alias for -9\n");
	}
	if (!compat)
		print_output("	-L, --level level	set lzma/bzip2/gzip compression level (1-9, default 7)\n");
	print_output("	-N, --nice-level value	Set nice value to value (default %d)\n", compat ? 0 : 19);
	print_output("	-p, --threads value	Set processor count to override number of threads\n");
	print_output("	-m, --maxram size	Set maximum available ram in hundreds of MB\n");
	print_output("				overrides detected amount of available ram\n");
	print_output("	-T, --threshold		Disable LZ4 compressibility testing\n");
	print_output("	-U, --unlimited		Use unlimited window size beyond ramsize (potentially much slower)\n");
	print_output("	-w, --window size	maximum compression window in hundreds of MB\n");
	print_output("				default chosen by heuristic dependent on ram and chosen compression\n");
	print_output("\nLRZIP=NOCONFIG environment variable setting can be used to bypass lrzip.conf.\n");
	print_output("TMP environment variable will be used for storage of temporary files when needed.\n");
	print_output("TMPDIR may also be stored in lrzip.conf file.\n");
	print_output("\nIf no filenames or \"-\" is specified, stdin/out will be used.\n");

}