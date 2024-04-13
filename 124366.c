decompileActions(int n, SWF_ACTION *actions, int indent)
{
	int i, svindent;

	svindent = gIndent;
	gIndent = indent;
	
	for(i=0;i<n;i++) {
		i+=decompileAction(i, actions, n);
	}
	gIndent = svindent;
}