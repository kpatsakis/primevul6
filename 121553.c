void compute_thread_mem(const char * arg,
                        size_t * mem_init,
                        size_t * thread_mem_init,
                        bool *needs_huge_pages,
                        bool *avx2upgrade) {
    if (strcmp(arg, "-hugepages") == 0) {
        *needs_huge_pages = true;
    }
    if ( strcmp(arg, "-avx2upgrade") == 0) {
        *avx2upgrade = true;
    }
    if (strstr(arg, "-help")) {
        show_help();
        exit(0);
    }
    if (strcmp(arg, "-h") == 0) {
        show_help();
        exit(0);
    }
    const char mem_arg_name[]="-memory=";
    const char thread_mem_arg_name[]="-threadmemory=";
    if (starts_with(arg, mem_arg_name)) {
        arg += strlen(mem_arg_name);
        *mem_init = local_atoi(arg);
    }
    if (starts_with(arg, thread_mem_arg_name)) {
        arg += strlen(thread_mem_arg_name);
        *thread_mem_init = local_atoi(arg);
    }
}