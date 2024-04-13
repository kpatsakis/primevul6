DNS::DNS()
{
	ServerInstance->Logs->Log("RESOLVER",DEBUG,"DNS::DNS");
	/* Clear the Resolver class table */
	memset(Classes,0,sizeof(Classes));

	/* Clear the requests class table */
	memset(requests,0,sizeof(requests));

	/* Set the id of the next request to 0
	 */
	currid = 0;

	/* DNS::Rehash() sets this to a valid ptr
	 */
	this->cache = NULL;

	/* Again, DNS::Rehash() sets this to a
	 * valid value
	 */
	this->SetFd(-1);

	/* Actually read the settings
	 */
	this->Rehash();

	this->PruneTimer = new CacheTimer(this);

	ServerInstance->Timers->AddTimer(this->PruneTimer);
}