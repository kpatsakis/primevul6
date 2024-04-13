readconf_readname(uschar *name, int len, uschar *s)
{
int p = 0;
while (isspace(*s)) s++;
if (isalpha(*s))
  {
  while (isalnum(*s) || *s == '_')
    {
    if (p < len-1) name[p++] = *s;
    s++;
    }
  }
name[p] = 0;
while (isspace(*s)) s++;
return s;
}