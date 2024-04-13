CachedQuery::CachedQuery(const std::string &res, QueryType qt, unsigned int ttl) : data(res), type(qt)
{
	expires = ServerInstance->Time() + ttl;
}