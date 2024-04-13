node_str_head_pad(StrNode* sn, int num, UChar val)
{
  UChar buf[NODE_STR_BUF_SIZE];
  int i, len;

  len = sn->end - sn->s;
  onig_strcpy(buf, sn->s, sn->end);
  onig_strcpy(&(sn->s[num]), buf, buf + len);
  sn->end += num;

  for (i = 0; i < num; i++) {
    sn->s[i] = val;
  }
}