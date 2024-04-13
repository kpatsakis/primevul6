uint64_t smb_roundup(connection_struct *conn, uint64_t val)
{
	uint64_t rval = lp_allocation_roundup_size(SNUM(conn));

	/* Only roundup for Windows clients. */
	enum remote_arch_types ra_type = get_remote_arch();
	if (rval && (ra_type != RA_SAMBA) && (ra_type != RA_CIFSFS)) {
		val = SMB_ROUNDUP(val,rval);
	}
	return val;
}