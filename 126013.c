bool __init __attribute((weak)) arch_hugetlb_valid_size(unsigned long size)
{
	return size == HPAGE_SIZE;
}