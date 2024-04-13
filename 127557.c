		void create(bool userSwitching, const string &defaultUser,
		            const string &defaultGroup, uid_t webServerWorkerUid,
		            gid_t webServerWorkerGid)
		{
			TRACE_POINT();
			bool runningAsRoot = geteuid() == 0;
			struct passwd *defaultUserEntry;
			uid_t defaultUid;
			gid_t defaultGid;
			
			defaultUserEntry = getpwnam(defaultUser.c_str());
			if (defaultUserEntry == NULL) {
				throw NonExistentUserException("Default user '" + defaultUser +
					"' does not exist.");
			}
			defaultUid = defaultUserEntry->pw_uid;
			defaultGid = lookupGid(defaultGroup);
			if (defaultGid == (gid_t) -1) {
				throw NonExistentGroupException("Default group '" + defaultGroup +
					"' does not exist.");
			}
			
			/* We set a very tight permission here: no read or write access for
			 * anybody except the owner. The individual files and subdirectories
			 * decide for themselves whether they're readable by anybody.
			 */
			makeDirTree(path, "u=rwx,g=x,o=x");
			
			/* Write structure version file. */
			string structureVersionFile = path + "/structure_version.txt";
			createFile(structureVersionFile,
				toString(SERVER_INSTANCE_DIR_GENERATION_STRUCTURE_MAJOR_VERSION) + "." +
				toString(SERVER_INSTANCE_DIR_GENERATION_STRUCTURE_MINOR_VERSION),
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			
			string passengerVersionFile = path + "/passenger_version.txt";
			createFile(passengerVersionFile,
				PASSENGER_VERSION "\n",
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			
			
			/* We want the upload buffer directory to be only writable by the web
			 * server's worker processs. Other users may not have any access to this
			 * directory.
			 */
			if (runningAsRoot) {
				makeDirTree(path + "/buffered_uploads", "u=rwx,g=,o=",
					webServerWorkerUid, webServerWorkerGid);
			} else {
				makeDirTree(path + "/buffered_uploads", "u=rwx,g=,o=");
			}
			
			/* The HelperAgent must be able to connect to an application. */
			if (runningAsRoot) {
				if (userSwitching) {
					/* Each application process may be running as a different user,
					 * so the backends subdirectory must be world-writable.
					 * However we don't want everybody to be able to know the
					 * sockets' filenames, so the directory is not readable.
					 */
					makeDirTree(path + "/backends", "u=rwx,g=wx,o=wx,+t");
				} else {
					/* All application processes are running as defaultUser/defaultGroup,
					 * so make defaultUser/defaultGroup the owner and group of the
					 * subdirecory.
					 *
					 * The directory is not readable as a security precaution:
					 * nobody should be able to know the sockets' filenames without
					 * having access to the application pool.
					 */
					makeDirTree(path + "/backends", "u=rwx,g=x,o=x", defaultUid, defaultGid);
				}
			} else {
				/* All application processes are running as the same user as the web server,
				 * so only allow access for this user.
				 */
				makeDirTree(path + "/backends", "u=rwx,g=,o=");
			}
			
			owner = true;
		}