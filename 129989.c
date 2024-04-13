sigbuffer_add_byte (SigBuffer *buf, guint8 val)
{
	sigbuffer_make_room (buf, 1);
	buf->p [0] = val;
	buf->p++;
}