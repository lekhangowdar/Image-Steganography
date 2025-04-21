/*
name : Lekhan Gowda R
date : 14/8/2024
description : steganography

Input : (CLA) /a.out -e beautiful.bmp secret.txt out.bmp

Encoding Selected
Open file success
width = 1024
height = 768
Capacity available
Header copied
Magic String encoded
Secret File Extension Size Encoded
Secret File Extension Encoded
Secret File Size Encoded
Secret File Data Encoded
Remaining image data copied
output image size = 2359351
source image size = 2359351
Encoding Sucessful

Output :(CLA) ./a.out -d out.bmp decoded.txt

Decoding Selected
Open Decode file success
Magic string is decoded
Secret File Extension Size decoded
Secret File Extension decoded
Output file opened
output File Size decoded
Output file decoded
Decoding Sucessful
*/

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

//pass the CLA in main function
int main(int argc, char *argv[])
{
    //declare structure pointer for both encode and ecode
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    //check if minimum 3 arguements is passed
    if (argc<3)
    {
        printf("CLA should not be less than 3\n");
        return 0;
    }

    //call check operation type 
    int ret = check_operation_type(argv);

    //check if -e is entered
    if(ret == e_encode) 
    {
        if (argc<4)   //check if minimum 3 arguements is passed
        {
            printf("CLA should not be less than 4 for encoding operation\n");
            return 0;
        }
        printf("\n\tEncoding Selected\n");
        printf("---------------------------------------------------\n");
        //call read and validate encode function
        if(read_and_validate_encode_args(argv,&encInfo) == e_success)
        {
            //call do_encoding function
            if(do_encoding(&encInfo) == e_success)
            {
                printf("---------------------------------------------------\n");
                printf("\tEncoding Sucessful\n\n");
                return 0;
            }
            else
            {
                printf("---------------------------------------------------\n");
                printf("\tEncoding Failure\n\n");
                return 0;
            }
        }
        else
        {
            printf("Read and Validate Failure\n");
            return 0;
        }
    }

    //check if -d is entered
    else if(ret == e_decode)
    {
        if (argc<3)  //check if minimum 3 arguements is passed
        {
            printf("CLA should not be less than 3 for decoding operation\n");
            return 0;
        }
        printf("\n\tDecoding Selected\n");
        printf("---------------------------------------------------\n");
        //call read and validate encode function
        if(read_and_validate_decode_args(argv,&decInfo) == e_success)
        {
            //call do_encoding function
            if(do_decoding(&decInfo) == e_success)
            {
                printf("---------------------------------------------------\n");
                printf("\tDecoding Sucessful\n\n");
            }
            else
            {
                printf("---------------------------------------------------\n");
                printf("\tDecoding Failure\n\n");
            }
        }
        else
            printf("Read and Validate Failure\n"); 
    }
    else
        printf("Unsupported Format\n");
    return 0; 
}

//function defination to check operation type
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)   //check if -e is entered
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)  //check if -d is entered
    {
        return e_decode;
    }
    else return e_unsupported;
    return 0;
}

