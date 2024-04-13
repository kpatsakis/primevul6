	__releases(rose_route_list_lock)
{
	spin_unlock_bh(&rose_route_list_lock);
}