void DNS::CleanResolvers(Module* module)
{
	for (int i = 0; i < MAX_REQUEST_ID; i++)
	{
		if (Classes[i])
		{
			if (Classes[i]->GetCreator() == module)
			{
				Classes[i]->OnError(RESOLVER_FORCEUNLOAD, "Parent module is unloading");
				delete Classes[i];
				Classes[i] = NULL;
			}
		}
	}
}