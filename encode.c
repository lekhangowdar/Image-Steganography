/*
name : Lekhan Gowda R
date : 14/8/2024
description : steganography
*/
//include all the header including user defined header files
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;  //multipy 3, because it,s defined to one(in RGB) of pixel, we need all 3(R,G&B) of pixel
}

/* Get file size
 * Input: file ptr of the file whose size is to be found
 * Output: file size
 * return value : size of the file
 */
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);             //to move fptr from starting to the last position 
    unsigned int size = ftell(fptr);    //store the size using ftell
    rewind(fptr);                       //make the file pointer point to the beginning of the file 
    return size;
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    //open Src Image file in read mode and store in structure
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r"); 
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    //open Secret file in read mode and store in structure
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");          
    // Do Error handling
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);   //store secret file size in structure
    //do error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    //open Stego image file in read mode and store in structure
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    return e_success;
}

/* validate and store arguements passed in structure
 * Inputs: command line arguements , structure pointer
 * Output: store srrc file name, secret file name and output file name 
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check if src file extension is ".bmp" and other file validation
    char *src_file_ext = strstr(argv[2],".bmp");      
    int src_file_len = strlen(argv[2]);
    if( (src_file_ext-argv[2] == src_file_len-4) && src_file_ext )  //to check ".bmp" is present at the end of the file name
    {
        encInfo->src_image_fname = argv[2];     //store 2nd arguement in stucture 
    }
    else 
    {
        printf("Source file is not in .bmp format\n");
        return e_failure;
    }

    //store secret file extension and other file validation
    char *sec_file_ext = strstr(argv[3],".");
    strcpy(encInfo->extn_secret_file,sec_file_ext);  //store extension in structure
    int size = strlen(argv[3]);
    if( sec_file_ext>argv[3] && (sec_file_ext-argv[3]< size) )  //check if extension is present at the end
        encInfo->secret_fname = argv[3];           //store 3rd arguement in stucture 
    else
    {
        printf("Secret file is not in proper file format\n");
        return e_failure;
    }

    //store output file extension and other file validation
    if(argv[4])    //check if arguement is passed for output file name
    {
        char *out_file_ext = strstr(argv[4],".bmp");  //check if output file extension is ".bmp"
        int out_file_len = strlen(argv[4]);
        if( (out_file_ext-argv[4] == out_file_len-4) && out_file_ext)  //to check ".bmp" is present at the end of the file name
            encInfo->stego_image_fname = argv[4];   //store 4th arguement in stucture 
        else
        {
            printf("Output file is not in .bmp format\n");
            return e_failure;
        }
    }
    //if output file name is not passed, assign default file name.bmp
    else
        encInfo->stego_image_fname = "stego.bmp";
    return e_success;
}

/* step by step to fallow encoding process by calling various functions
 * Inputs: structure pointer and function calls for encoding
 * Output: encode magic_string, extensions, secret data in output image file 
 * Return Value: e_success or e_failure, on file errors
 */
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)   //function call to open files
    {
        printf("Open file failure\n");
        return e_failure;
    }
    printf("Open file success\n");

    if (check_capacity(encInfo) == e_failure)   //function call to check capacity
    {
        printf("Capacity not available\n");
        return e_failure;
    }
    printf("Capacity available\n");
    printf("---------------------------------------------------\n");

    if (copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure) //function call to copy header to output
    {
        printf("Header not copied\n");
        return e_failure;
    }
    printf("Header copied\n");

    if (encode_magic_string(MAGIC_STRING,encInfo) == e_failure)  //function call to encode magic string
    {
        printf("Magic String not encoded\n");
        return e_failure;
    }
    printf("Magic String encoded\n");

    if (encode_secret_file_extn_size(encInfo) == e_failure)  //function call to encode sec file extn size
    {
        printf("Secret File Extension Size not Encoded\n");
        return e_failure;
    }
    printf("Secret File Extension Size Encoded\n");

    if (encode_secret_file_extn(encInfo->extn_secret_file,encInfo ) == e_failure) //function call to encode sec file extn
    {
        printf("Secret File Extension not Encoded\n");
        return e_failure;
    }
    printf("Secret File Extension Encoded\n");

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)  //function call to encode sec file size
    {
        printf("Secret File Size not Encoded\n");
        return e_failure;
    }
    printf("Secret File Size Encoded\n");
 
    if ( encode_secret_file_data(encInfo) == e_failure )   //function call to encode sec file data
    {
        printf("Secret File Data not Encoded\n");
        return e_failure;
    }
    printf("Secret File Data Encoded\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)   //function call to copy remaining data
    {
        printf("Remaining image data is not copied\n");
        return e_failure;
    }
    printf("Remaining image data copied\n");

    //printf the size of output and src image files
    printf("---------------------------------------------------\n");
    printf("output image size = %d\n",get_file_size(encInfo->fptr_stego_image));
    printf("source image size = %d\n",get_file_size( encInfo->fptr_src_image));
    return e_success;
}

/* copy header file 
 * Inputs: src file pointer and output file pointer
 * Output: copy header file from src image to output image file as it is
 * Return Value: e_success or e_failure, on file errors
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char arr[54];
    rewind(fptr_dest_image);           //both rewind(fptr) and fseek(fptr,0,SEEK_SET),
    fseek(fptr_src_image,0,SEEK_SET);  //performs same function

    //read 54 bytes from src image and write it to the output image file
    fread(arr,54,1,fptr_src_image);    
    fwrite(arr,54,1,fptr_dest_image);
    return e_success;
}

/* check capacity 
 * Inputs: stucture pointer
 * Output: check capacity, if it can store the data in the image file
 * Return Value: e_success or e_failure, on file errors
 */
Status check_capacity(EncodeInfo *encInfo)
{
    int magic_len = strlen(MAGIC_STRING);    //store length of magic string 
    int ext_sec_len = strlen(encInfo->extn_secret_file);   //store length of extn of sec file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  //store image capacity

    //check if the image capacity is more than or equal to data need to be encoded
    if( encInfo->image_capacity >= 54 +(magic_len +4 +ext_sec_len +4 +encInfo->size_secret_file)*8)
    {
        return e_success;
    }
    return e_failure;
}

/* encode magic string 
 * Inputs: structure pointer and magic string
 * Output: encode magic string
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //function call encode_data to image
    if(encode_data_to_image(magic_string,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error in encoding magic string\n");
        return e_failure;
    }
    return e_success;
}

/* encode secret file extn size
 * Inputs: structure pointer 
 * Output: encode secret file extn size
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_extn_size(EncodeInfo *encinfo)
{
    uint len = strlen(encinfo->extn_secret_file);   //store length of exten file
    //function call encode size
    if(encode_secret_file_size(len, encinfo) == e_failure)
    {
        printf("Error in encoding secret file extn size\n");
        return e_failure;
    }
    return e_success;
}

/* encode secret file extn 
 * Inputs: structure pointer and file extn
 * Output: encode secret file extn 
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encinfo)
{
    int a = strlen(encinfo->extn_secret_file);  //store length of exten file
    //function call encode_data to image
    if(encode_data_to_image(encinfo->extn_secret_file, a, encinfo->fptr_src_image, encinfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    return e_success;
}

/* encode secret file size 
 * Inputs: structure pointer and file extn
 * Output: encode secret file size 
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encinfo)
{
    char buffer[32];     //create charater buffer of size 32

    //read 32 bytes, encode data and write 32 bytes to output image from src image file
    fread(buffer, 1, 32, encinfo->fptr_src_image);   
    for (int i = 0; i < 32; i++)
    {
    /*    get a bit from src dat
          clear the lsb of the byte od dest dat
          or the lsb bit and get bit and store in output   */
        buffer[i] = (buffer[i] & 0xFE) | (file_size >> i) & 1;   
    }
    fwrite(buffer, 1, 32, encinfo->fptr_stego_image);
    return e_success;
}

/* encode secret file data
 * Inputs: structure pointer
 * Output: encode secret file data
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_data(EncodeInfo *encinfo)
{
    char buffer[encinfo->size_secret_file];     //create buffer of sec file size
    //read the data into buffer from file
    fread(buffer, 1, sizeof(buffer), encinfo->fptr_secret);
    //function call encode data to image
    if(encode_data_to_image(buffer, encinfo->size_secret_file, encinfo->fptr_src_image, encinfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    return e_success;
}

/* encode data to image
 * Inputs: data, size, src file pointer, output file pointer
 * Output: encode data to image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];   //create a buffer of size 8
    for(int i=0;i<size;i++)   //run loop of size times
    {
        //read from src image to buffer,encode and write to output file
        fread(buffer,1,8,fptr_src_image);
        if(encode_byte_to_lsb(data[i],buffer) == e_failure)  //function call encode byte to lsb
        {
            printf("Error in encoding byte to LSB\n");
            return e_failure;
        }
        fwrite(buffer,1,8,fptr_stego_image);
    }
    return e_success;
}

/* encode byte to lsb
 * Inputs: data, buffer
 * Output: encode data to image
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i;
    for(i=0;i<8;i++)   //run loop for 8 times
    {
    /*    get a bit from data
          clear the lsb of the byte od buff
          or the lsb bit and get bit and store in output   */
        image_buffer[i]= (image_buffer[i]& 0xFE) | ((data>>i) & 1);
    }
    return e_success;
}

/* copy remaining data
 * Inputs: src file pointer and dest file pointer
 * Output: copy remaining data
 * Return Value: e_success or e_failure, on file errors
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    //read a character from file and run while loop till a character is not read
    while( fread(&ch,1,1,fptr_src) > 0)  
    {
        fwrite(&ch,1,1,fptr_dest);  //write into the dest
    }
    return e_success;
}