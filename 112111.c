reset_shared(void)
{
	/*
	 * Create or re-create shared memory and semaphores.
	 *
	 * Note: in each "cycle of life" we will normally assign the same IPC keys
	 * (if using SysV shmem and/or semas).  This helps ensure that we will
	 * clean up dead IPC objects if the postmaster crashes and is restarted.
	 */
	CreateSharedMemoryAndSemaphores();
}