cmp_key(const struct archive_rb_node *n, const void *key)
{
	/* This function won't be called */
	(void)n; /* UNUSED */
	(void)key; /* UNUSED */
	return 1;
}