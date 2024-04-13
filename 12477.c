static void on_scanner_error(pj_scanner *scanner)
{
    PJ_UNUSED_ARG(scanner);

    PJ_THROW(SYNTAX_ERROR);
}