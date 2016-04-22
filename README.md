# FAT12_Disk_Recovery
* Authors
  * JL Garofalo 
  * Parker Vansickle
* Instructor: Dr. Sorber
* Class:      CPSC 3220
* Date:         12-04-2015

## Purpose
This tool parses disk images (formatted using the FAT12 File System), printing out information about each of the files contained on the disk image including deleted files that were recovered. The tool saves the files into a subdirectory specified by the user on the command line.

## Running Instructions
1. Download the project4.tgz into a directory
2. De-compress it:

		tar -cvzf project4.tgz
3. Type "make" command into the terminal

      		make
      
4. Execute:

		./Fat12Recovery <image filename> <output directory>
		./Fat12Recovery floppy/simple.img

## Design:
Sequence of Execution

1. main()
  * Translates input parameters
  * Makes an output directory
  * Sequence Step 2
  * Sequence Step 3
  * Sequence Step 4
  * Returns and terminates execution
2. getDisk(char *filepath)
  * Determines image file size
  * Finds and opens the disk image into an unsigned char array pointer
  * Returns to Sequence Step 1
3. translateDisk(unsigned char *disk)
  * Parses the raw image bytes into the filesystem components:
    * Boot Block
    * FAT1
    * FAT2
    * Root Directory
    * Data Block
    * Converts the FAT entries into their proper 12-bit format
    * Returns to Sequence Step 1
4. exploreRoot()
  * Parses the root directory into 32 Byte directory entries
  * For each directory entry:
    * Records the initial filepath
    * Sequence Step 5
    * Converts little endian file sizebytes to big endian
    * If the file size is greater than 0 (indicating a data file):
      * Sequence Step 6
      * Sequence Step 7
    * Else the file size is 0 (indicating directory):
      * Sequence Step 8
      * Returns to Sequence Step 1
5. createFile(unsigned char *raw, char *filepath)
    * Parses 32 Byte raw unsigned char directory entry data 
    * Stores each respective into a single struct File pointer
    * Returns struct File pointer to Sequence Step 4 or 7
6. printfFile(struct File *file)
    * Checks information about the recovered file
    * Prints to stdout using format:
       * FILE<tab>NORMAL<tab><file path><tab><file size (B)>
       * FILE<tab>DELETED<tab><file path><tab><file size (B)>
    * Returns to Sequence Step 4 or 7 or 8
7. exploreFile(struct File *file)
    * Extracts the extension information from file and creates string with full path to save output
    * Opens a new file for writing
    * Determines first logical cluster and moves to appropriate index of global data array
    * Uses FAT array for subsequent clusters, writes data out byte by byte to new file
    * Returns to Sequence Step 4 or 8
8. exploreDirectory(struct File *file)
    * Converts the directory's first logical cluster to Big Endian
    * Uses FAT1 to get the all the sectors in data
    * Parses the file directory into 32 Byte directory entries
    * For each directory entry:
       * Records the initial filepath
       * Sequence Step 5
       * Converts little endian file sizebytes to big endian
       * If the file size is greater than 0 (indicating a data file):
          * Sequence Step 6
          * Sequence Step 7
       * Else the file size is 0 (indicating directory):
          * Sequence Step 8 (Recursion)
          * Returns to Sequence Step 4 or 8


## Known Problems
  * File Data Corruption
     * Source : Unknown
          * With more complex or deep directory trees, the tool may change the contents of the file slightly causing data corruption.
  * Segmentation Fault
     * Source : Unknown
          * Often with random disk images, there's a segmentation fault error when attempting to explore subdirectories of the root.

## Appendix
### Table 1.1: Directory Entry Byte Breakdown

Starts at (offset) |  Length (B) | Description                   
-------------------|-------------|-------------------------------
         0         |  8          | Filename                      
         8         |  3          | Extension                     
        11         |  1          | Attributes                    
        12         |       2     | Reserved                      
        14         |       2     | Creation Time  
        16         |       2     | Creation Date       
       18          |       2     | Last Access Date              
        22         |       2     | Last Write Time               
        24         |       2     | Last Write Date               
        26         |       2     | First Logical Cluster         
        28         |       4     | File Size                     
