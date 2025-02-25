int IsCompressedRTF(variableLength *p) {
  unsigned int in;
  BYTE *src;
  ULONG magic;

  if (p->size < 4)
    return 0;

  src = p->data;
  in = 0;

  in += 4;
  in += 4;
  magic = SwapDWord((BYTE*)src + in, 4);

  if (magic == 0x414c454d) {
    return 1;
  } else if (magic == 0x75465a4c) {
    return 1;
  } else {
    return 0;
  }
}