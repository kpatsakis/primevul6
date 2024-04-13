int DNS::PruneCache()
{
	int n = 0;
	dnscache* newcache = new dnscache();
	for (dnscache::iterator i = this->cache->begin(); i != this->cache->end(); i++)
		/* Dont include expired items (theres no point) */
		if (i->second.CalcTTLRemaining())
			newcache->insert(*i);
		else
			n++;

	delete this->cache;
	this->cache = newcache;
	return n;
}