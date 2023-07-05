# pnmtodjvurle

DjVu Color RLE.

## Build

Type:
```shell
make
```

## Example

```shell
tifftopnm foreground.tif | pnmtodjvurle > foreground.rle
tifftopnm background.tif > background.ppm
cat foreground.rle background.ppm > output.sep
csepdjvu output.sep demo.djvu
rm foreground.rle background.ppm output.sep
```
or
```shell
tifftopnm foreground.tif > foreground.pnm
tifftopnm background.tif > background.ppm
pnmtodjvurle foreground.pnm foreground.rle
cat foreground.rle background.ppm > output.sep
csepdjvu output.sep demo.djvu
rm foreground.rle foreground.pnm background.ppm output.sep
```

## Copyright

Autors: Arcand, manfred.  
This is free and unencumbered software released into the public domain.
