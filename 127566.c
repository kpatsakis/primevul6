	GenerationPtr newGeneration(bool userSwitching, const string &defaultUser,
	                            const string &defaultGroup, uid_t webServerWorkerUid,
	                            gid_t webServerWorkerGid)
	{
		GenerationPtr newestGeneration = getNewestGeneration();
		unsigned int newNumber;
		if (newestGeneration != NULL) {
			newNumber = newestGeneration->getNumber() + 1;
		} else {
			newNumber = 0;
		}
		
		GenerationPtr generation(new Generation(path, newNumber));
		generation->create(userSwitching, defaultUser, defaultGroup,
			webServerWorkerUid, webServerWorkerGid);
		return generation;
	}