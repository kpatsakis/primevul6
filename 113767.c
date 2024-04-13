	CacheTimer(DNS* thisdns)
		: Timer(3600, ServerInstance->Time(), true), dns(thisdns) { }