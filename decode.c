#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

Status Open_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }

    // Decode file
    decInfo->fptr_decode = fopen(decInfo->decode_fname, "w");
    // Do Error handling
    if (decInfo->fptr_decode == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*Function to validate input arguments from user*/ 
Status read_and_validate_decode_args(char *argv[] , DecodeInfo *decInfo)
{
    //check if stego file is passed as an argument
    if(strcmp(strstr(argv[2],".") , ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
        return e_failure;

    //check if output file to store secret data is passed 
    //if not passed create an output file
    if(argv[3] != NULL)
    {
        decInfo->decode_fname = argv[3];
    }
    else
    {
        decInfo->decode_fname = "decode.txt";
    }
    return e_success;
}

/*Function to decode magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
    unsigned char char_byte;
    unsigned char ch = 0;
    int i,j;

    //Make file pointer point to 54th byte
    fseek(decInfo->fptr_stego_image, 54 , SEEK_SET);

    //logic to decode magic string
    for(i = 0 ; i <2 ; i++)
    {
        for(j = 0 ; j < 8 ; j++)
        {
            //read 1 byte of data at a time from stego image
            //decode the lsb of each byte and combine to get a character 
            fread(&char_byte, 1 , 1 ,decInfo->fptr_stego_image);
            char_byte = char_byte & 0x01;
            ch <<= 1;
            ch = ch | char_byte;
        }
        decInfo->magic_string[i] = ch;
    }
    decInfo->magic_string[i] = '\0';

    //logic to check if magic string is decode properly
    if(strcmp(decInfo->magic_string , "#*") == 0)
        return e_success;
    else
        return e_failure;
}

/*Function to decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    unsigned char ch = 0;
    unsigned int size = 0;
    int i , j;

    //logic to decode secret file extension size
    for(i = 0; i < 32 ; i++)
    {
        //read 1 byte of data at a time from stego image
        //decode lsb from each byte and combine to get secret file extn size
        fread(&ch, 1 , 1 , decInfo->fptr_stego_image);
        ch &= 0x01;
        size <<= 1;
        size |= ch;
    }
    //store secret file extension size in structure member
    decInfo->secret_file_extn_size = size;
    return e_success;
}

/*Function to decode secret file extension*/
Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    unsigned char ch = 0;
    int i,j;

    //logic to decode secret file file extension and store in structure member
    for(i = 0 ; i < 4 ; i++)
    {
        unsigned char char_byte = 0;
        for(j = 0 ; j < 8 ; j++)
        {
            //read 1 byte of data at a time from stego image
            //decode the lsb of each byte and combine to get a character 
            fread(&char_byte, 1 , 1 ,decInfo->fptr_stego_image);
            char_byte = char_byte & 0x01;
            ch <<= 1;
            ch = ch | char_byte;
        }
        decInfo->secret_file_extn[i] = ch;
    }
    decInfo->secret_file_extn[i] = '\0';

    return e_success;
}

/*Function to decode secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char ch = 0;
    unsigned int size = 0;
    int i , j;

    //Logic to decode secret file size and store in structure member
    for(i = 0; i < 32 ; i++)
    {
        //read 1 byte of data at a time from stego image
        //decode lsb from each byte and combine to get secret file size
        fread(&ch, 1 , 1 , decInfo->fptr_stego_image);
        ch &= 0x01;
        size <<= 1;
        size |= ch;
    } 
    decInfo->secret_file_size = size;
    return e_success;
}

/*Function to create secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char str[decInfo->secret_file_size];
    unsigned char ch=0;
    int i , j;

    //logic to decode secret file data    
    for(i = 0 ; i < sizeof(str) ; i++)
    {
        unsigned char char_byte = 0;
        for(j = 0 ; j < 8 ; j++)
        {
            //read 1 byte of data at a time from stego image
            //decode lsb from each byte and combine to get a character of secret file data
            fread(&ch, 1, 1, decInfo->fptr_stego_image);
            ch &= 0x01;
            char_byte <<= 1;
            char_byte |= ch;
        }
        str[i] = char_byte;
    }
    //write decoded secret file data into output file 
    fwrite(str , sizeof(str), 1 ,decInfo->fptr_decode);
    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    if( Open_files(decInfo) == e_success)
    {
        printf("Open files is a success\n");

         if(decode_magic_string(decInfo) == e_success)
         {
             printf("Decoded magic string\n");
                           
             if(decode_secret_file_extn_size(decInfo) == e_success)
             {
                 printf("Decoded secret file extension size. It is %d bytes.\n",decInfo->secret_file_extn_size);
                 if(decode_secret_file_extn(decInfo) == e_success)
                 {
                     printf("Decoded secret file extension successfully. It is \"%s\".\n",decInfo->secret_file_extn);
                     if(decode_secret_file_size(decInfo) == e_success)
                     {
                         printf("Decoded secret file size. It is %d bytes.\n", decInfo->secret_file_size);
                         if(decode_secret_file_data(decInfo) == e_success)
                         {
                             printf("Decoded secret file data successfully. Decoded data successfully written in file \"%s\".\n",decInfo->decode_fname);
                         }
                         else
                         {
                             printf("Failed to decode secret file data\n");
                         }
                     }
                     else
                     {
                         printf("Failed to decode secret file size\n");
                         return e_failure;
                     }
                 }
                 else
                 {
                     printf("Faild to decode secret file extn\n");
                     return e_failure;
                 }
                    }
                    else
                    {
                        printf("Failed to decode secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to decode magic string\n");
                    return e_failure;
                }
    }
    else
    {
        printf("Open files is a failure\n");
        return -1;
    }
    return e_success;
}
