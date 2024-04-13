void Resolver::TriggerCachedResult()
{
	if (CQ)
		OnLookupComplete(CQ->data, time_left, true);
}