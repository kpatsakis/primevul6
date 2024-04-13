readconf_printtime(int t)
{
int s, m, h, d, w;
uschar *p = time_buffer;

if (t < 0)
  {
  *p++ = '-';
  t = -t;
  }

s = t % 60;
t /= 60;
m = t % 60;
t /= 60;
h = t % 24;
t /= 24;
d = t % 7;
w = t/7;

if (w > 0) { sprintf(CS p, "%dw", w); while (*p) p++; }
if (d > 0) { sprintf(CS p, "%dd", d); while (*p) p++; }
if (h > 0) { sprintf(CS p, "%dh", h); while (*p) p++; }
if (m > 0) { sprintf(CS p, "%dm", m); while (*p) p++; }
if (s > 0 || p == time_buffer) sprintf(CS p, "%ds", s);

return time_buffer;
}