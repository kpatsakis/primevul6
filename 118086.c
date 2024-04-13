assign_warnings(graph_chars)
register uchar *graph_chars;
{
	int i;
	for (i = 0; i < WARNCOUNT; i++)
	    if (graph_chars[i]) warnsyms[i] = graph_chars[i];
}