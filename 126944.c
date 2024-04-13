static int sqlite_callback(void *data, int argc,
			   char **argv, char **azColName) {
  struct ntopng_sqlite_state *s = (struct ntopng_sqlite_state*)data;

  lua_newtable(s->vm);

  for(int i=0; i<argc; i++)
    lua_push_str_table_entry(s->vm, (const char*)azColName[i],
			     (char*)(argv[i] ? argv[i] : "NULL"));

  lua_pushinteger(s->vm, ++s->num_rows);
  lua_insert(s->vm, -2);
  lua_settable(s->vm, -3);

  return(0);
}