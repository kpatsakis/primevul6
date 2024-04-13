void DNS::DelCache(const std::string &source)
{
	cache->erase(source.c_str());
}