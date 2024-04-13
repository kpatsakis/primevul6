int skip_eobrun( int* cmp, int* dpos, int* rstw, unsigned int* eobrun )
{
    if ( (*eobrun) > 0 ) // error check for eobrun
    {
        // compare rst wait counter if needed
        if ( rsti > 0 ) {
            if ( (int)(*eobrun) > (*rstw) )
                return -1;
            else
                (*rstw) -= (*eobrun);
        }

        // fix for non interleaved mcu - horizontal
        if ( cmpnfo[(*cmp)].bch != cmpnfo[(*cmp)].nch ) {
            (*dpos) += ( ( ( (*dpos) % cmpnfo[(*cmp)].bch ) + (*eobrun) ) /
                        cmpnfo[(*cmp)].nch ) * ( cmpnfo[(*cmp)].bch - cmpnfo[(*cmp)].nch );
        }

        // fix for non interleaved mcu - vertical
        if ( cmpnfo[(*cmp)].bcv != cmpnfo[(*cmp)].ncv ) {
            if ( (*dpos) / cmpnfo[(*cmp)].bch >= cmpnfo[(*cmp)].ncv )
                (*dpos) += ( cmpnfo[(*cmp)].bcv - cmpnfo[(*cmp)].ncv ) *
                        cmpnfo[(*cmp)].bch;
        }

        // skip blocks
        (*dpos) += (*eobrun);

        // reset eobrun
        (*eobrun) = 0;

        // check position
        if ( (*dpos) == cmpnfo[(*cmp)].bc ) return 2;
        else if ( (*dpos) > cmpnfo[(*cmp)].bc ) return -1;
        else if ( rsti > 0 )
            if ( (*rstw) == 0 ) return 1;
    }

    return 0;
}