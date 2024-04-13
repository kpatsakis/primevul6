static int __init cmdline_parse_hugetlb_cma(char *p)
{
	int nid, count = 0;
	unsigned long tmp;
	char *s = p;

	while (*s) {
		if (sscanf(s, "%lu%n", &tmp, &count) != 1)
			break;

		if (s[count] == ':') {
			nid = tmp;
			if (nid < 0 || nid >= MAX_NUMNODES)
				break;

			s += count + 1;
			tmp = memparse(s, &s);
			hugetlb_cma_size_in_node[nid] = tmp;
			hugetlb_cma_size += tmp;

			/*
			 * Skip the separator if have one, otherwise
			 * break the parsing.
			 */
			if (*s == ',')
				s++;
			else
				break;
		} else {
			hugetlb_cma_size = memparse(p, &p);
			break;
		}
	}

	return 0;
}