clearopbeep(oparg_T *oap)
{
    clearop(oap);
    beep_flush();
}