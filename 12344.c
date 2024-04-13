prep_redo_cmd(cmdarg_T *cap)
{
    prep_redo(cap->oap->regname, cap->count0,
				     NUL, cap->cmdchar, NUL, NUL, cap->nchar);
}