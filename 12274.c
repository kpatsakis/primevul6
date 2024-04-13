	__releases(rose_node_list_lock)
{
	spin_unlock_bh(&rose_node_list_lock);
}