static inline size_t buf_room_for_htx_data(const struct buffer *buf)
{
	size_t room;

	room = b_room(buf);
	if (room <= sizeof(struct htx) + 2 * sizeof(struct htx_blk))
		room = 0;
	else
		room -= sizeof(struct htx) + 2 * sizeof(struct htx_blk);

	return room;
}