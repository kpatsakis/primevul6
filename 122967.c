RADIUS_PACKET *rad_alloc(int newvector)
{
	RADIUS_PACKET	*rp;

	if ((rp = malloc(sizeof(RADIUS_PACKET))) == NULL) {
		librad_log("out of memory");
		return NULL;
	}
	memset(rp, 0, sizeof(RADIUS_PACKET));
	if (newvector) {
		int i;
		uint32_t hash, base;

		/*
		 *	Don't expose the actual contents of the random
		 *	pool.
		 */
		base = lrad_rand();
		for (i = 0; i < AUTH_VECTOR_LEN; i += sizeof(uint32_t)) {
			hash = lrad_rand() ^ base;
			memcpy(rp->vector + i, &hash, sizeof(hash));
		}
	}
	lrad_rand();

	return rp;
}