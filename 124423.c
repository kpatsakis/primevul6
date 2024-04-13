bool get_fileinfo(rzip_control *control)
{
	i64 u_len, c_len, second_last, last_head, utotal = 0, ctotal = 0, ofs = 25, stream_head[2];
	i64 expected_size, infile_size, chunk_size = 0, chunk_total = 0;
	int header_length, stream = 0, chunk = 0;
	char *tmp, *infilecopy = NULL;
	char chunk_byte = 0;
	long double cratio;
	uchar ctype = 0;
	uchar save_ctype = 255;
	struct stat st;
	int fd_in;

	if (!STDIN) {
		struct stat fdin_stat;

		stat(control->infile, &fdin_stat);
		if (!S_ISREG(fdin_stat.st_mode) && (tmp = strrchr(control->infile, '.')) &&
		    strcmp(tmp,control->suffix)) {
			infilecopy = alloca(strlen(control->infile) + strlen(control->suffix) + 1);
			strcpy(infilecopy, control->infile);
			strcat(infilecopy, control->suffix);
		} else
			infilecopy = strdupa(control->infile);
	}

	if ( IS_FROM_FILE )
		fd_in = fileno(control->inFILE);
	else if (STDIN)
		fd_in = 0;
	else {
		fd_in = open(infilecopy, O_RDONLY);
		if (unlikely(fd_in == -1))
			fatal_return(("Failed to open %s\n", infilecopy), false);
	}

	/* Get file size */
	if (unlikely(fstat(fd_in, &st)))
		fatal_goto(("bad magic file descriptor!?\n"), error);
	infile_size = st.st_size;

	/* Get decompressed size */
	if (unlikely(!read_magic(control, fd_in, &expected_size)))
		goto error;

	if (ENCRYPT) {
		print_output("Encrypted lrzip archive. No further information available\n");
		if (!STDIN && !IS_FROM_FILE)
			close(fd_in);
		goto out;
	}

	if (control->major_version == 0 && control->minor_version > 4) {
		if (unlikely(read(fd_in, &chunk_byte, 1) != 1))
			fatal_goto(("Failed to read chunk_byte in get_fileinfo\n"), error);
		if (unlikely(chunk_byte < 1 || chunk_byte > 8))
			fatal_goto(("Invalid chunk bytes %d\n", chunk_byte), error);
		if (control->major_version == 0 && control->minor_version > 5) {
			if (unlikely(read(fd_in, &control->eof, 1) != 1))
				fatal_goto(("Failed to read eof in get_fileinfo\n"), error);
			if (unlikely(read(fd_in, &chunk_size, chunk_byte) != chunk_byte))
				fatal_goto(("Failed to read chunk_size in get_fileinfo\n"), error);
			chunk_size = le64toh(chunk_size);
			if (unlikely(chunk_size < 0))
				fatal_goto(("Invalid chunk size %lld\n", chunk_size), error);
		}
	}

	if (control->major_version == 0 && control->minor_version < 4) {
		ofs = 24;
		header_length = 13;
	} else if (control->major_version == 0 && control->minor_version == 4) {
		ofs = 24;
		header_length = 25;
	} else if (control->major_version == 0 && control->minor_version == 5) {
		ofs = 25;
		header_length = 25;
	} else {
		ofs = 26 + chunk_byte;
		header_length = 1 + (chunk_byte * 3);
	}
	if (control->major_version == 0 && control->minor_version < 6 &&
		!expected_size)
			goto done;
next_chunk:
	stream = 0;
	stream_head[0] = 0;
	stream_head[1] = stream_head[0] + header_length;

	print_verbose("Rzip chunk %d:\n", ++chunk);
	if (chunk_byte)
		print_verbose("Chunk byte width: %d\n", chunk_byte);
	if (chunk_size) {
		chunk_total += chunk_size;
		print_verbose("Chunk size: %lld\n", chunk_size);
	}
	if (unlikely(chunk_byte && (chunk_byte > 8 || chunk_size < 0)))
		failure("Invalid chunk data\n");
	while (stream < NUM_STREAMS) {
		int block = 1;

		second_last = 0;
		if (unlikely(lseek(fd_in, stream_head[stream] + ofs, SEEK_SET) == -1))
			fatal_goto(("Failed to seek to header data in get_fileinfo\n"), error);
		if (unlikely(!get_header_info(control, fd_in, &ctype, &c_len, &u_len, &last_head, chunk_byte)))
			return false;

		print_verbose("Stream: %d\n", stream);
		print_maxverbose("Offset: %lld\n", stream_head[stream] + ofs);
		print_verbose("Block\tComp\tPercent\tSize\n");
		do {
			i64 head_off;

			if (unlikely(last_head && last_head < second_last))
				failure_goto(("Invalid earlier last_head position, corrupt archive.\n"), error);
			second_last = last_head;
			if (unlikely(last_head + ofs > infile_size))
				failure_goto(("Offset greater than archive size, likely corrupted/truncated archive.\n"), error);
			if (unlikely((head_off = lseek(fd_in, last_head + ofs, SEEK_SET)) == -1))
				fatal_goto(("Failed to seek to header data in get_fileinfo\n"), error);
			if (unlikely(!get_header_info(control, fd_in, &ctype, &c_len, &u_len,
					&last_head, chunk_byte)))
				return false;
			if (unlikely(last_head < 0 || c_len < 0 || u_len < 0))
				failure_goto(("Entry negative, likely corrupted archive.\n"), error);
			print_verbose("%d\t", block);
			if (ctype == CTYPE_NONE)
				print_verbose("none");
			else if (ctype == CTYPE_BZIP2)
				print_verbose("bzip2");
			else if (ctype == CTYPE_LZO)
				print_verbose("lzo");
			else if (ctype == CTYPE_LZMA)
				print_verbose("lzma");
			else if (ctype == CTYPE_GZIP)
				print_verbose("gzip");
			else if (ctype == CTYPE_ZPAQ)
				print_verbose("zpaq");
			else
				print_verbose("Dunno wtf");
			if (save_ctype == 255)
				save_ctype = ctype; /* need this for lzma when some chunks could have no compression
						     * and info will show rzip + none on info display if last chunk
						     * is not compressed. Adjust for all types in case it's used in
						     * the future */
			utotal += u_len;
			ctotal += c_len;
			print_verbose("\t%.1f%%\t%lld / %lld", percentage(c_len, u_len), c_len, u_len);
			print_maxverbose("\tOffset: %lld\tHead: %lld", head_off, last_head);
			print_verbose("\n");
			block++;
		} while (last_head);
		++stream;
	}

	if (unlikely((ofs = lseek(fd_in, c_len, SEEK_CUR)) == -1))
		fatal_goto(("Failed to lseek c_len in get_fileinfo\n"), error);

	if (ofs >= infile_size - (HAS_MD5 ? MD5_DIGEST_SIZE : 0))
		goto done;
	/* Chunk byte entry */
	if (control->major_version == 0 && control->minor_version > 4) {
		if (unlikely(read(fd_in, &chunk_byte, 1) != 1))
			fatal_goto(("Failed to read chunk_byte in get_fileinfo\n"), error);
		if (unlikely(chunk_byte < 1 || chunk_byte > 8))
			fatal_goto(("Invalid chunk bytes %d\n", chunk_byte), error);
		ofs++;
		if (control->major_version == 0 && control->minor_version > 5) {
			if (unlikely(read(fd_in, &control->eof, 1) != 1))
				fatal_goto(("Failed to read eof in get_fileinfo\n"), error);
			if (unlikely(read(fd_in, &chunk_size, chunk_byte) != chunk_byte))
				fatal_goto(("Failed to read chunk_size in get_fileinfo\n"), error);
			chunk_size = le64toh(chunk_size);
			if (unlikely(chunk_size < 0))
				fatal_goto(("Invalid chunk size %lld\n", chunk_size), error);
			ofs += 1 + chunk_byte;
			header_length = 1 + (chunk_byte * 3);
		}
	}
	goto next_chunk;
done:
	if (unlikely(ofs > infile_size))
		failure_goto(("Offset greater than archive size, likely corrupted/truncated archive.\n"), error);
	print_verbose("Rzip compression: %.1f%% %lld / %lld\n",
			percentage (utotal, expected_size),
			utotal, expected_size);
	print_verbose("Back end compression: %.1f%% %lld / %lld\n",
			percentage(ctotal, utotal),
			ctotal, utotal);
	print_verbose("Overall compression: %.1f%% %lld / %lld\n",
			percentage(ctotal, expected_size),
			ctotal, expected_size);

	cratio = (long double)expected_size / (long double)infile_size;

	print_output("%s:\nlrzip version: %d.%d file\n", infilecopy, control->major_version, control->minor_version);

	print_output("Compression: ");
	if (save_ctype == CTYPE_NONE)
		print_output("rzip alone\n");
	else if (save_ctype == CTYPE_BZIP2)
		print_output("rzip + bzip2\n");
	else if (save_ctype == CTYPE_LZO)
		print_output("rzip + lzo\n");
	else if (save_ctype == CTYPE_LZMA)
		print_output("rzip + lzma\n");
	else if (save_ctype == CTYPE_GZIP)
		print_output("rzip + gzip\n");
	else if (save_ctype == CTYPE_ZPAQ)
		print_output("rzip + zpaq\n");
	else
		print_output("Dunno wtf\n");
	print_output("Decompressed file size: %llu\n", expected_size);
	print_output("Compressed file size: %llu\n", infile_size);
	print_output("Compression ratio: %.3Lf\n", cratio);

	if (HAS_MD5) {
		char md5_stored[MD5_DIGEST_SIZE];
		int i;

		print_output("MD5 used for integrity testing\n");
		if (unlikely(lseek(fd_in, -MD5_DIGEST_SIZE, SEEK_END) == -1))
			fatal_goto(("Failed to seek to md5 data in runzip_fd\n"), error);
		if (unlikely(read(fd_in, md5_stored, MD5_DIGEST_SIZE) != MD5_DIGEST_SIZE))
			fatal_goto(("Failed to read md5 data in runzip_fd\n"), error);
		print_output("MD5: ");
		for (i = 0; i < MD5_DIGEST_SIZE; i++)
			print_output("%02x", md5_stored[i] & 0xFF);
		print_output("\n");
	} else
		print_output("CRC32 used for integrity testing\n");
	if ( !IS_FROM_FILE )
		if (unlikely(close(fd_in)))
			fatal_return(("Failed to close fd_in in get_fileinfo\n"), false);

out:
	dealloc(control->outfile);
	return true;
error:
	if (!STDIN && ! IS_FROM_FILE) close(fd_in);
	return false;
}