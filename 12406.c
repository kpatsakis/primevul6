call_yank_do_autocmd(int regname)
{
    oparg_T	oa;
    yankreg_T	*reg;

    clear_oparg(&oa);
    oa.regname = regname;
    oa.op_type = OP_YANK;
    oa.is_VIsual = TRUE;
    reg = get_register(regname, TRUE);
    yank_do_autocmd(&oa, reg);
    free_register(reg);
}