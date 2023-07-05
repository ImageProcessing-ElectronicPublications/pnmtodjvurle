
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAXINT 32767

typedef unsigned int word;
typedef unsigned long dword;
typedef unsigned char byte;
typedef unsigned char gray;

struct rgb
{
    byte r, g, b;
};

typedef struct
{
    word width, height;
} pnm_image_t;

extern pnm_image_t pnm_image;
extern struct rgb palette[4096];
extern word palLength;

extern FILE* tempfile, * inputfile, * outputfile;

extern void ABORT(char* msg);

extern void pbmtodjvurle(void);
extern void pgmtodjvurle(void);
extern void ppmtodjvurle(void);

