static void init(void)
{
	attr_pool = mempool_create(sizeof(struct rad_attr_t));
	packet_pool = mempool_create(sizeof(struct rad_packet_t));
	buf_pool = mempool_create(REQ_LENGTH_MAX);
}