uint32_t lrad_rand(void)
{
	uint32_t num;

	/*
	 *	Ensure that the pool is initialized.
	 */
	if (lrad_rand_index < 0) {
		lrad_rand_seed(NULL, 0);
	}

	/*
	 *	We don't return data directly from the pool.
	 *	Rather, we return a summary of the data.
	 */
	num = lrad_rand_pool.randrsl[lrad_rand_index & 0xff];
	lrad_rand_index++;
	lrad_rand_index &= 0xff;

	/*
	 *	Every so often, churn the pool.
	 */
	if (((int) (num & 0xff)) == lrad_rand_index) {
		lrad_isaac(&lrad_rand_pool);
	}

	return num;
}