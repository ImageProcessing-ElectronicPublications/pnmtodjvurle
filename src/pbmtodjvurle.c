
#include "pnmtodjvurle.h"
#include <limits.h>

#define PBM_WHITE 0
#define PBM_BLACK 1

/* Write a byte to a file and check for errors. */

static inline void writebyte(FILE* const file, byte const c)
{
    if (fputc (c, file) == EOF)
        ABORT((char*)"failed to write to the RLE file");
}

/* Write a run length to the RLE file. */

static void write_rle (FILE* file, dword tally)
{
    do
    {
        /* Output a single run. */
        if (tally < 192)
        {
            /* Single-byte runs */
            writebyte(file, tally);
            tally >>= 8;
        }
        else
        {
            /* Two-byte runs */
            writebyte(file, ((tally>>8)&0x3F) + 0xC0);
            writebyte(file, tally&0xFF);
            tally >>= 14;
        }

        /* Very large runs need to be split into smaller runs.  We
         * therefore need to toggle back to the same color we had for the
         * previous smaller run. */
        if (tally > 0)
            writebyte(file, 0);
    }
    while (tally > 0);
}

static byte pbm_getrawbyte(FILE* const file)
{
    int iby;

    iby = getc(file);
    if (iby == EOF)
        ABORT((char*)"EOF / read error reading a one-byte sample");
    return (byte) iby;
}

static void pbm_readpbmrow(FILE* file, byte* bitrow, word cols)
{
    register word col, bitshift, bitshiftflag;
    register byte* bP;
    register byte item;

    bitshift = -1;
    bitshiftflag = bitshift;
    item = 0;  /* item's value is meaningless here */
    for ( col = 0, bP = bitrow; col < cols; ++col, ++bP )
    {
        if ( bitshift == bitshiftflag )
        {
            item = pbm_getrawbyte( file );
            bitshift = 7;
        }
        *bP = ( item >> bitshift ) & 1;
        --bitshift;
    }
}

static byte* pbm_allocrow(word cols)
{
    word size = sizeof(byte);
    void* itrow;

    if (UINT_MAX / cols < size)
        ABORT((char*)"Arithmetic overflow multiplying to get the "
              "size of a row to allocate.");
    itrow = malloc(cols * size);
    if (itrow == NULL)
        ABORT((char*)"out of memory allocating a row");
    return (byte*)itrow;
}

static void pbm_freerow(byte* const itrow)
{
    free(itrow);
}

void pbmtodjvurle(void)
{
    byte* pbmrow;               /* One row of the PBM file */
    dword pixeltally;           /* Run length of the current color */
    word row, col;              /* Row and column loop variables */

    /* Write an RLE header. */
    fprintf(outputfile, "R4\n");
    fprintf(outputfile, "%d %d\n", pnm_image.width, pnm_image.height);

    /* Write the RLE data. */
    pbmrow = pbm_allocrow (pnm_image.width);
    for (row = 0; row < pnm_image.height; row++)
    {
        byte prevpixel;        /* Previous pixel seen */

        pbm_readpbmrow (inputfile, pbmrow, pnm_image.width);
        prevpixel = PBM_WHITE;   /* Bitonal RLE rows always start with white */
        pixeltally = 0;
        for (col = 0; col < pnm_image.width; col++)
        {
            byte newpixel = pbmrow[col];      /* Current pixel color */
            if (newpixel == prevpixel)
                pixeltally++;
            else
            {
                write_rle (outputfile, pixeltally);
                pixeltally = 1;
                prevpixel = newpixel;
            }
        }
        write_rle(outputfile, pixeltally);
    }
    /* Finish up cleanly. */
    pbm_freerow (pbmrow);
}
