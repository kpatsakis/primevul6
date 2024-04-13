bool HHVM_FUNCTION(mb_ereg_search_setpos,
                   int position) {
  if (position < 0 || position >= (int)MBSTRG(search_str).size()) {
    raise_warning("Position is out of range");
    MBSTRG(search_pos) = 0;
    return false;
  }
  MBSTRG(search_pos) = position;
  return true;
}