rndget(void)
{
	/*
	 * this is the same Linear Congruential PRNG as Borland
	 * C/C++ allegedly uses in its built-in rand() function
	 */
	return (((lcg_state = 22695477 * lcg_state + 1) >> 16) & 0x7FFF);
}