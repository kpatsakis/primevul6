int CachedQuery::CalcTTLRemaining()
{
	int n = expires - ServerInstance->Time();
	return (n < 0 ? 0 : n);
}