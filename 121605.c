void show_help( void )
{
    fprintf(msgout, "Usage: %s [switches] input_file [output_file]", appname );
    fprintf(msgout, "\n" );
    fprintf(msgout, "\n" );
    fprintf(msgout, " [-version]       File format version of lepton codec\n" );
    fprintf(msgout, " [-revision]      GIT Hash of lepton source that built this binary\n");
    fprintf(msgout, " [-zlib0]         Instead of a jpg, return a zlib-compressed jpeg\n");
    fprintf(msgout, " [-startbyte=<n>] Encoded file will only contain data at and after <n>\n");
    fprintf(msgout, " [-trunc=<n>]     Encoded file will be truncated at size <n> - startbyte\n");
//    fprintf(msgout, " [-avx2upgrade]   Try to exec <binaryname>-avx if avx is available\n");
//    fprintf(msgout, " [-injectsyscall={1..4}]  Inject a \"chdir\" syscall & check SECCOMP crashes\n");
    fprintf(msgout, " [-unjailed]      Do not jail this process (use only with trusted data)\n" );
    fprintf(msgout, " [-singlethread]  Do not clone threads to operate on the input file\n" );
    fprintf(msgout, " [-maxencodethreads=<n>] Can use <n> threads to decode: higher=bigger file\n");
    fprintf(msgout, " [-allowprogressive] Allow progressive jpegs through the compressor\n");
    fprintf(msgout, " [-rejectprogressive] Reject encoding of progressive jpegs\n");
    fprintf(msgout, " [-timebound=<>ms]For -socket, enforce a timeout since first byte received\n");
    fprintf(msgout, " [-lepcat] Concatenate lepton files together into a file that contains multiple substrings\n");
    fprintf(msgout, " [-memory=<>M]    Upper bound on the amount of memory allocated by main\n");
    fprintf(msgout, " [-threadmemory=<>M] Bound on the amount of memory allocated by threads\n");
    fprintf(msgout, " [-recodememory=<>M] Check that a singlethreaded recode only uses <>M mem\n");
#ifndef _WIN32
    fprintf(msgout, " [-hugepages]     Allocate from the hugepages on the system\n");
    fprintf(msgout, " [-socket=<name>] Serve requests on a Unix Domain Socket at path <name>\n" );
    fprintf(msgout, " [-listen=<port>] Serve requests on a TCP socket on <port> (default 2402)\n" );
    fprintf(msgout, " [-listenbacklog=<n>] n clients queued for encoding if maxchildren reached\n" );
    fprintf(msgout, " [-zliblisten=<port>] Serve requests on a TCP socket on <port> (def 2403)\n" );
    fprintf(msgout, " [-maxchildren]   Max codes to ever spawn at the same time in socket mode\n");
#endif
    fprintf(msgout, " [-benchmark]     Run a benchmark on optional [<input_file>] (or included file)\n");
    fprintf(msgout, " [-verbose]       Run the benchmark in verbose mode (more output to stderr)\n");
    fprintf(msgout, " [-benchreps=<n>] Number of trials to run the benchmark for each category\n");
    fprintf(msgout, " [-benchthreads=<n>] Max number of parallel codings to benchmark\n");
#ifdef SKIP_VALIDATION
    fprintf(msgout, " [-validate]      Round-trip this file when encoding [default:off]\n");
#else
    fprintf(msgout, " [-validate]      Round-trip this file when encoding [default:on]\n");
    fprintf(msgout, " [-skipvalidate]  Avoid round-trip check when encoding (Warning: unsafe)\n");
#endif
}