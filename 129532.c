compression_name(const int compression)
{
	static const int num_compression_methods =
		sizeof(compression_methods)/sizeof(compression_methods[0]);
	int i=0;

	while(compression >= 0 && i < num_compression_methods) {
		if (compression_methods[i].id == compression)
			return compression_methods[i].name;
		i++;
	}
	return "??";
}