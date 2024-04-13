static int help(void) {
  fputs("PNG2WebP " VERSION "\n\
\n\
Usage:\n\
png2webp [-refv-] INFILE ...\n\
png2webp -p[refv-] [{INFILE|-} [OUTFILE|-]]\n\
\n\
-p: Work with a single file, allowing Piping from stdin or to stdout,\n\
    or using a different output filename to the input.\n\
    `INFILE` and `OUTFILE` default to stdin and stdout respectively,\n\
    or explicitly as \"-\".\n\
    Will show this message if stdin/stdout is used and is a terminal.\n\
-r: Convert from WebP to PNG instead.\n\
-e: Keep RGB data on pixels where alpha is 0. Always enabled for `-r`.\n\
-f: Force overwrite of output files (has no effect on stdout).\n\
-v: Be verbose.\n\
--: Explicitly stop parsing options.\n",
    stderr);
  return -1;
}