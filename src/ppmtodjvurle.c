
#include "pnmtodjvurle.h"

static struct rgb transparentColor;

static int inline cmpColors(struct rgb a, struct rgb b)
{
    return (a.r == b.r && a.g == b.g && a.b == b.b);
}

static inline void writeLong(FILE* const file, long const l)
{
    if (putc((l >> 24) & 0xff, file) == EOF ||
            putc((l >> 16) & 0xff, file) == EOF ||
            putc((l >> 8) & 0xff, file) == EOF ||
            putc(l & 0xff, file) == EOF)
        ABORT((char*)"failed to write to the RLE file");
}

static int getPalIndex(struct rgb color)
{
    word i;

    for (i = 0; i < palLength; i++)
        if (palette[i].r == color.r && palette[i].g == color.g && palette[i].b == color.b)
            return i;
    palette[palLength++] = color;
    if (palLength >= 0xFF0)
        ABORT((char*)"getPalIndex: too many colors in input file");
    // color index > 0FF0 reserved
    return i;
}

static void writeRun(struct rgb color, dword len)
{
    dword rleValue;

    if (len == 0) return;
    if (len >= ((dword)1 << 20))
        ABORT((char*)"writeRun: too long rleLength");
    if (cmpColors(color, transparentColor))
        rleValue = 0xFFF;
    else
        rleValue = getPalIndex(color);
    rleValue = (rleValue << 20) | len;
    writeLong(tempfile, rleValue);
}

static void processFile(void)
{
    dword r, i, runLength;
    struct rgb curr, prev;

    for (r = 0; r < pnm_image.height; r++)
    {
        runLength = 1;
        if (fread(&prev, sizeof(struct rgb), 1, inputfile) !=1)
            ABORT((char*)"EOF / read error reading a one-rgb sample");
        // if ret!=1 EoF_error
        for (i = 1; i < pnm_image.width; i++)
        {
            if (fread(&curr, sizeof(struct rgb), 1, inputfile) !=1)
                ABORT((char*)"EOF / read error reading a one-rgb sample");
            // if ret!=1 EoF_error
            if (cmpColors(prev, curr))
            {
                runLength++;
                if (runLength >= ((unsigned long)(1 << 20) - 1))
                {
                    writeRun(prev, runLength);
                    runLength = 1;
                }
            }
            else
            {
                writeRun(prev, runLength);
                prev = curr;
                runLength = 1;
            }
        } // for i
        writeRun(prev, runLength);
    }
}

void ppmtodjvurle(void)
{
    int lenr, lenw;

    transparentColor.r = transparentColor.g = transparentColor.b = 0xFF;
    palLength=0;
    tempfile = tmpfile();
    processFile();

    // write header "R6", rows, cols, palettesize
    fprintf(outputfile, "R6\r%d %d\r%d\r", pnm_image.width, pnm_image.height, palLength);
    // write palette
    fwrite(palette, sizeof(struct rgb), palLength, outputfile);

    // append tempfile
    rewind(tempfile);
    while (1)
    {
        if ((lenr = fread(&palette, 1, sizeof(palette), tempfile))==0) break;
        lenw = fwrite(&palette, 1, lenr, outputfile);
        if (lenr != lenw) ABORT((char*)"Merge file error: (disk full?)");
    }
    fclose(tempfile);
}
