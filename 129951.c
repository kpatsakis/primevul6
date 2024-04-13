sigbuffer_init (SigBuffer *buf, int size)
{
	buf->buf = g_malloc (size);
	buf->p = buf->buf;
	buf->end = buf->buf + size;
}