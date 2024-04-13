void execute(const std::function<bool()> &function)
{
    clock_t begin = 0, end = 0;
    bool success;



    if ( errorlevel.load() < err_tresh )
    {
        // get statusmessage
        //function();
        // write statusmessage
        // set starttime
        if (!g_use_seccomp) {
            begin = clock();
        }
        // call function
        success = function();
        // set endtime
        if (!g_use_seccomp) {
            end = clock();
        }

        // write statusmessage
        if ( success ) {
            if (verbosity == 2 && !g_use_seccomp) {
                fprintf( msgout,  "%6ims",
                         (int) ( (double) (( end - begin ) * 1000) / CLOCKS_PER_SEC ) );
            }
        }
        else {
            if ( verbosity == 2 ) {
                while(write(2, "ERROR\n", strlen("ERROR\n")) < 0 && errno == EINTR) {

                }
            }
        }
    }
}