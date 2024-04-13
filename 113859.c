get_name_end_code_point(OnigCodePoint start)
{
  switch (start) {
  case '<':  return (OnigCodePoint )'>'; break;
  case '\'': return (OnigCodePoint )'\''; break;
  default:
    break;
  }

  return (OnigCodePoint )0;
}