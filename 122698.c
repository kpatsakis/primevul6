static int nfs3_get_attributes_offset(uint32_t *data)
{
	if (ntohl(data[1]) != 0) {
		/* 'attributes_follow' flag is TRUE,
		 * so we have attributes on 21 dwords */
		/* Skip unused values :
			type;	32 bits value,
			mode;	32 bits value,
			nlink;	32 bits value,
			uid;	32 bits value,
			gid;	32 bits value,
			size;	64 bits value,
			used;	64 bits value,
			rdev;	64 bits value,
			fsid;	64 bits value,
			fileid;	64 bits value,
			atime;	64 bits value,
			mtime;	64 bits value,
			ctime;	64 bits value,
		*/
		return 22;
	} else {
		/* 'attributes_follow' flag is FALSE,
		 * so we don't have any attributes */
		return 1;
	}
}