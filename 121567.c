int next_mcuposn( int* cmp, int* dpos, int* rstw )
{
    // increment position
    (*dpos)++;

    // fix for non interleaved mcu - horizontal
    if ( cmpnfo[(*cmp)].bch != cmpnfo[(*cmp)].nch ) {
        if ( (*dpos) % cmpnfo[(*cmp)].bch == cmpnfo[(*cmp)].nch )
            (*dpos) += ( cmpnfo[(*cmp)].bch - cmpnfo[(*cmp)].nch );
    }

    // fix for non interleaved mcu - vertical
    if ( cmpnfo[(*cmp)].bcv != cmpnfo[(*cmp)].ncv ) {
        if ( (*dpos) / cmpnfo[(*cmp)].bch == cmpnfo[(*cmp)].ncv )
            (*dpos) = cmpnfo[(*cmp)].bc;
    }

    // check position
    if ( (*dpos) >= cmpnfo[(*cmp)].bc ) return 2;
    else if ( rsti > 0 )
        if ( --(*rstw) == 0 ) return 1;


    return 0;
}