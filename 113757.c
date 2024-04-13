CachedQuery* DNS::GetCache(const std::string &source)
{
	dnscache::iterator x = cache->find(source.c_str());
	if (x != cache->end())
		return &(x->second);
	else
		return NULL;
}