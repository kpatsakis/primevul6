find_odc_header(struct archive_read *a)
{
	const void *h;
	const char *p, *q;
	size_t skip, skipped = 0;
	ssize_t bytes;

	for (;;) {
		h = __archive_read_ahead(a, odc_header_size, &bytes);
		if (h == NULL)
			return (ARCHIVE_FATAL);
		p = h;
		q = p + bytes;

		/* Try the typical case first, then go into the slow search.*/
		if (memcmp("070707", p, 6) == 0 && is_octal(p, odc_header_size))
			return (ARCHIVE_OK);
		if (memcmp("070727", p, 6) == 0 && is_afio_large(p, bytes)) {
			a->archive.archive_format = ARCHIVE_FORMAT_CPIO_AFIO_LARGE;
			return (ARCHIVE_OK);
		}

		/*
		 * Scan ahead until we find something that looks
		 * like an odc header.
		 */
		while (p + odc_header_size <= q) {
			switch (p[5]) {
			case '7':
				if ((memcmp("070707", p, 6) == 0
				    && is_octal(p, odc_header_size))
				    || (memcmp("070727", p, 6) == 0
				        && is_afio_large(p, q - p))) {
					skip = p - (const char *)h;
					__archive_read_consume(a, skip);
					skipped += skip;
					if (p[4] == '2')
						a->archive.archive_format =
						    ARCHIVE_FORMAT_CPIO_AFIO_LARGE;
					if (skipped > 0) {
						archive_set_error(&a->archive,
						    0,
						    "Skipped %d bytes before "
						    "finding valid header",
						    (int)skipped);
						return (ARCHIVE_WARN);
					}
					return (ARCHIVE_OK);
				}
				p += 2;
				break;
			case '0':
				p++;
				break;
			default:
				p += 6;
				break;
			}
		}
		skip = p - (const char *)h;
		__archive_read_consume(a, skip);
		skipped += skip;
	}
}