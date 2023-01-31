/*Documentation
Name: Y.Charan Teja Reddy
Date: 27/01/2023
Description: C-Project1: "Steganography"
Input:
For encoding: ./a.out -e beautiful.bmp secret.txt
Secret file data: My password is secret :)
Encoding output: 
Selected encoding..........
Read and validate encode arguments is a success
<..........Started Encoding..........>
Open files is a success
check capacity is a success
copied bmp header successfully
Encoded magic string
Encoded secret file extension size
Encoded secret file extension successfully
Encode secret file size successfully
Encoded secret file data
Copied remaining data
Encoded successfullly

For decoding: ./a.out -d stego.bmp decode.txt
Decoding output:
Selected decoding..........
Read and validate decode arguments is a success
<..........Started Decoding..........>
Open files is a success
Decoded magic string
Decoded secret file extension size. It is 4 bytes.
Decoded secret file extension successfully. It is ".txt".
Decoded secret file size. It is 25 bytes.
Decoded secret file data successfully. Decoded data successfully written in file "decode.txt".
Decoded successfully

Decoded data: My password is secret :)
*/

#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc , char **argv)
{
    if(argc < 3)
    {
        printf("Error!! Invalid number of arguments entered.\nPlease enter minimum 4 valid arguments for encoding and minimum 3 valid arguments for decoding\n");
        exit(0);
    }
    //Check operation type
    if(check_operation_type(argv) == e_encode)
    {
        printf("Selected encoding..........\n");

        //Declare struture member for encoding
        EncodeInfo encInfo;

        //Validate input arguments for encoding 
        if((read_and_validate_encode_args(argv,&encInfo)) == e_success)
        {
            printf("Read and validate encode arguments is a success\n");
            printf("<..........Started Encoding..........>\n");

            //start encoding
            if(do_encoding(&encInfo) == e_success)
            {
                 printf("Encoded successfully\n");
            }
            else
            {
                printf("Failed to encode\n");
                return -1;
            }
        }
        else
        {
            printf("Read and validate encode argument is a failure\n");
            return -1;
        }
    }

    //Check if argument type is decoding 
    else if(check_operation_type(argv) == e_decode)
    {
        printf("Selected decoding..........\n");

        //Declare struture member for decoding
        DecodeInfo decInfo;

        //Validate input arguments for decoding
        if((read_and_validate_decode_args(argv,&decInfo)) == e_success)
        {
            printf("Read and validate decode arguments is a success\n");
            printf("<..........Started Decoding..........>\n");

            //Start decoding
            if(do_decoding(&decInfo) == e_success)
            {
                printf("Decoded successfully\n");
            }
            else
            {
                printf("Failed to decode\n");
                return -1;
            }
        }
        else
        {
            printf("Read and validate decode argument is a failure\n");
            return -1;
        }
    }

    else
    {
        printf("Invalid option\nPlease pass for\nEncoding: ./a.out -e  beautiful.bmp secret.txt stego.bmp\nDecoding: ./a.out -d stego.bmp decode.txt\n");
    }
    
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1] , "-e") == 0)
        return e_encode;
    if(strcmp(argv[1] , "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
