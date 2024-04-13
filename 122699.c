static void nfs_read_req(int offset, int readlen)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	p = &(data[0]);
	p = rpc_add_credentials(p);

	if (supported_nfs_versions & NFSV2_FLAG) {
		memcpy(p, filefh, NFS_FHSIZE);
		p += (NFS_FHSIZE / 4);
		*p++ = htonl(offset);
		*p++ = htonl(readlen);
		*p++ = 0;
	} else { /* NFSV3_FLAG */
		*p++ = htonl(filefh3_length);
		memcpy(p, filefh, filefh3_length);
		p += (filefh3_length / 4);
		*p++ = htonl(0); /* offset is 64-bit long, so fill with 0 */
		*p++ = htonl(offset);
		*p++ = htonl(readlen);
		*p++ = 0;
	}

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req(PROG_NFS, NFS_READ, data, len);
}