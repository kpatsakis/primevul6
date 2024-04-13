bool DNS::AddResolverClass(Resolver* r)
{
	ServerInstance->Logs->Log("RESOLVER",DEBUG,"AddResolverClass 0x%08lx", (unsigned long)r);
	/* Check the pointers validity and the id's validity */
	if ((r) && (r->GetId() > -1))
	{
		/* Check the slot isnt already occupied -
		 * This should NEVER happen unless we have
		 * a severely broken DNS server somewhere
		 */
		if (!Classes[r->GetId()])
		{
			/* Set up the pointer to the class */
			Classes[r->GetId()] = r;
			return true;
		}
		else
			/* Duplicate id */
			return false;
	}
	else
	{
		/* Pointer or id not valid.
		 * Free the item and return
		 */
		if (r)
			delete r;

		return false;
	}
}