int main(int argc, char **argv) {
  { // should optimize out
    uint32_t endian;
    memcpy(&endian, (char[4]){"\xAA\xBB\xCC\xDD"}, 4);
    E(endian == 0xAABBCCDD || endian == 0xDDCCBBAA,
      "32-bit mixed-endianness (%X) not supported", endian)
  }
  bool pipe = 0, usestdin = 0, usestdout = 0, reverse = 0;
#ifdef USEGETOPT
  for(int c; (c = getopt(argc, argv, ":prefv")) != -1;)
    switch(c)
#else
  while(--argc && **++argv == '-' && argv[0][1])
    while(*++*argv)
      switch(**argv)
#endif
    {
      case 'p': pipe = 1; break;
      case 'r': reverse = 1; break;
      case 'e': exact = 1; break;
      case 'f': force = 1; break;
      case 'v': verbose = 1; break;
#ifndef USEGETOPT
      case '-':
	if(argv[0][1]) return help();
	argc--;
	argv++;
	goto endflagloop;
#endif
      default: return help();
    }
#ifdef USEGETOPT
  argc -= optind;
  argv += optind;
#else
endflagloop:
#endif
#define URGC (unsigned)argc
#define PIPEARG(x) (*argv[x] == '-' && !argv[x][1])
  if(pipe) {
    if(URGC > 2 || ((usestdin = (!argc || PIPEARG(0))) && isatty(0)) ||
      ((usestdout = (URGC < 2 || PIPEARG(1))) && isatty(1)))
      return help();
    if(usestdin) setmode(0, O_BINARY);
    if(usestdout) setmode(1, O_BINARY);
    return (reverse ? w2p : p2w)(usestdin ? 0 : *argv, usestdout ? 0 : argv[1]);
  }
  if(!argc) return help();
  bool ret = 0;
  if(reverse)
    for(; argc; argc--, argv++) {
      size_t len = strlen(*argv);
      if(len > 4) {
	uint32_t ext, extmatch;
	memcpy(&ext, *argv + len - 4, 4);
	memcpy(&extmatch, (char[4]){"webp"}, 4);
	if(argv[0][len - 5] == '.' && (ext | 0x20202020) == extmatch) len -= 5;
      }
      {
#if defined __STDC_NO_VLA__ && !defined NOVLA
#define NOVLA
#endif
#ifdef NOVLA
	char *op = malloc(len + 5);
	E(op, "adding .%s extension to %s: Out of memory", "png", *argv)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
	char op[len + 5];
#pragma GCC diagnostic pop
#endif
	memcpy(op, *argv, len); // the only real memcpy
	memcpy(op + len, ".png", 5);
	ret = w2p(*argv, op) || ret;
#ifdef NOVLA
	free(op);
#endif
      }
    }
  else
    for(; argc; argc--, argv++) {
      size_t len = strlen(*argv);
      if(len > 3) {
	uint32_t ext, extmask, extmatch;
	memcpy(&ext, *argv + len - 4, 4);
	memcpy(&extmask, (char[4]){"\0   "}, 4);
	memcpy(&extmatch, (char[4]){".png"}, 4);
	if((ext | extmask) == extmatch) len -= 4;
      }
      {
#ifdef NOVLA
	char *op = malloc(len + 6);
	E(op, "adding .%s extension to %s: Out of memory", "webp", *argv)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
	char op[len + 6];
#pragma GCC diagnostic pop
#endif
	memcpy(op, *argv, len); // the only real memcpy
	memcpy(op + len, ".webp", 6);
	ret = p2w(*argv, op) || ret;
#ifdef NOVLA
	free(op);
#endif
      }
    }
  return ret;
}