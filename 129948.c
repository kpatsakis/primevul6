sigbuffer_free (SigBuffer *buf)
{
	g_free (buf->buf);
}