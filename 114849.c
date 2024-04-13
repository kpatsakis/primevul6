bool restart_stratum(struct pool *pool)
{
	if (!initiate_stratum(pool))
		return false;
	if (!auth_stratum(pool))
		return false;
	return true;
}