archive_read_format_cpio_bid(struct archive_read *a, int best_bid)
{
	const unsigned char *p;
	struct cpio *cpio;
	int bid;

	(void)best_bid; /* UNUSED */

	cpio = (struct cpio *)(a->format->data);

	if ((p = __archive_read_ahead(a, 6, NULL)) == NULL)
		return (-1);

	bid = 0;
	if (memcmp(p, "070707", 6) == 0) {
		/* ASCII cpio archive (odc, POSIX.1) */
		cpio->read_header = header_odc;
		bid += 48;
		/*
		 * XXX TODO:  More verification; Could check that only octal
		 * digits appear in appropriate header locations. XXX
		 */
	} else if (memcmp(p, "070727", 6) == 0) {
		/* afio large ASCII cpio archive */
		cpio->read_header = header_odc;
		bid += 48;
		/*
		 * XXX TODO:  More verification; Could check that almost hex
		 * digits appear in appropriate header locations. XXX
		 */
	} else if (memcmp(p, "070701", 6) == 0) {
		/* ASCII cpio archive (SVR4 without CRC) */
		cpio->read_header = header_newc;
		bid += 48;
		/*
		 * XXX TODO:  More verification; Could check that only hex
		 * digits appear in appropriate header locations. XXX
		 */
	} else if (memcmp(p, "070702", 6) == 0) {
		/* ASCII cpio archive (SVR4 with CRC) */
		/* XXX TODO: Flag that we should check the CRC. XXX */
		cpio->read_header = header_newc;
		bid += 48;
		/*
		 * XXX TODO:  More verification; Could check that only hex
		 * digits appear in appropriate header locations. XXX
		 */
	} else if (p[0] * 256 + p[1] == 070707) {
		/* big-endian binary cpio archives */
		cpio->read_header = header_bin_be;
		bid += 16;
		/* Is more verification possible here? */
	} else if (p[0] + p[1] * 256 == 070707) {
		/* little-endian binary cpio archives */
		cpio->read_header = header_bin_le;
		bid += 16;
		/* Is more verification possible here? */
	} else
		return (ARCHIVE_WARN);

	return (bid);
}