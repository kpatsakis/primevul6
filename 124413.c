bool compress_file(rzip_control *control)
{
	const char *tmp, *tmpinfile; 	/* we're just using this as a proxy for control->infile.
					 * Spares a compiler warning
					 */
	int fd_in = -1, fd_out = -1;
	char header[MAGIC_LEN];

	if (MD5_RELIABLE)
		control->flags |= FLAG_MD5;
	if (ENCRYPT)
		if (unlikely(!get_hash(control, 1)))
			return false;
	memset(header, 0, sizeof(header));

	if ( IS_FROM_FILE )
		fd_in = fileno(control->inFILE);
	else if (!STDIN) {
		 /* is extension at end of infile? */
		if ((tmp = strrchr(control->infile, '.')) && !strcmp(tmp, control->suffix)) {
			print_err("%s: already has %s suffix. Skipping...\n", control->infile, control->suffix);
			return false;
		}

        fd_in = open(control->infile, O_RDONLY);
		if (unlikely(fd_in == -1))
			fatal_return(("Failed to open %s\n", control->infile), false);
	} 
	else
		fd_in = 0;

	if (!STDOUT) {
		if (control->outname) {
				/* check if outname has control->suffix */
				if (*(control->suffix) == '\0') /* suffix is empty string */
					control->outfile = strdup(control->outname);
				else if ((tmp=strrchr(control->outname, '.')) && strcmp(tmp, control->suffix)) {
					control->outfile = malloc(strlen(control->outname) + strlen(control->suffix) + 1);
					if (unlikely(!control->outfile))
						fatal_goto(("Failed to allocate outfile name\n"), error);
					strcpy(control->outfile, control->outname);
					strcat(control->outfile, control->suffix);
					print_output("Suffix added to %s.\nFull pathname is: %s\n", control->outname, control->outfile);
				} else	/* no, already has suffix */
					control->outfile = strdup(control->outname);
		} else {
			/* default output name from control->infile
			 * test if outdir specified. If so, strip path from filename of
			 * control->infile
			*/
			if (control->outdir && (tmp = strrchr(control->infile, '/')))
				tmpinfile = tmp + 1;
			else
				tmpinfile = control->infile;

			control->outfile = malloc((control->outdir == NULL? 0: strlen(control->outdir)) + strlen(tmpinfile) + strlen(control->suffix) + 1);
			if (unlikely(!control->outfile))
				fatal_goto(("Failed to allocate outfile name\n"), error);

			if (control->outdir) {	/* prepend control->outdir */
				strcpy(control->outfile, control->outdir);
				strcat(control->outfile, tmpinfile);
			} else
				strcpy(control->outfile, tmpinfile);
			strcat(control->outfile, control->suffix);
			print_progress("Output filename is: %s\n", control->outfile);
		}

		fd_out = open(control->outfile, O_RDWR | O_CREAT | O_EXCL, 0666);
		if (FORCE_REPLACE && (-1 == fd_out) && (EEXIST == errno)) {
			if (unlikely(unlink(control->outfile)))
				fatal_goto(("Failed to unlink an existing file: %s\n", control->outfile), error);
			fd_out = open(control->outfile, O_RDWR | O_CREAT | O_EXCL, 0666);
		}
		if (unlikely(fd_out == -1)) {
			/* We must ensure we don't delete a file that already
			 * exists just because we tried to create a new one */
			control->flags |= FLAG_KEEP_BROKEN;
			fatal_goto(("Failed to create %s\n", control->outfile), error);
		}
		control->fd_out = fd_out;
		if (!STDIN) {
			if (unlikely(!preserve_perms(control, fd_in, fd_out)))
				goto error;
		}
	} else {
		if (unlikely(!open_tmpoutbuf(control)))
			goto error;
	}

	/* Write zeroes to header at beginning of file */
	if (unlikely(!STDOUT && write(fd_out, header, sizeof(header)) != sizeof(header)))
		fatal_goto(("Cannot write file header\n"), error);

	rzip_fd(control, fd_in, fd_out);

	/* Wwrite magic at end b/c lzma does not tell us properties until it is done */
	if (!STDOUT) {
		if (unlikely(!write_magic(control)))
			goto error;
	}

	if (ENCRYPT)
		release_hashes(control);

	if (unlikely(!STDIN && !STDOUT && !preserve_times(control, fd_in))) {
		fatal("Failed to preserve times on output file\n");
		goto error;
	}

	if (unlikely(close(fd_in))) {
		fatal("Failed to close fd_in\n");
		fd_in = -1;
		goto error;
	}
	if (unlikely(!STDOUT && close(fd_out)))
		fatal_return(("Failed to close fd_out\n"), false);
	if (TMP_OUTBUF)
		close_tmpoutbuf(control);

	if (!KEEP_FILES && !STDIN) {
		if (unlikely(unlink(control->infile)))
			fatal_return(("Failed to unlink %s\n", control->infile), false);
	}

	dealloc(control->outfile);
	return true;
error:
	if (! IS_FROM_FILE && STDIN && (fd_in > 0))
		close(fd_in);
	if ((!STDOUT) && (fd_out > 0))
		close(fd_out);
	return false;
}