#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
/*Function to calculate input .bmp file size*/
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

/*Function to open files in required mode*/
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*Function to validate input arguments for encoding from user*/
Status read_and_validate_encode_args(char *argv[] , EncodeInfo *encInfo)
{
    //check if original .bmp file passed or not
    if(strcmp(strstr(argv[2],".") , ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
        return e_failure;

    //check if secret file passed or not
    if(strcmp(strstr(argv[3],".") , ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
        return e_failure;

    //check if image file to encode data passed or not
    //if not passed create a file 
    if(argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

/*Function to check capacity of input bmp file*/
Status check_capacity(EncodeInfo *encInfo)
{
    //call function to get input .bmp image size and store in structure member
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //call function to get input secret file size and store in structure member
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    //logic to check if input .bmp image file is capable to store secret file data
    if(encInfo->image_capacity > ((2 + 4 + 4 + 4 + encInfo->size_secret_file) * 8))
        return e_success;
    else
        return e_failure;
}

/*Function to get file size*/
uint get_file_size(FILE *fptr)
{
    //seek last position of file
    fseek(fptr, 0 ,SEEK_END);

    //ftell will return byte it is pointing to
    return ftell(fptr);
}

/*Function to copy input bmp file header to stego image */
Status copy_bmp_header(FILE *fptr_src_image , FILE *fptr_dest_image )
{
    char str[54];

    //seek 0th position
    fseek(fptr_src_image, 0, SEEK_SET);

    //read 54 byte bmp header data from input file
    fread(str, 54, 1, fptr_src_image);

    //write 54 byte bmp header data into stego image
    fwrite(str, 54, 1, fptr_dest_image);

    return e_success;
}

/*Function to encode magic string*/
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/*Function to encode data to stego image*/
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for(int i = 0 ; i < size ; i++)
    {
        //read 8 byte data at a time from input bmp file
        fread(encInfo->image_data, 8, 1, fptr_src_image);

        //encode data to be hidden into lsb of data read from input bmp file 
        encode_byte_to_lsb(data[i], encInfo->image_data);

        //write the encode 8 bytes into stego image
        fwrite(encInfo->image_data, 8, 1, fptr_stego_image);
    }
}

/*Function to encode secret file extension size*/
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char str[32];
    //read 32 bytes from input bmp source image
    fread(str, 32, 1, fptr_src_image);

    //store secret file extn size into these 32 bytes
    encode_size_to_lsb(size,str);

    //write these encoded 32 bytes into stego image
    fwrite(str, 32, 1, fptr_stego_image);
    return e_success;
}

/*Function to encode size to lsb*/
Status encode_size_to_lsb(int size, char *image_buffer)
{
    //logic to encode size into lsb of data read from input bmp file
    unsigned int mask = 1 << 31, i;

    for(i = 0 ; i < 32 ; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
}

/*Function to encode byte to lsb*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //logic to encode data into lsb of data read from input bmp file
    unsigned int mask = 0x80, i;

    for(i = 0 ; i < 8 ; i++)
    {
        image_buffer[i] = ((image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i)));
        mask = mask >> 1;
    }
}

/*Function to store secret file extension into stego image*/
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/*Function to encode secret file size into stego image*/
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
    char str[32];
    //read 32 byte data from source bmp file
    fread(str, 32, 1, encInfo->fptr_src_image);

    //encode secret file size into these 32 byte data
    encode_size_to_lsb(size, str);

    //write these encoded 32 bytes into stego image
    fwrite(str, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

/*Function to store secret file data into stego image*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //seek 0th position of secret file
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];

    //read secret file size equivalent data from secret file
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);

    //encode data read from secert file to stego image file
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/*Function to copy remainig input bmp file dat to stego image*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    //read 1 byte data at a time from bmp source image
    while((fread(&ch, 1, 1, fptr_src)) > 0)
    {
        //write the data read from source image to destinaton image i.e stego image
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("Open files is a success\n");
        
        if(check_capacity(encInfo) == e_success) 
        {
            printf("check capacity is a success\n");

            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("copied bmp header successfully\n");

                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Encoded magic string\n");
                    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, ".") );
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        printf("Encoded secret file extension size\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoded secret file extension successfully\n");
                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Encode secret file size successfully\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoded secret file data\n");
                                    if(copy_remaining_img_data(encInfo-> fptr_src_image, encInfo-> fptr_stego_image) == e_success)
                                    {
                                        printf("Copied remaining data\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to encode secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to encode secret file size");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to encode secret file extension\n");
                        }
                    }
                    else
                    {
                        printf("Failed to encode secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("failed to copy bmp header\n");
                return -1;
            }
        }
        else
        {
            printf("check capactiy is a failure\n");
            return -1;
        }
    }
    else
    {
        printf("Open files is a failure\n");
        return -1;
    }
    return e_success;
}
