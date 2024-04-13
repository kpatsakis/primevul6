unsigned CLASS pana_bits(int nbits)
{
#ifndef LIBRAW_NOTHREADS
#define buf tls->pana_bits.buf
#define vbits tls->pana_bits.vbits
#else
  static uchar buf[0x4000];
  static int vbits;
#endif
  int byte;

  if (!nbits)
    return vbits = 0;
  if (!vbits)
  {
    fread(buf + load_flags, 1, 0x4000 - load_flags, ifp);
    fread(buf, 1, load_flags, ifp);
  }
  vbits = (vbits - nbits) & 0x1ffff;
  byte = vbits >> 3 ^ 0x3ff0;
  return (buf[byte] | buf[byte + 1] << 8) >> (vbits & 7) & ~((~0u) << nbits);
#ifndef LIBRAW_NOTHREADS
#undef buf
#undef vbits
#endif
}