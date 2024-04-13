static void databuf_free(struct data_buffer *db)
{
	if (!db)
		return;

	free(db->buf);
#ifdef DEBUG_DATABUF
	applog(LOG_DEBUG, "databuf_free(%p)", db->buf);
#endif

	memset(db, 0, sizeof(*db));
}