#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to decode secret file information stored in
 * stego image to Output file
 * Info about intput and intermediate data is
 * also stored
 */


typedef struct _DecodeInfo
{
   
    /* Decode File Info */
    char *decode_fname;
    FILE *fptr_decode;
    char magic_string[3];
    char secret_file_extn[5];
    unsigned int secret_file_extn_size;
    unsigned int secret_file_size;
    

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status Open_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode Secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Deocde secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);





#endif
