int DNS::ClearCache()
{
	/* This ensures the buckets are reset to sane levels */
	int rv = this->cache->size();
	delete this->cache;
	this->cache = new dnscache();
	return rv;
}