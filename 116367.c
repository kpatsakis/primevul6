record_hardlink(struct archive_read *a,
    struct cpio *cpio, struct archive_entry *entry)
{
	struct links_entry      *le;
	dev_t dev;
	int64_t ino;

	if (archive_entry_nlink(entry) <= 1)
		return (ARCHIVE_OK);

	dev = archive_entry_dev(entry);
	ino = archive_entry_ino64(entry);

	/*
	 * First look in the list of multiply-linked files.  If we've
	 * already dumped it, convert this entry to a hard link entry.
	 */
	for (le = cpio->links_head; le; le = le->next) {
		if (le->dev == dev && le->ino == ino) {
			archive_entry_copy_hardlink(entry, le->name);

			if (--le->links <= 0) {
				if (le->previous != NULL)
					le->previous->next = le->next;
				if (le->next != NULL)
					le->next->previous = le->previous;
				if (cpio->links_head == le)
					cpio->links_head = le->next;
				free(le->name);
				free(le);
			}

			return (ARCHIVE_OK);
		}
	}

	le = (struct links_entry *)malloc(sizeof(struct links_entry));
	if (le == NULL) {
		archive_set_error(&a->archive,
		    ENOMEM, "Out of memory adding file to list");
		return (ARCHIVE_FATAL);
	}
	if (cpio->links_head != NULL)
		cpio->links_head->previous = le;
	le->next = cpio->links_head;
	le->previous = NULL;
	cpio->links_head = le;
	le->dev = dev;
	le->ino = ino;
	le->links = archive_entry_nlink(entry) - 1;
	le->name = strdup(archive_entry_pathname(entry));
	if (le->name == NULL) {
		archive_set_error(&a->archive,
		    ENOMEM, "Out of memory adding file to list");
		return (ARCHIVE_FATAL);
	}

	return (ARCHIVE_OK);
}