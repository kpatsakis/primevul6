static void set_nettime(void)
{
	wr_lock(&netacc_lock);
	gettimeofday(&nettime, NULL);
	wr_unlock(&netacc_lock);
}