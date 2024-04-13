	__releases(rose_neigh_list_lock)
{
	spin_unlock_bh(&rose_neigh_list_lock);
}