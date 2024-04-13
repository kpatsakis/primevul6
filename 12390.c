do_nv_ident(int c1, int c2)
{
    oparg_T	oa;
    cmdarg_T	ca;

    clear_oparg(&oa);
    CLEAR_FIELD(ca);
    ca.oap = &oa;
    ca.cmdchar = c1;
    ca.nchar = c2;
    nv_ident(&ca);
}