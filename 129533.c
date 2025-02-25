read_eocd(struct zip *zip, const char *p, int64_t current_offset)
{
	/* Sanity-check the EOCD we've found. */

	/* This must be the first volume. */
	if (archive_le16dec(p + 4) != 0)
		return 0;
	/* Central directory must be on this volume. */
	if (archive_le16dec(p + 4) != archive_le16dec(p + 6))
		return 0;
	/* All central directory entries must be on this volume. */
	if (archive_le16dec(p + 10) != archive_le16dec(p + 8))
		return 0;
	/* Central directory can't extend beyond start of EOCD record. */
	if (archive_le32dec(p + 16) + archive_le32dec(p + 12)
	    > current_offset)
		return 0;

	/* Save the central directory location for later use. */
	zip->central_directory_offset = archive_le32dec(p + 16);

	/* This is just a tiny bit higher than the maximum
	   returned by the streaming Zip bidder.  This ensures
	   that the more accurate seeking Zip parser wins
	   whenever seek is available. */
	return 32;
}