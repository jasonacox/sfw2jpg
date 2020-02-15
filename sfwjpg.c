/*
 * sfwjpg.c
 *
 * Copyright (c) 1997-1999  Everett Lipman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * The GNU General Public License is available at
 *
 *    http://www.fsf.org/copyleft/gpl.html
 *
 * Alternatively, you can write to the 
 *
 *    Free Software Foundation, Inc.
 *    59 Temple Place - Suite 330
 *    Boston, MA  02111-1307, USA
 *
 *
 * Revision History:
 *
 *     22 Sep 1997  EAL Completed first version.
 *      9 Jan 1999  EAL Changed to forward scanning for the end of
 *                      JFIF data, as suggested by Bo Lindbergh.
 *      9 May 1999  EAL Unknown markers now result in warning, not
 *                      failure.  Unknown markers will be converted
 *                      to COM (0xfe) marker.  Added code to deal with
 *                      SFW DHT marker (0xa4).  Added output options
 *                      suggested by John Oppenheimer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define USCH unsigned char
#define HUFFSIZE 420

/*** 420 characters ***/

USCH hufftbl[] = {
0xFF,0xC4,0x01,0xA2,0x00,0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,
0x0B,0x01,0x00,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x10,0x00,
0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D,0x01,
0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,
0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,0x24,
0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,0x29,
0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,
0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,
0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,
0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,
0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,
0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,0xE3,
0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,
0xFA,0x11,0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,
0x02,0x77,0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,
0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,0x33,
0x52,0xF0,0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,
0x1A,0x26,0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,
0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,
0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,
0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,
0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,
0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,
0xD9,0xDA,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,
0xF7,0xF8,0xF9,0xFA
};

USCH markertbl[256];

int sfw_to_jfif(USCH *sfwstart, USCH *sfwend, char *filename);
USCH *forward_scan(USCH *start, USCH *stop, USCH *goal, int length);
int fix_marker(USCH *marker);
long read_skip_length(USCH *marker);

int main(int argc, char **argv)
{
   struct stat filestat;
           int retval;
        size_t sretval, namelen;
          char mesg[256];
          char infilename[256];
          char outfilename[256];
          USCH *filebuf;
          FILE *infile;

if ((argc == 1) || (argc > 3))
   {
   fprintf(stderr,"\nusage: sfwjpg inputfile [-|outputfile]\n\n");
   fprintf(stderr,"where inputfile is the .sfw file which\n");
   fprintf(stderr,"will be converted to .jpg (JFIF) format,\n");
   fprintf(stderr,"and outputfile is the name of the resulting\n");
   fprintf(stderr,"file.  - specifies standard output.\n");
   fprintf(stderr,"If no output file is specified, the .sfw in\n");
   fprintf(stderr,"the input file will be replaced with .jpg, and\n");
   fprintf(stderr,"the resulting filename will be used.\n\n");
   exit(1);
   }

/*** make input and output filenames *********************************/

strncpy(infilename,argv[1],255);
namelen = strlen(infilename);
if ( strcasecmp(infilename+(namelen-4), ".sfw") != 0 )
   {
   strcat(infilename, ".sfw");
   namelen += 4;
   }

if (argc == 3)
   {
   if (strcasecmp(argv[2],"-") == 0)
      outfilename[0] = '\0';
   else
      strncpy(outfilename,argv[2],255);
   }
else
   {
   strcpy(outfilename,infilename);
   strcpy(outfilename+(namelen-4),".jpg");
   }

/*** read in .sfw file ***********************************************/

retval = stat(infilename,&filestat);
if (retval == -1)
   {
   sprintf(mesg,"Error getting status for file '%s'",infilename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   exit(1);
   }

filebuf = (USCH *) malloc( (size_t) filestat.st_size);
if (filebuf == NULL)
   {
   fprintf(stderr,"\n");
   perror("Error allocating memory for filebuf");
   fprintf(stderr,"\n");
   exit(1);
   }

infile = fopen(infilename,"rb");
if (infile == NULL)
   {
   sprintf(mesg,"Error opening file '%s'",infilename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   exit(1);
   }

sretval = fread(filebuf, filestat.st_size, 1, infile);
if (sretval == 0)
   {
   sprintf(mesg,"Error reading file '%s'",infilename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   exit(1);
   }
fclose(infile);

retval = sfw_to_jfif(filebuf, filebuf+filestat.st_size-1, outfilename);

if (retval != 0)
   {
   fprintf(stderr, "Conversion to %s failed.  Exiting.\n\n",outfilename);
   exit(1);
   }

free( (void *) filebuf );
exit(0);
}
/***********************************************************************/
/***********************************************************************/

int sfw_to_jfif(USCH *sfwstart, USCH *sfwend, char *filename)
{
   int i, retval, dataflag = 0, huffmanflag = 0;
size_t sretval;
  char mesg[256];
  USCH scanbuf[256];
  USCH *bufpos, *headerstart, *headerend, *dataend;
  FILE *outfile;

/*** Initialize lookup table for marker conversions ***/

for (i=0;i<256;i++) markertbl[i] = 0x00;

markertbl[0xa0] = 0xc0;   /* SOF0 */
markertbl[0xa4] = 0xc4;   /* DHT  */
markertbl[0xc8] = 0xd8;   /* SOI  */
markertbl[0xc9] = 0xd9;   /* EOI  */
markertbl[0xca] = 0xda;   /* SOS  */
markertbl[0xcb] = 0xdb;   /* DQT  */
markertbl[0xd0] = 0xe0;   /* APP0 */

/*** Scan for start of JFIF data ***/

scanbuf[0] = 0xff;
scanbuf[1] = 0xc8;
scanbuf[2] = 0xff;
scanbuf[3] = 0xd0;
headerstart = forward_scan(sfwstart, sfwend, scanbuf, 4);
if (headerstart == NULL) return(-1);

/*** fix SOI and APP0 tags ***/

retval = fix_marker(headerstart);
if (retval == -1) return(-1);
retval = fix_marker(headerstart+2);
if (retval == -1) return(-1);

/*** fix identifier and version number           ***/
/*** place string "JFIF\0\001\0" in proper place ***/

headerstart[6]  = 0x4a;
headerstart[7]  = 0x46;
headerstart[8]  = 0x49;
headerstart[9]  = 0x46;
headerstart[10] = 0x00;
headerstart[11] = 0x01;
headerstart[12] = 0x00;

/*** set bufpos to start of next marker ***/

bufpos = headerstart + 2;
bufpos += read_skip_length(bufpos); 

/*** fix remaining markers ***/

while (!dataflag)
   {
   retval = fix_marker(bufpos);
   if (retval == -1) return(-1);
   if (retval == (int)0xc4) huffmanflag = 1;
   if (bufpos[1] == 0xda)
      dataflag = 1;
   else
      bufpos += read_skip_length(bufpos);
   }
headerend = bufpos-1;

/*** scan forward for EOI marker ***/

scanbuf[0] = 0xff;
scanbuf[1] = 0xc9;
dataend = forward_scan(bufpos, sfwend, scanbuf, 2);
if (dataend == NULL) return(-1);

/*** fix EOI marker ***/

retval = fix_marker(dataend);
if (retval == -1) return(-1);
dataend ++;

/*** open output file ***/

if (filename[0] == '\0') 
   outfile = stdout;
else
   outfile = fopen(filename,"wb");

if (outfile == NULL)
   {
   if (filename[0] == '\0') strcpy(filename, "standard output");
   sprintf(mesg,"Error opening file '%s'",filename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   return(-1);
   }

/*** write jfif file header ***/

sretval = fwrite(headerstart, (headerend-headerstart)+1, 1, outfile);
if (sretval == 0)
   {
   sprintf(mesg,"Error writing file '%s'",filename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   return(-1);
   }

/*** write Huffman table if it is not already there ***/

if (!huffmanflag)
   {
   sretval = fwrite(hufftbl, HUFFSIZE, 1, outfile);
   if (sretval == 0)
      {
      sprintf(mesg,"Error writing file '%s'",filename);
      fprintf(stderr,"\n");
      perror(mesg);
      fprintf(stderr,"\n");
      return(-1);
      }
   }

/*** write rest of jfif file ***/

sretval = fwrite(headerend+1, dataend-headerend, 1, outfile);
if (sretval == 0)
   {
   sprintf(mesg,"Error writing file '%s'",filename);
   fprintf(stderr,"\n");
   perror(mesg);
   fprintf(stderr,"\n");
   return(-1);
   }

fclose(outfile);
return(0);
}
/***********************************************************************/

USCH *forward_scan(USCH *start, USCH *stop, USCH *goal, int length)
{
USCH *i;
int j, flag;

for (i=start; i<stop; i++)
   {
   if (*i != *goal) continue;
   else
      {
      if (length == 1) return(i);
      else
         {
         flag = 1;
         for (j=1; j<length; j++)
            if ( *(i+j) != *(goal+j) )
               {
               flag = 0;
               break;
               }
         if (flag) return(i);
         }
      }
   }
fprintf(stderr,"forward_scan() failed.\n\n");
return(NULL);
}
/***********************************************************************/

int fix_marker(USCH *marker)
{
if (marker[0] != 0xFF)
   {
   fprintf(stderr, "fix_marker(): Marker must begin with 0xFF. 0x%02x 0x%02x\n"
                    , marker[0], marker[1]);
   return(-1);
   }
if (markertbl[marker[1]] == 0)
   {
   fprintf(stderr, "\nWARNING: Unknown marker 0x%02x changed to comment.\n\n",
           marker[1]);
   marker[1] = 0xfe;   /* 0xfe is the comment marker */
   }
else
   marker[1] = markertbl[marker[1]];
return((int)marker[1]);
}
/***********************************************************************/

long read_skip_length(USCH *marker)
{
long msb,lsb, retval;

msb = (long) marker[2];
lsb = (long) marker[3];

retval = 256*msb + lsb + 2;
return( retval );
}
/***********************************************************************/
