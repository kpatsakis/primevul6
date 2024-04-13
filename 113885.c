i_free_name_entry(UChar* key, NameEntry* e, void* arg ARG_UNUSED)
{
  xfree(e->name);
  if (IS_NOT_NULL(e->back_refs)) xfree(e->back_refs);
  xfree(key);
  xfree(e);
  return ST_DELETE;
}