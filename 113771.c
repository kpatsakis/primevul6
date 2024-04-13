bool CheckCommandLineArguments(int argc, base::CommandLine::CharType** argv) {
  DCHECK(std::is_sorted(std::begin(kBlacklist), std::end(kBlacklist),
                        [](const char* a, const char* b) {
                          return base::StringPiece(a) < base::StringPiece(b);
                        }))
      << "The kBlacklist must be in sorted order";
  DCHECK_NE(std::find(std::begin(kBlacklist), std::end(kBlacklist),
                      base::StringPiece("inspect")),
            std::end(kBlacklist))
      << "Do not forget to add Node command line flags to kBlacklist";

  const base::CommandLine::StringType dashdash(2, '-');
  bool block_blacklisted_args = false;
  for (int i = 0; i < argc; ++i) {
    if (argv[i] == dashdash)
      break;
    if (block_blacklisted_args) {
      if (IsBlacklistedArg(argv[i]))
        return false;
    } else if (IsUrlArg(argv[i])) {
      block_blacklisted_args = true;
    }
  }
  return true;
}