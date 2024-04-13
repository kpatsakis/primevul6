int mg_base64_update(unsigned char ch, char *to, int n) {
  int rem = (n & 3) % 3;
  if (rem == 0) {
    to[n] = (char) mg_b64idx(ch >> 2);
    to[++n] = (char) ((ch & 3) << 4);
  } else if (rem == 1) {
    to[n] = (char) mg_b64idx(to[n] | (ch >> 4));
    to[++n] = (char) ((ch & 15) << 2);
  } else {
    to[n] = (char) mg_b64idx(to[n] | (ch >> 6));
    to[++n] = (char) mg_b64idx(ch & 63);
    n++;
  }
  return n;
}