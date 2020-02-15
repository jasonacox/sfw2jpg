/*
 * flipjpeg.c
 *
 * Copyright (c) 1997  Everett Lipman
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
 * ---------------------------------------------------------------
 *
 * This program will do a lossless vertical inversion of a jpeg 
 * file.  It will only work properly if the jpeg image has a height
 * in pixels which is a multiple of 8.
 * 
 * This program requires the Independent JPEG Group's free JPEG software.
 * You can get a copy at
 *
 *   ftp://ftp.uu.net/graphics/jpeg
 * 
 * Most of the code here was taken from the program jpegtran.c, which
 * is part of the IJG software package.  Since I have little experience
 * using the IJG package, I am not sure if this program will run on any
 * platform other than Sun Solaris (UNIX).
 *
 */

#include <stdio.h>
#include <string.h>
#include "cdjpeg.h"       /* Common decls for cjpeg/djpeg applications */
#include "jversion.h"     /* for version message */

static const char * progname;  /* program name for error messages */

LOCAL(void)
usage (void)
/*
 * complain about bad command line
 */
{
  fprintf(stderr, "\nusage: flipjpeg < inputfile > outputfile\n\n");
  exit(EXIT_FAILURE);
}


/*
 * The main program.
 */
int
main (int argc, char **argv)
{
  struct jpeg_decompress_struct srcinfo;
  struct jpeg_compress_struct dstinfo;
  struct jpeg_error_mgr jsrcerr, jdsterr;
  jvirt_barray_ptr *coef_arrays;
  JDIMENSION i, compnum, rownum, blocknum;
  size_t block_row_size;
  JBLOCKARRAY coef_buffers[MAX_COMPONENTS];
  JBLOCKARRAY row_ptrs[MAX_COMPONENTS];
  FILE * input_file;
  FILE * output_file;

  if (argc != 1) usage();

  progname = argv[0];
  if (progname == NULL || progname[0] == 0)
    progname = "flipjpeg";  /* in case C library doesn't provide it */

  /* 
   * Initialize the JPEG decompression object with default error handling.
   */

  srcinfo.err = jpeg_std_error(&jsrcerr);
  jpeg_create_decompress(&srcinfo);

  /*
   * Initialize the JPEG compression object with default error handling.
   */

  dstinfo.err = jpeg_std_error(&jdsterr);
  jpeg_create_compress(&dstinfo);

  jsrcerr.trace_level = jdsterr.trace_level;
  srcinfo.mem->max_memory_to_use = dstinfo.mem->max_memory_to_use;

  /*
   * Use stdin and stdout
   */

  input_file = read_stdin();
  output_file = write_stdout();
  
  /*
   * Specify data source for decompression
   */

  jpeg_stdio_src(&srcinfo, input_file);

  /*
   * Read file header
   */

  (void) jpeg_read_header(&srcinfo, TRUE);

  /*
   * Allocate memory to hold entire coefficient array for all components
   */

  for (compnum=0; compnum<srcinfo.num_components; compnum++)
    coef_buffers[compnum] = ((&dstinfo)->mem->alloc_barray) 
                            ((j_common_ptr) &dstinfo, JPOOL_IMAGE,
                             srcinfo.comp_info[compnum].width_in_blocks,
                             srcinfo.comp_info[compnum].height_in_blocks);

  /*
   * Read source file as DCT coefficients
   */

  coef_arrays = jpeg_read_coefficients(&srcinfo);

  /*
   * Initialize destination compression parameters from source values
   */

  jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

/***************************************************************************/

  /*
   * print out some info about the image
   */
/*
 * fprintf(stderr,"\ncomponents: %d  image width: %d  image height: %d\n\n",
 *         srcinfo.num_components, srcinfo.image_width, srcinfo.image_height); 
 *
 * for (compnum=0; compnum < srcinfo.num_components; compnum++)
 *   fprintf(stderr,
 *           " component: %d  width in blocks: %d  height in blocks: %d\n",
 *           compnum,srcinfo.comp_info[compnum].width_in_blocks,
 *           srcinfo.comp_info[compnum].height_in_blocks);
 * fprintf(stderr,"\n");
 */

/***************************************************************************/

  /*
   * Copy coefficients into new real array, flipping the sign
   * of each coefficient corresponding to a DCT basis
   * function which is asymmetric in the vertical direction.
   */

  for (compnum=0; compnum<srcinfo.num_components; compnum++)
    {
    block_row_size = (size_t) SIZEOF(JCOEF)*DCTSIZE2
                              *srcinfo.comp_info[compnum].width_in_blocks;
    for (rownum=0; rownum<srcinfo.comp_info[compnum].height_in_blocks; rownum++)
      {
      row_ptrs[compnum] = ((&dstinfo)->mem->access_virt_barray) 
                          ((j_common_ptr) &dstinfo, coef_arrays[compnum], 
                            rownum, (JDIMENSION) 1, FALSE);
      for (blocknum=0; blocknum<srcinfo.comp_info[compnum].width_in_blocks;
           blocknum++)
        for (i=0; i<DCTSIZE2; i++)
          {
          if ( (i/DCTSIZE)%2 == 1 ) 
            {
            coef_buffers[compnum][rownum][blocknum][i] =
            -row_ptrs[compnum][0][blocknum][i];
            }
          else
            {
            coef_buffers[compnum][rownum][blocknum][i] =
            row_ptrs[compnum][0][blocknum][i];
            }
          }
      }
    }

/***************************************************************************/

  /*
   * Write coefficient block rows back into virtual array, in
   * upside-down order.
   */

  for (compnum=0; compnum<srcinfo.num_components; compnum++)
    {
    block_row_size = (size_t) SIZEOF(JCOEF)*DCTSIZE2
                              *srcinfo.comp_info[compnum].width_in_blocks;
    for (rownum=0; rownum<srcinfo.comp_info[compnum].height_in_blocks; rownum++)
      {
      row_ptrs[compnum] = ((&dstinfo)->mem->access_virt_barray) 
                          ((j_common_ptr) &dstinfo, coef_arrays[compnum], 
                           rownum, (JDIMENSION) 1, TRUE);
      memcpy(row_ptrs[compnum][0][0], 
             coef_buffers[compnum]
             [(srcinfo.comp_info[compnum].height_in_blocks-1)-rownum][0],
             block_row_size);
      }
    }

/***************************************************************************/

  /*
   * Specify data destination for compression
   */

  jpeg_stdio_dest(&dstinfo, output_file);

  /*
   * Start compressor
   */

  jpeg_write_coefficients(&dstinfo, coef_arrays);

  /*
   * Finish compression and release memory
   */

  jpeg_finish_compress(&dstinfo);
  jpeg_destroy_compress(&dstinfo);
  (void) jpeg_finish_decompress(&srcinfo);
  jpeg_destroy_decompress(&srcinfo);

  /*
   * All done.
   */

  exit(jsrcerr.num_warnings + jdsterr.num_warnings ?EXIT_WARNING:EXIT_SUCCESS);
  return 0;      /* suppress no-return-value warnings */
}
