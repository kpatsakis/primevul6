static uint32_t *rpc_add_credentials(uint32_t *p)
{
	/* Here's the executive summary on authentication requirements of the
	 * various NFS server implementations:	Linux accepts both AUTH_NONE
	 * and AUTH_UNIX authentication (also accepts an empty hostname field
	 * in the AUTH_UNIX scheme).  *BSD refuses AUTH_NONE, but accepts
	 * AUTH_UNIX (also accepts an empty hostname field in the AUTH_UNIX
	 * scheme).  To be safe, use AUTH_UNIX and pass the hostname if we have
	 * it (if the BOOTP/DHCP reply didn't give one, just use an empty
	 * hostname).  */

	/* Provide an AUTH_UNIX credential.  */
	*p++ = htonl(1);		/* AUTH_UNIX */
	*p++ = htonl(20);		/* auth length */
	*p++ = 0;			/* stamp */
	*p++ = 0;			/* hostname string */
	*p++ = 0;			/* uid */
	*p++ = 0;			/* gid */
	*p++ = 0;			/* auxiliary gid list */

	/* Provide an AUTH_NONE verifier.  */
	*p++ = 0;			/* AUTH_NONE */
	*p++ = 0;			/* auth length */

	return p;
}