/*
 * Author :: JL Garofalo (Username: Garofal)
 * Class  :: CPSC 3220
 * Instructor :: Dr. Sorber
 * Date       :: 12-04-2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* Helper Functions */
 uint16_t getSector(uint16_t fatEntry){
 	return (33 + fatEntry - 2);
 }

 ssize_t getFileSize(char *filepath){
 	FILE *fp = fopen(filepath, "rb");
 	assert(fp != NULL);

 	//Seek to the end of the file and get the file size
 	fseek(fp, 0L, SEEK_END);
 	ssize_t filesize = ftell(fp);
 	rewind(fp);
 	fclose(fp);
 	return filesize;
 }


 int main(int argc, char *argv[]){
 	assert(argc == 3);

 	//Program Input Parameters
 	char *diskpath = argv[1];
 	char *destDirectory = argv[2];

 	//Get Disk size
 	ssize_t disksize = 0;
 	disksize = getFileSize(diskpath);
 	assert(disksize > 0);

 	printf("%zu\n", disksize);

 	return 0;
 }