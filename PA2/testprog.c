#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
//device driver will have maximum buffer size of 1024
#define BUFFER_MAX_SIZE 1024
#define WRITE_SIZE 100

//function for reading from file
void _read(int fd) {
	//get the number of bytes to be read from user - can't be more than 1024
	printf("Enter the number of bytes you want to read\n");
	char *c, input[100];
	long bytes_to_read;
	while (fgets(input,sizeof(input),stdin)) {
		bytes_to_read = strtol(input, &c, 10);
		if (c == input || *c != '\n') {
			printf("Please enter an integer: ");
		}
		else if (bytes_to_read < 0 || bytes_to_read > BUFFER_MAX_SIZE) {
			printf("Please enter an integer between 0 and 1024: ");
		}
		else {
			break;
		}	
	}
	//successfully got bytes to read
	char *user_buffer = (char *)malloc(bytes_to_read);
	int readbytes = read(fd, user_buffer, bytes_to_read);
	//error or signal interrupt when reading
	if (readbytes > 0) {		
		printf("# of bytes read: %d\n", readbytes);
		printf("Bytes read: %s\n", user_buffer);
	}
	else if (readbytes == 0) {
		printf("Reached end of file when reading\n");
		printf("Bytes read: %s\n", user_buffer);
	}
	else {
		printf("Error or signal interrupt when reading");
		exit(1);
	}
	//free heap space
	free(user_buffer);
	return;
}

void _write(int fd) {
	//prompt user for data to write
	printf("Enter the data you want to write:");
	//max size of input
	char buff[WRITE_SIZE];
	//get input
	fgets(buff,WRITE_SIZE,stdin);
	//write input bytes	
	int writebytes = write(fd, buff, strlen(buff));
	//error or signal interrupt when writing
	if (writebytes > 0) {
                printf("# of bytes written: %d\n", writebytes);
	}
        else if (writebytes == 0) {
                printf("Reached end of file when reading\n");
        }
        else {
                printf("Error or signal interrupt when reading");
                exit(1);
        }
}

void _seek(int fd) {
        printf("Enter an offset value:\n");
        //make sure input is integer
	char *c, input[100];
        long byte_offset;
        while (fgets(input,sizeof(input),stdin)) {
                byte_offset = strtol(input, &c, 10);
                if (c == input || *c != '\n') {
                        printf("Please enter an integer: ");
                }
                else {
                        break;
                }
	}
	//have valid byte offset - get whence
	char whence[9] = "";
	char set[9] = "SEEK_SET";
	char cur[9] = "SEEK_CUR";
	char end[9] = "SEEK_END";
	int check;
	//get whence input
	while (1) {
		printf("Enter a value for whence: (SEEK_SET,SEEK_CUR,SEEK_END)\n");
		fgets(whence,9,stdin);
		//check input
		if (strcmp(whence,set) == 0) {
			check = 0;
			break;
		}
		else if (strcmp(whence,cur) == 0) {
                        check = 1;
			break;
                }
		else if (strcmp(whence,end) == 0) {
                        check = 2;
			break;
                }
	}
	//have valid offset and whence
	int offset;
	if (check == 0) {
		offset = lseek(fd,byte_offset,SEEK_SET);
	}
	else if (check == 1) {
		offset = lseek(fd,byte_offset,SEEK_CUR);
	}
	else if (check == 2) {
		offset = lseek(fd,byte_offset,SEEK_END);
	}
	else {
		printf("Something went wrong in _seek\n");
		exit(1);
	}
	//error handling
	if (offset < 0) {
		printf("Error when setting offset in file\n");
		exit(1);
	}
	else {
		printf("Offset value set to byte: %d in file\n",offset);
	}
}


int main(int argc, char *argv[]) {
	//user input fixed size
	char input[20];	
	char *filename;	
	//no commandline argument given
	if (argc < 2) {
		printf("No filename specified\n");
		exit(1);
	}
	//command line argument given
	else {
		filename = argv[1];
	}
	//open file in read/write mode - byte offset automatically set to 0
	int fd = open(filename,O_RDWR);
	//if file wasn't opened successfully
	if(fd < 0) {
		printf("Couldn't open %s successfully\n",filename);
		perror("While opening file");
		exit(1);
	}
	//file was opened sucessfully
	else {
		printf("%s opened successfully\n",filename);
		printf("option?\n");
		int option;
		while (fgets(input,20,stdin)) {
			//convert input to integer for switch
			if (*input == 'r') { option = 0;}
			else if (*input == 'w') { option = 1;}
			else if (*input == 's') { option = 2;}
			else { option = 3;}
  			//switch cases for input
			switch (option)
			{
				case 0:
					//call read fcn
					_read(fd);
                                        printf("option?\n");		
					break;
				case 1:
					//call write fcn
					_write(fd);
                                        printf("option?\n");
					break;
				case 2:
					//call offset fcn
					_seek(fd);
					//catch \n
					fgets(input,20,stdin);
					printf("option?\n");
					break;
				//ignore and re-prompt
				default:
					printf("option?\n");
					break;
			}
		}
		//Close file, exit program 
		if (close(fd) < 0) {
			printf("Couldn't close %s successfully\n", filename);
			perror("While closing file");
			exit(1);
		} 
		//file clossed successfully
		printf("File closed successfully\n");
	}
}
