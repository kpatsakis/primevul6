sigbuffer_add_to_blob_cached (MonoDynamicImage *assembly, SigBuffer *buf)
{
	char blob_size [8];
	char *b = blob_size;
	guint32 size = buf->p - buf->buf;
	/* store length */
	g_assert (size <= (buf->end - buf->buf));
	mono_metadata_encode_value (size, b, &b);
	return add_to_blob_cached (assembly, blob_size, b-blob_size, buf->buf, size);
}