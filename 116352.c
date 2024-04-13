archive_read_format_cpio_cleanup(struct archive_read *a)
{
	struct cpio *cpio;

	cpio = (struct cpio *)(a->format->data);
        /* Free inode->name map */
        while (cpio->links_head != NULL) {
                struct links_entry *lp = cpio->links_head->next;

                if (cpio->links_head->name)
                        free(cpio->links_head->name);
                free(cpio->links_head);
                cpio->links_head = lp;
        }
	free(cpio);
	(a->format->data) = NULL;
	return (ARCHIVE_OK);
}