u32 hugetlb_fault_mutex_hash(struct address_space *mapping, pgoff_t idx)
{
	unsigned long key[2];
	u32 hash;

	key[0] = (unsigned long) mapping;
	key[1] = idx;

	hash = jhash2((u32 *)&key, sizeof(key)/(sizeof(u32)), 0);

	return hash & (num_fault_mutexes - 1);
}