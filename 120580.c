
void tsro_del(GF_Box *s)
{
	GF_TimeOffHintEntryBox *tsro = (GF_TimeOffHintEntryBox *)s;
	gf_free(tsro);