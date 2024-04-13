struct rad_packet_t *rad_packet_alloc(int code)
{
	struct rad_packet_t *pack;

	pack = mempool_alloc(packet_pool);
	if (!pack) {
		log_emerg("radius:packet: out of memory\n");
		return NULL;
	}

	memset(pack, 0, sizeof(*pack));
	pack->code = code;
	pack->len = 20;
	pack->id = 1;
	INIT_LIST_HEAD(&pack->attrs);

	return pack;
}