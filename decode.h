/*
name : Lekhan Gowda R
date : 14/8/2024
description : steganography
*/
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding stego/encoded file to output file
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* output info */
    char *output_fname;
    FILE *fptr_output;
    char extn_output_file[MAX_FILE_SUFFIX];
    int size_output_file;
    int extn_output_size;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    //temporary size for storing length of data(magic string, output extn, output dat) to be encoded 
    int data_size;

} DecodeInfo;

/* Decoding function prototype */
/* Read and validate Decode args from argv */
Status read_and_validate_decode_args( char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get Decode File pointers for i/p and o/p files */
Status open_stego_image(DecodeInfo *decInfo);

//decode magic string 
Status decode_magic_string(int magic_len, DecodeInfo *decInfo); 

//decode byte from lsb 
Status decode_byte_from_lsb(char *data, DecodeInfo *decInfo); 

//decode output extension size 
Status decode_extn_size_of_output(DecodeInfo *decInfo); 

//decode size from lsb 
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo); 

//decode output extension 
Status decode_output_extn(DecodeInfo *decInfo); 

//open output file 
Status open_output_file(DecodeInfo *decInfo); 

//get output file size 
Status get_output_file_size(DecodeInfo *decInfo); 

//decode output file data 
Status decode_output_file_data(DecodeInfo *decInfo); 

#endif