static int compat_copy_strings(int argc, compat_uptr_t __user *argv,
				struct linux_binprm *bprm)
{
	struct page *kmapped_page = NULL;
	char *kaddr = NULL;
	unsigned long kpos = 0;
	int ret;

	while (argc-- > 0) {
		compat_uptr_t str;
		int len;
		unsigned long pos;

		if (get_user(str, argv+argc) ||
		    !(len = strnlen_user(compat_ptr(str), MAX_ARG_STRLEN))) {
			ret = -EFAULT;
			goto out;
		}

		if (len > MAX_ARG_STRLEN) {
			ret = -E2BIG;
			goto out;
		}

		/* We're going to work our way backwords. */
		pos = bprm->p;
		str += len;
		bprm->p -= len;

		while (len > 0) {
			int offset, bytes_to_copy;

			offset = pos % PAGE_SIZE;
			if (offset == 0)
				offset = PAGE_SIZE;

			bytes_to_copy = offset;
			if (bytes_to_copy > len)
				bytes_to_copy = len;

			offset -= bytes_to_copy;
			pos -= bytes_to_copy;
			str -= bytes_to_copy;
			len -= bytes_to_copy;

			if (!kmapped_page || kpos != (pos & PAGE_MASK)) {
				struct page *page;

#ifdef CONFIG_STACK_GROWSUP
				ret = expand_stack_downwards(bprm->vma, pos);
				if (ret < 0) {
					/* We've exceed the stack rlimit. */
					ret = -E2BIG;
					goto out;
				}
#endif
				ret = get_user_pages(current, bprm->mm, pos,
						     1, 1, 1, &page, NULL);
				if (ret <= 0) {
					/* We've exceed the stack rlimit. */
					ret = -E2BIG;
					goto out;
				}

				if (kmapped_page) {
					flush_kernel_dcache_page(kmapped_page);
					kunmap(kmapped_page);
					put_page(kmapped_page);
				}
				kmapped_page = page;
				kaddr = kmap(kmapped_page);
				kpos = pos & PAGE_MASK;
				flush_cache_page(bprm->vma, kpos,
						 page_to_pfn(kmapped_page));
			}
			if (copy_from_user(kaddr+offset, compat_ptr(str),
						bytes_to_copy)) {
				ret = -EFAULT;
				goto out;
			}
		}
	}
	ret = 0;
out:
	if (kmapped_page) {
		flush_kernel_dcache_page(kmapped_page);
		kunmap(kmapped_page);
		put_page(kmapped_page);
	}
	return ret;
}