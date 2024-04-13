void lrad_rand_seed(const void *data, size_t size)
{
	uint32_t hash;

	/*
	 *	Ensure that the pool is initialized.
	 */
	if (lrad_rand_index < 0) {
		int fd;
		
		memset(&lrad_rand_pool, 0, sizeof(lrad_rand_pool));

		fd = open("/dev/urandom", O_RDONLY);
		if (fd >= 0) {
			size_t total;
			ssize_t this;

			total = this = 0;
			while (total < sizeof(lrad_rand_pool.randrsl)) {
				this = read(fd, lrad_rand_pool.randrsl,
					    sizeof(lrad_rand_pool.randrsl) - total);
				if ((this < 0) && (errno != EINTR)) break;
				if (this > 0) total += this;
 			}
			close(fd);
		} else {
			lrad_rand_pool.randrsl[0] = fd;
			lrad_rand_pool.randrsl[1] = time(NULL);
			lrad_rand_pool.randrsl[2] = errno;
		}

		lrad_randinit(&lrad_rand_pool, 1);
		lrad_rand_index = 0;
	}

	if (!data) return;

	/*
	 *	Hash the user data
	 */
	hash = lrad_hash(data, size);
	
	lrad_rand_pool.randrsl[lrad_rand_index & 0xff] ^= hash;
	lrad_rand_index++;
	lrad_rand_index &= 0xff;

	/*
	 *	Churn the pool every so often after seeding it.
	 */
	if (((int) (hash & 0xff)) == lrad_rand_index) {
		lrad_isaac(&lrad_rand_pool);
	}
}