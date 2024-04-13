i_renumber_name(UChar* key ARG_UNUSED, NameEntry* e, GroupNumRemap* map)
{
  int i;

  if (e->back_num > 1) {
    for (i = 0; i < e->back_num; i++) {
      e->back_refs[i] = map[e->back_refs[i]].new_val;
    }
  }
  else if (e->back_num == 1) {
    e->back_ref1 = map[e->back_ref1].new_val;
  }

  return ST_CONTINUE;
}