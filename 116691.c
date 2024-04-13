int update_server_info(int force)
{
	/* We would add more dumb-server support files later,
	 * including index of available pack files and their
	 * intended audiences.
	 */
	int errs = 0;

	errs = errs | update_info_refs(force);
	errs = errs | update_info_packs(force);

	/* remove leftover rev-cache file if there is any */
	unlink_or_warn(git_path("info/rev-cache"));

	return errs;
}