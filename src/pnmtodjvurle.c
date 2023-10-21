#include "pnmtodjvurle.h"

static char PNM;
struct rgb palette[4096];
word palLength;
pnm_image_t pnm_image;

FILE* tempfile, * inputfile, * outputfile;

void ABORT(char* msg)
{
    fputs(msg, stderr);
    exit(1);
}

static void skip_blank(void)
{
    char ch;

    ch = fgetc(inputfile);
    if (ch == '#')
        while((ch = fgetc(inputfile)) != '\n' && ch != '\r');
    if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t')
        ABORT((char*)"skip_blank: Invalid pnm header");
    do
        ch = fgetc(inputfile);
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    ungetc(ch, inputfile);
}

static int readInt(void)
{
    char ch;
    int num = 0;

    ch = fgetc(inputfile);
    if (ch=='#')
    {
        do
            ch= fgetc(inputfile);
        while (ch != '\r' &&ch != '\n');
        ch = fgetc(inputfile);
    }
    if (ch < '0' || ch > '9')
        ABORT((char*)"readInt: Invalid pnm header");
    do
    {
        if (num > MAXINT/10-10)
            ABORT((char*)"readInt: Invalid pnm header");
        num = num*10 + (ch-'0');
        ch = fgetc(inputfile);
    }
    while (ch >= '0' && ch <= '9');
    ungetc(ch, inputfile);
    return(num);
}

static void read_pnmHeader(void)
{

    if (fgetc(inputfile)!='P')
        ABORT((char*)"Invalid pnm header: signature P4, P5 or P6 expected");

    switch ( PNM=fgetc(inputfile) )
    {
    case '4':
        ;
    case '5':
        ;
    case '6':
        break;
    default:
        ABORT((char*)"Invalid pnm header: signature P4, P5 or P6 expected");
    };
    skip_blank();
    pnm_image.width = readInt();
    skip_blank();
    pnm_image.height = readInt();
    if (PNM == '5' || PNM == '6')
    {
        skip_blank();
        if( readInt() != 255)
            ABORT((char*)"Invalid pnm header: invalid maxColor");
    }
    fgetc(inputfile);
}

int main(int argc, char *argv[])
{
    inputfile  = (argc > 1) ? fopen(argv[1], "rb") : stdin;
    outputfile = (argc > 2) ? fopen(argv[2], "wb") : stdout;
    if (!inputfile)
        ABORT((char*)"Invalid input file.");
    if (!outputfile)
        ABORT((char*)"Invalid output file.");
    read_pnmHeader();
    switch (PNM)
    {
    case '4':
        pbmtodjvurle();
        break;
    case '5':
        pgmtodjvurle();
        break;
    case '6':
        ppmtodjvurle();
        break;
    };
    if (argc > 1)
        fclose(inputfile);
    if (argc > 2)
        fclose(outputfile);
}
