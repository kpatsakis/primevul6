CachedQuery::CachedQuery(const std::string &res, unsigned int ttl) : data(res)
{
	expires = ServerInstance->Time() + ttl;
}