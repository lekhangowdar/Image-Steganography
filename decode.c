/*
name : Lekhan Gowda R
date : 14/8/2024
description : steganography
*/
//include all the header including user defined header files
#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>

/* validate and store arguements passed in structure
 * Inputs: command line arguements , structure pointer
 * Output: store src file name,  output file name 
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_decode_args( char *argv[], DecodeInfo *decInfo)
{
    //check if src file extension is ".bmp" and other file validation
    char *stego_file_ext = strstr(argv[2],".bmp");  //to check ".bmp" is present at the end of the file name
    int stego_file_len = strlen(argv[2]);
    if( (stego_file_ext-argv[2] == stego_file_len-4) && stego_file_ext )
    {
        decInfo->stego_image_fname = argv[2];   //store 2nd arguement in stucture
    }
    else
    {
        printf("Encoded Image file is not in .bmp format\n");
        return e_failure;
    }

    //check if arguement is passed for output file name
    if(argv[3] && argv[3][0]!='.')
    {
        decInfo->output_fname = argv[3];  //store 4th arguement in stucture 
    }
    //if output file name is not passed, assign default file name.txt
    else    
        decInfo->output_fname = "decoded" ;
    return e_success;
}

/* Get File pointers for i/p files
 * Inputs: Stego Image file
 * Output: FILE pointer for stego image files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_stego_image(DecodeInfo *decInfo)
{
    //open Stego Image file in read mode and store in structure
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
    	return e_failure;
    }
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
}

/* step by step to fallow decoding process by calling various functions
 * Inputs: structure pointer and function calls for decoding
 * Output: decode magic_string, extensions, secret data in output image file 
 * Return Value: e_success or e_failure, on file errors
 */
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_stego_image(decInfo) == e_failure)  //function call to open stego image
    {
        printf("Open Decode file failure\n");
        return e_failure;
    }
    printf("Open Decode file success\n");

    if (decode_magic_string(strlen(MAGIC_STRING),decInfo) == e_failure) //function call to decode magic string
    {
        return e_failure;
    }
    printf("Magic string is decoded\n");

    if (decode_extn_size_of_output(decInfo) == e_failure)  //function call to decode output extn size
    {
        printf("Secret File Extension Size not decoded\n");
        return e_failure;
    }
    printf("Secret File Extension Size decoded\n");

    if ( decode_output_extn(decInfo) == e_failure)  //function call to decode output extn 
    {    
        printf("Secret File Extension not decoded\n");
        return e_failure;
    }
    printf("Secret File Extension decoded\n");

    if(open_output_file(decInfo) == e_failure)   //function call to open output file
    {
        printf("Output file not opened\n");
        return e_failure;
    }
    printf("Output file opened\n");

    if(get_output_file_size(decInfo)== e_failure)  //function call to decode output file size
    {
        printf("output File Size not decoded\n");
        return e_failure;
    }
    printf("output File Size decoded\n");
  
    if ( decode_output_file_data(decInfo) == e_failure )  //function call to decode output file data
    {
        printf("Secret File Data not Encoded\n");
        return e_failure;
    } 
    printf("Output file decoded\n");
    return e_success;
}

/* decode magic string 
 * Inputs: structure pointer and magic string length
 * Output: decode magic string
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_magic_string(int magic_len, DecodeInfo *decInfo)
{
    char buff[magic_len+1];            //create buff of length of magic string + 1
    decInfo->data_size = magic_len;    //store length of magic string in structure (data size)

    //function call decode byte from lsb
    if(decode_byte_from_lsb(buff,decInfo) == e_failure)
    {
        return e_failure;
    }

    //check if decoded data is matched with the original magic string
    if(strcmp(buff,MAGIC_STRING) != 0)
    {
        printf("Magic String is not matched\n");
        return e_failure;
    }
    return e_success;
}

/* decode byte from lsb
 * Inputs: empty array (data[]) to store decoded data, structre pointer
 * Output: decode data to image
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_byte_from_lsb(char data[], DecodeInfo *decInfo)
{
    int i,j;
    char lsb[8];   //declare a buffer of size 8

    //run a outter loop for data_size times
    for(j=0;j<decInfo->data_size;j++)
    {
        //read from stego image to buffer(lsb),decode data and write to output file
        fread(lsb,1,8,decInfo->fptr_stego_image);
        for ( i=7; i>=0; i-- )
        {
            //left shift data 1 time
            //get bit from lsb and OR with data[j], later update data
            data[j]= (data[j]<<1) | (lsb[i] & 1) ; 
        }
    }
    data[decInfo->data_size] = '\0' ;  //add character at the end of the data
    return e_success;
}

/* decode output file extn size
 * Inputs: structure pointer
 * Output: decode output file extn size
 * Return Value: e_success or e_failure, on file errors
 */ 
Status decode_extn_size_of_output(DecodeInfo *decInfo)
{
    //function call decode size from lsb
    if(decode_size_from_lsb(&decInfo->extn_output_size,decInfo) == e_failure)
    {
        return e_failure;
    }
    return e_success;
}

/* decode size from lsb
 * Inputs: integer pointer size, structre pointer
 * Output: decode size from lsb
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_size_from_lsb(int *size, DecodeInfo *decInfo)
{
    char buff[32];   //declare a buffer of size 32
    //read 32 bytes from stego image file and store it in buffer
    fread(buff, 1, 32, decInfo->fptr_stego_image);

    //run a loop for 32 time where i is decrementing from 31 to 0
    for (int i = 31; i >= 0; i--)   
    {
    /*    get lsb bit from buff
          left shift size by 1 time
          OR the get bit and size   */
        *size = (*size << 1) | (buff[i] & 1);  
    }
    return e_success;
}

/* decode output file extn 
 * Inputs: structure pointer
 * Output: decode output file extn
 * Return Value: e_success or e_failure, on file errors
 */  
Status decode_output_extn(DecodeInfo *decInfo)    
{
    char buff[decInfo->extn_output_size + 1];         //create buff of length of extn_output_size  + 1
    decInfo->data_size = decInfo->extn_output_size;   //store extn_output_size in structure (data size)
    //function call decode byte from lsb
    if(decode_byte_from_lsb(buff,decInfo) == e_failure)
    {
        return e_failure;
    }
    strcpy(decInfo->extn_output_file,buff);    //store decoded extn of output file in structure
    return e_success;
}

/* Get File pointers for o/p file
 * Inputs: Structure pointer
 * Output: FILE pointer for output files
 * Return Value: e_success or e_failure, on file errors
 */ 
Status open_output_file(DecodeInfo *decInfo)
{
    // Check if the extn for output file is present while passing through CLA
    printf("---------------------------------------------------\n");
    printf("Output file name is '%s' \n",decInfo->output_fname);
        printf("'%s' file name is converted into",decInfo->output_fname);
        char *pos = strstr(decInfo->output_fname, "."); // store location, where "." is present in output file name

        // Allocate enough memory for the new output file name
        int new_output_name_len = strlen(decInfo->output_fname) + strlen(decInfo->extn_output_file) + 1;
        char *new_output_name = (char *)malloc(new_output_name_len);

        if (pos != NULL)
        {
            int index = pos - decInfo->output_fname; // get the index where the extn starts in file name
            strncpy(new_output_name, decInfo->output_fname, index); // copy index number of bits from output name
            new_output_name[index] = '\0';           // store null at index position
            strcat(new_output_name, decInfo->extn_output_file); // concatenate output name with the extn
        }
        else 
        {
            strcpy(new_output_name, decInfo->output_fname);     // copy the original output name
            strcat(new_output_name, decInfo->extn_output_file); // concatenate output name with the extn
        }

        // Update the new output file name in structure
        decInfo->output_fname = new_output_name;
        printf(" '%s'\n",decInfo->output_fname);
    printf("---------------------------------------------------\n");
    // Open output file name in write mode
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    
    // Do Error handling
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open output file %s\n", decInfo->output_fname);
        return e_failure;
    }

    return e_success;
}


/* decode output file size
 * Inputs: structure pointer
 * Output: decode output file size
 * Return Value: e_success or e_failure, on file errors
 */
Status get_output_file_size(DecodeInfo *decInfo)
{
    //function call decode size from lsb
    if(decode_size_from_lsb(&decInfo->size_output_file,decInfo) == e_failure)
    {
        return e_failure;
    }
    return e_success;    
}

/* decode output file data
 * Inputs: structure pointer
 * Output: decode output file data
 * Return Value: e_success or e_failure, on file errors
 */   
Status decode_output_file_data(DecodeInfo *decInfo)
{
    char buff[decInfo->size_output_file + 1];        //create buff of size of output_file  + 1  
    decInfo->data_size = decInfo->size_output_file;  //store size of output_file in structure (data size)
    //function call decode byte from lsb
    if(decode_byte_from_lsb(buff,decInfo) == e_failure)
    {
        return e_failure;
    }

    //write the data of size_output_file bytes from buffer to output file
    fwrite(buff,1,decInfo->size_output_file,decInfo->fptr_output);
    return e_success;
}