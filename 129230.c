static int load_aout_library(struct file *file)
{
	struct inode * inode;
	unsigned long bss, start_addr, len;
	unsigned long error;
	int retval;
	struct exec ex;

	inode = file->f_path.dentry->d_inode;

	retval = -ENOEXEC;
	error = kernel_read(file, 0, (char *) &ex, sizeof(ex));
	if (error != sizeof(ex))
		goto out;

	/* We come in here for the regular a.out style of shared libraries */
	if ((N_MAGIC(ex) != ZMAGIC && N_MAGIC(ex) != QMAGIC) || N_TRSIZE(ex) ||
	    N_DRSIZE(ex) || ((ex.a_entry & 0xfff) && N_MAGIC(ex) == ZMAGIC) ||
	    i_size_read(inode) < ex.a_text+ex.a_data+N_SYMSIZE(ex)+N_TXTOFF(ex)) {
		goto out;
	}

	if (N_FLAGS(ex))
		goto out;

	/* For  QMAGIC, the starting address is 0x20 into the page.  We mask
	   this off to get the starting address for the page */

	start_addr =  ex.a_entry & 0xfffff000;

	if ((N_TXTOFF(ex) & ~PAGE_MASK) != 0) {
		loff_t pos = N_TXTOFF(ex);

#ifdef WARN_OLD
		static unsigned long error_time;
		if ((jiffies-error_time) > 5*HZ)
		{
			printk(KERN_WARNING 
			       "N_TXTOFF is not page aligned. Please convert library: %s\n",
			       file->f_path.dentry->d_name.name);
			error_time = jiffies;
		}
#endif
		down_write(&current->mm->mmap_sem);
		do_brk(start_addr, ex.a_text + ex.a_data + ex.a_bss);
		up_write(&current->mm->mmap_sem);
		
		file->f_op->read(file, (char __user *)start_addr,
			ex.a_text + ex.a_data, &pos);
		flush_icache_range((unsigned long) start_addr,
				   (unsigned long) start_addr + ex.a_text + ex.a_data);

		retval = 0;
		goto out;
	}
	/* Now use mmap to map the library into memory. */
	down_write(&current->mm->mmap_sem);
	error = do_mmap(file, start_addr, ex.a_text + ex.a_data,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_FIXED | MAP_PRIVATE | MAP_DENYWRITE | MAP_32BIT,
			N_TXTOFF(ex));
	up_write(&current->mm->mmap_sem);
	retval = error;
	if (error != start_addr)
		goto out;

	len = PAGE_ALIGN(ex.a_text + ex.a_data);
	bss = ex.a_text + ex.a_data + ex.a_bss;
	if (bss > len) {
		down_write(&current->mm->mmap_sem);
		error = do_brk(start_addr + len, bss - len);
		up_write(&current->mm->mmap_sem);
		retval = error;
		if (error != start_addr + len)
			goto out;
	}
	retval = 0;
out:
	return retval;
}