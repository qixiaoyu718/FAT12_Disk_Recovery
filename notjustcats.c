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
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* FAT12 Constants */
 //Size Constants 
 	//Memory (in Bytes)
	#define SECTOR_SIZE 512
	#define FAT_SIZE (512 * 9)
	#define ROOT_SIZE (512 * 14)
	#define DATA_SIZE (512 * 2847)
	#define MAX_BYTES (512 * 2879)
 	#define DIR_ENTR_SIZE 32
 	//File System Components
 	#define SECTOR_DIRECTORIES 16
 	#define FAT_ENTRIES ((FAT_SIZE * (2/3)) + 1)

 //Location Constants
	#define DATA_LOC (SECTOR_SIZE + (FAT_SIZE * 2) + ROOT_SIZE)
	#define FAT2_LOC (SECTOR_SIZE + FAT_SIZE)
	#define ROOT_LOC (SECTOR_SIZE + (FAT_SIZE * 2))

/* Struct to hold two entries */
typedef struct Entries{
	uint16_t first;
	uint16_t second;
} Entries;

typedef struct File {
	unsigned char *filename;
	unsigned char *extension;
	unsigned char *attributes;
	unsigned char *reserved;
	unsigned char *creationTime;
	unsigned char *creationDate;
	unsigned char *lastAccessed;
	unsigned char *lastModTime;
	unsigned char *lastModDate;
	unsigned char *firstLogCluster; 
	unsigned char *fileSize; 		//(in Bytes)
} File;

/* Global Variables */
char bootBlock[SECTOR_SIZE];
uint16_t *fat1[FAT_ENTRIES];
uint16_t *fat2[FAT_ENTRIES];
unsigned char rootDirectory[ROOT_SIZE];
unsigned char data[DATA_SIZE];

File **rootFiles;
int numRootFiles = 0;
int rootDirectorySize = 16;



/* Helper Functions */
	/* Function printRoot       */

	/* Function: addRootFile    */
		//Purpose: Adds a root file to the data structure to hold
		void addRootFile(File *file){
			assert(file != NULL);
			if(numRootFiles < rootDirectorySize){
				rootFiles[numRootFiles] = file;
				numRootFiles++;
			} else {
				//rootFiles = (File **) realloc(rootFiles, (rootDirectorySize + 16));
				rootFiles[numRootFiles] = file;
				numRootFiles++;
			}
		}

	/* Function: printFile      */
		//Purpose: Print formatted file information to stdout
		void printFile(File *file){
			assert(file != NULL);
			assert(file->filename != NULL);
			int i = 0;
			while(file->filename[i] != ' ' && file->filename[i] != '.' && i < 8){
				printf("%c", file->filename[i]);
				i++;
			}
			printf(".");
			assert(file->extension != NULL);
			i = 0;
			while(file->extension[i] != '.' && i < 3){
				printf("%c", file->extension[i]);
				i++;
			}
			printf("\t");
			assert(file->fileSize);
			i = 0;
			while(file->fileSize[i] != '.' && i < 4){
				printf("%c", file->fileSize[i]);
				i++;
			}
			printf("\n");
		}

	/* Function: getValidFiles  */
		void getValidFiles(){
			assert(rootDirectory != NULL);

			int currentInd = 0;
			while((currentInd + DIR_ENTR_SIZE) <= ROOT_SIZE){
				int startingIndex = currentInd;
				if((229 != rootDirectory[currentInd]) && (rootDirectory[currentInd] != 0)){
					File *nextFile = malloc(sizeof(File));
					//Read in filename-------------------------------------------
					assert((currentInd - startingIndex) == 0);
					nextFile->filename = (unsigned char *)malloc(8 * sizeof(char));
					for(int i = 0; i < 8; i++){
						nextFile->filename[i] = rootDirectory[currentInd+i];
					}
					currentInd += 8;

					//Read in extension------------------------------------------
					assert((currentInd - startingIndex) == 8);
					nextFile->extension = (unsigned char *)malloc(3 * sizeof(char));
					for(int i = 0; i < 3; i++){
						nextFile->extension[i] = rootDirectory[currentInd+i];
					}
					currentInd += 3;

					//Read in attributes-----------------------------------------
					assert((currentInd - startingIndex) == 11);
					nextFile->attributes = (unsigned char *)malloc(1 * sizeof(char));
					nextFile->attributes[0] = rootDirectory[currentInd];
					currentInd++;

					//Read in reserved-------------------------------------------
					assert((currentInd - startingIndex) == 12);
					nextFile->reserved = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->reserved[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in creationTime---------------------------------------
					assert((currentInd - startingIndex) == 14);
					nextFile->creationTime = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->creationTime[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in creationDate---------------------------------------
					assert((currentInd - startingIndex) == 16);
					nextFile->creationDate = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->creationDate[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in lastAccessed info----------------------------------
					assert((currentInd - startingIndex) == 18);
					nextFile->lastAccessed = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->lastAccessed[i] = rootDirectory[currentInd+i];
					}
					currentInd += 4;

					//Read in lastModified Time----------------------------------
					assert((currentInd - startingIndex) == 22);
					nextFile->lastModTime = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->lastModTime[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in lastModified Date----------------------------------
					assert((currentInd - startingIndex) == 24);
					nextFile->lastModDate = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->lastModDate[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in first logical cluster------------------------------
					assert((currentInd - startingIndex) == 26);
					nextFile->firstLogCluster = (unsigned char *)malloc(2 * sizeof(char));
					for(int i = 0; i < 2; i++){
						nextFile->reserved[i] = rootDirectory[currentInd+i];
					}
					currentInd += 2;

					//Read in file size------------------------------------------
					assert((currentInd - startingIndex) == 28);
					nextFile->fileSize = (unsigned char *)malloc(4 * sizeof(char));
					for(int i = 0; i < 4; i++){
						nextFile->reserved[i] = rootDirectory[currentInd+i];
					}
					currentInd += 4;

					printFile(nextFile);
					//addRootFile(nextFile);
				} else {
					currentInd += 32;
				}
			}

		}

	/* Function: getEntries     */
		//Purpose: Calculates the entry values from three bytes (specific to FAT12)
		Entries getEntries(char *firstByte, char *secondByte, char *thirdByte){
			Entries myEntries;
			// Split the second byte up
			uint16_t second = (uint16_t) strtol(secondByte, &secondByte, 16);
			uint16_t nib1 = (second >> 4); //Dest: Second
			uint16_t nib2 = ((second & 0x0f) << 8); //Dest: First

			//Place the nibs into their respective entries
			uint16_t first = (uint16_t) strtol(firstByte, &firstByte, 16);
			myEntries.first = (nib2 | first);
			uint16_t third = (uint16_t) strtol(thirdByte, &thirdByte, 16);
			myEntries.second = (nib1 | (third << 4));

			return myEntries;
		}

	/* Function: getSector      */
		uint16_t getSector(uint16_t fatEntry){
			return (uint16_t)(33 + fatEntry - 2);
		}

	/* Function: translateDisk  */
		void translateDisk(unsigned char *disk){
			//Read the contents of the disk into the appropriate data structures
		char FAT1[FAT_SIZE];
		char FAT2[FAT_SIZE];

			int counter = 0;
			while(counter < MAX_BYTES){
				unsigned char nextByte = disk[counter];

				if(counter < SECTOR_SIZE){
					bootBlock[counter] = nextByte;
				} else if(counter < FAT2_LOC) {
					FAT1[counter-SECTOR_SIZE] = nextByte;
				} else if(counter < ROOT_LOC){
					FAT2[counter-FAT2_LOC] = nextByte;
				} else if(counter < DATA_LOC){
					rootDirectory[counter-ROOT_LOC] = nextByte;
				} else {
					data[counter-DATA_LOC] = nextByte;
				}
				counter++;
			}

			//Format the FATs correctly
			int i = 0;
			int entryInd = 0;
			char *one = malloc(1);
			char *two = malloc(1);
			char *three = malloc(1);
			while(i+3 < FAT_SIZE){
				*one = FAT1[i];
				*two = FAT1[i+1];
				*three = FAT1[i+2];

				Entries e = getEntries(one, two, three);
				fat1[entryInd] = malloc(sizeof(uint16_t));
				fat1[entryInd+1] = malloc(sizeof(uint16_t));
				*fat1[entryInd] = e.first;
				*fat1[entryInd+1] = e.second;

				i += 3;
				entryInd += 2;
			}
		}

	/* Function: getDisk        */
		unsigned char *getDisk(char *filepath){
			unsigned char *disk;
			unsigned int length;
			struct stat filebuf;

			int fp = open(filepath, O_RDONLY);
			//Assert file was read properly
			assert(fp >= 0);
			assert(fstat(fp, &filebuf) >= 0);

			length = (unsigned int) filebuf.st_size;
			disk = (uint8_t *) mmap(0, length, PROT_READ, MAP_FILE|MAP_PRIVATE, fp, 0);
			assert(disk != 0);

			return disk;
		}

/* Removeable Debug Functions */
	/* Check Contents              */
		// void checkContents(){
		// 	 printf("\n\nBoot\n");
		// 	for(int i = 0; i < 10; i++){
		// 		printf("%c\n", bootBlock[i]);
		// 	}
		// 	printf("\n\nFAT1\n");
		// 	for(int i = 0; i < 10; i++){
		// 		printf("%c", FAT1[i]);
		// 	}

		// 	printf("\n\nFAT2\n");
		// 	for(int i = 0; i < 10; i++){
		// 		printf("%c", FAT2[i]);
		// 	}

		// 	printf("\n\nData\n");
		// 	for(int i = 0; i < 10; i++){
		// 		printf("%c", data[i]);
		// 	}
		// }

/* Main */

 int main(int argc, char *argv[]){
 	assert(argc == 3);

 	//Program Input Parameters
 	char *diskpath = argv[1];
 	char *destDirectory = argv[2];

 	//Get Disk
 	unsigned char *disk = getDisk(diskpath);
 	translateDisk(disk);

 	getValidFiles();
 	//printf("%c-%c-%c\n", rootDirectory[8], rootDirectory[9], rootDirectory[10]);


 	return 0;
 }