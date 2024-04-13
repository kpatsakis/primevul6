DNS::~DNS()
{
	ServerInstance->SE->Shutdown(this, 2);
	ServerInstance->SE->Close(this);
	ServerInstance->Timers->DelTimer(this->PruneTimer);
	if (cache)
		delete cache;
}