static inline int store_block(uchar *src, unsigned offset, unsigned len)
{
	ulong newsize = offset + len;
#ifdef CONFIG_SYS_DIRECT_FLASH_NFS
	int i, rc = 0;

	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; i++) {
		/* start address in flash? */
		if (load_addr + offset >= flash_info[i].start[0]) {
			rc = 1;
			break;
		}
	}

	if (rc) { /* Flash is destination for this packet */
		rc = flash_write((uchar *)src, (ulong)(load_addr+offset), len);
		if (rc) {
			flash_perror(rc);
			return -1;
		}
	} else
#endif /* CONFIG_SYS_DIRECT_FLASH_NFS */
	{
		void *ptr = map_sysmem(load_addr + offset, len);

		memcpy(ptr, src, len);
		unmap_sysmem(ptr);
	}

	if (net_boot_file_size < (offset + len))
		net_boot_file_size = newsize;
	return 0;
}