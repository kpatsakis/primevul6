decompileGETURL (SWF_ACTION *act)
{
	OUT_BEGIN(SWF_ACTIONGETURL);

	INDENT
	println("getUrl('%s',%s);", sact->UrlString, sact->TargetString);
}