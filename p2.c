/* Student Name: Sanchita Kanade
 * Class: CS570
 * Due Date: 11/28/2018
 * File Name:p2.c
 * Assignment: program4
 */


#include "p2.h"
#include "getword.h"

/*
 * Following global flags are for:
 * 1.word: This is an array to store every new string stored in storage array (s) by getword method.
 * 2.newargv: This is an array of pointers to characters, which is used to point each string stored in word array.
 * 3.counterDoubleLessThan: This is an integer used to count number of '<<' encountered in the string.
 * 4.pipeReadIndex = This indicates the index of the newargv array
 *   where executable command after the pipe symbol is located.
 * 5.input_output_filenames: this is an array used to store input and output file names for input/output redirection.
 * 6.fileDes: this is a file descriptor array with two slots.
 * 7. index1 to index11: These are indices of newargv array, which are used to get the location of commands used in pipes.
 * 8.cwd: It is an character array used to store the path of current working directory.
 * 9.token: This is a character array used to get last word of the current working directory.
 * 10.token1: This is character array used to store whole path after removing '~' sign, when '~' encounters first in the string.
 * 11.nullCommandFound: This is an integer which will set to value 1, when there is no command provided after or before pipe.
 * 12.tempfile: this is a temporary file created when '<<' encounters in the string.
 * 13.patternToSearch: It is a character array used to store a search pattern provided right after the '<<' in the string.
 * 14.pipeBackProcess: This is an integer which will set to value 1 when there are pipes in the string and
 * '&' is encountered at the end of the string, which indicates we need to run the pipe handling process in the background.
 * 15.input_redirect_back: this flag will be set to indicate that the
 *  input/output redirection process will be run in background.
 * 16.pipeFlag: this flag will set when pipe '|' is encountered.
 *17.lineCounter: this will set when we put null pointer at the place of pipe in the newargv array
 * and it will reset again for the next line.
 * 18. flags: this will set when '&' sign encounters at the end of the string
 * so this indicates process will run in the background. when this flag is set we
 * will not increase count so '&' will not add into the newargv array.
 * and we will not call execWithArgs method
 * 19.input_redirect_flags: this flag will set when we encounter input redirection symbols lile '<' or '>'
 *20.initIndex: this indicates the index of the executable file or command in the array
 *21.count: This is an integer which indicates current index of the newargv and word array.
 *22.inputRedirectCount: this indicates number of '<' symbols encountered
 *23.ouputRedirectCount: this indicates number of '>' symbols encountered
 *24.redirecterrorflag: this flag will set if there is any error in the input or output redirection.
 *25.str: This is a pointer to the string which will appear in the prompt after execution of 'cd' command.
 */

char word[MAXITEM][STORAGE];
char *newargv[MAXITEM];
int counterDoubleLessThan;
int pipeReadIndex;
char input_output_filenames[2][STORAGE];
int fileDes[2];
int index1,index2,index3, index4, index5,index6,index7,index8, index9,index10,index11;
char cwd[PATH_MAX];
char *token;
char *token1;
int nullCommandFound;
FILE *tempfile;
char patternToSearch[1][STORAGE];
int pipeBackProcess = 0;
int input_redirect_back = 0;
int pipeFlag = 0;
int lineCounter = 0;
int flags = 0;
int input_redirect_flags = 0;
int initIndex = 0;
int count = 0;
int inputRedirectCount = 0;
int ouputRedirectCount = 0;
int redirecterrorflag = 0;
char *str = "";

/*
 * This method creates a temporary file when '<<' encounters in the string and stores all input words into the
 * temporary file except the string pattern provided right after the '<<' symbol.
 */
void createTempFile() {
	char *buffer;
	size_t size;
	int numOfCharacters;

	tempfile = tmpfile();
	if(tempfile == NULL) {
		perror("tmpfile");
		exit(1);
	}

	strcat(patternToSearch[0],"\n");

	while(!feof(stdin)) {

		numOfCharacters = getline(&buffer,&size,stdin); //returns -1 on error or EOF
		if(strcmp(patternToSearch[0], buffer) == 0) {
			break;
		}
		if(numOfCharacters != -1 && strcmp(patternToSearch[0], buffer) != 0){
			fputs(buffer,tempfile);
		}
	}
}

/*
 * This method handles pipe redirection upto ten pipes.
 * here firstIndex indicates the index of the newargv array where first command provided to the pipe is stored. Similarly, lastIndex indicates
 * the index of the newargv array where last command provided to the pipe is stored.
 * In this method parent will create a 'firstchild', then that firstchild will fork another 'child' and execute last command.
 * Then child will create a for loop, fork a 'grandchild' and executes second last command. This process will repeat for all descendants
 * until the last descendant executes the first command.
 *
 * if input_redirect_flags is set to 1 or 3 then the last descendant will take an input from provided input file,
 * if input_redirect_flags is set to 2 or 3 then the firstchild will write the output into the output file.
 * Similarly if '<<' is encountered in the string then the last descendant will take an input from the temporary file.
 */
void multiPipe(char *arg[], int firstIndex,int index2,int index3, int index4, int index5, int index6, int index7, int index8, int index9, int index10, int lastIndex) {
	int fd[20];
	int i,offset,j,k,x,pipeOffset;
	pid_t child, grandchild,firstchild;

	fflush(stdout);
	fflush(stderr);
	CHK(firstchild = fork());
	if(firstchild == 0) {

		CHK(pipe(fd));

		fflush(stdout);
		fflush(stderr);
		CHK(child = fork());
		if(child != 0) {

			if(input_redirect_flags == 2 || input_redirect_flags == 3) {
				out_fd = creat(input_output_filenames[1], OUTPUT_MODE);
				if(out_fd < 0) {
					exit(3);
				}

				CHK(dup2(out_fd,STDOUT_FILENO));
				CHK(close(out_fd));
			}

			CHK(dup2(fd[0],STDIN_FILENO));
			CHK(close(fd[0]));
			CHK(close(fd[1]));

			CHK(execvp(arg[lastIndex],arg+lastIndex));

		} else {
			if(doubleLessthan > 0 && counterDoubleLessThan == 1){
				createTempFile();
			}

			pipeOffset = 2;
			for(x = 1; x < pipeFlag; x++) {
				CHK(pipe(fd + pipeOffset));
				pipeOffset = pipeOffset + 2;
			}

			offset = 0;
			for(i = 1; i <= pipeFlag; i++) {

				offset = i*2;

				if(i == pipeFlag) {
					if(doubleLessthan > 0 && counterDoubleLessThan == 1){

						//this method will take a pointer to the beginning of the file.
						rewind(tempfile);

						CHK(dup2(fileno(tempfile),STDIN_FILENO));
						CHK(close(fileno(tempfile)));

					}

					if(input_redirect_flags == 1 || input_redirect_flags == 3) {

						ip_fd = open(input_output_filenames[0],O_RDONLY);

						if(ip_fd < 0) {
							exit(2);
						}
						CHK(dup2(ip_fd,STDIN_FILENO));
						CHK(close(ip_fd));
					}


					CHK(dup2(fd[offset - 1],STDOUT_FILENO));

					for(j = 2; j < pipeOffset; j++) {
						CHK(close(fd[j]));
					}

					CHK(execvp(arg[firstIndex],arg+firstIndex));
				}

				fflush(stdout);
				fflush(stderr);
				CHK(grandchild = fork());
				if(grandchild != 0) {

					CHK(dup2(fd[offset],STDIN_FILENO));
					CHK(dup2(fd[offset - 1],STDOUT_FILENO));

					for(k = 0; k < pipeOffset; k++) {
						CHK(close(fd[k]));
					}

					if(i == 1) {
						CHK(execvp(arg[index10],arg+index10));
					} else if(i == 2) {
						CHK(execvp(arg[index9],arg+index9));
					}else if(i == 3) {
						CHK(execvp(arg[index8],arg+index8));
					}else if(i == 4) {
						CHK(execvp(arg[index7],arg+index7));
					}else if(i == 5) {
						CHK(execvp(arg[index6],arg+index6));
					}else if(i == 6) {
						CHK(execvp(arg[index5],arg+index5));
					}else if(i == 7) {
						CHK(execvp(arg[index4],arg+index4));
					}else if(i == 8) {
						CHK(execvp(arg[index3],arg+index3));
					}else if(i == 9) {
						CHK(execvp(arg[index2],arg+index2));
					}
				}
			}
		}
	}
	if(pipeBackProcess != 1) {
		for(;;) {
			pid_t pid;
			CHK(pid = wait(NULL));
			if(pid == firstchild)
				break;
		}
	}

	if(pipeBackProcess  == 1) {
		pipeBackProcess = 0;
		printf("%s [%d]\n",arg[index10],firstchild);
		fflush(stdout);
	}
}

/*
 * This method handles pipe redirection up to ten pipes.
 * When input_redirect_flags set to 1(input redirection) or 2(output redirection) or 3(input and output redirection)
 *  then this method handles input output redirection along with pipe.
 *  when input_redirect_flags is set to 1  or 3 then grandchild will read from the input file and execute the first command.
 *  Similarly, if '<<' encounters grandchild will read from the temporary file and execute the first command.
 *  when input_redirect_flags is set to 2 or 3 then child will write to the output file instead of stdout and execute last command.
 *  In order to handle more than one pipes this method will call multiPipe() method.
 *
 */
void pipeRedirect(char *arg[]) {
	pid_t child, grandchild;
	int input_fd;
	int output_fd;

	if(ouputRedirectCount > 1) {
		fprintf(stderr,"Ambigous ouput redirect\n");
		ouputRedirectCount = 0;
		redirecterrorflag = 1;
	}

	if(inputRedirectCount > 1) {
		fprintf(stderr,"Ambigous input redirect\n");
		inputRedirectCount = 0;
		redirecterrorflag = 1;
	}

	if(strcmp(input_output_filenames[1],"&") == 0) {
		fprintf(stderr, "Missing name for redirect\n");
		redirecterrorflag = 1;
	}

	if(pipeFlag > 1 && (input_redirect_flags == 2 || input_redirect_flags == 3) && access(input_output_filenames[1],F_OK) == 0) {
		fprintf(stderr,"%s: File exists\n",input_output_filenames[1]);
	}else if(pipeFlag > 1 && redirecterrorflag != 1) {

		if(pipeFlag == 2) {
			multiPipe(arg,index1,0,0,0,0,0,0,0,0,index2,index3);

		} else if(pipeFlag == 3) {
			multiPipe(arg, index1, 0, 0,0,0,0,0,0,index2,index3, index4);

		} else if(pipeFlag == 4) {
			multiPipe(arg, index1, 0, 0, 0,0,0,0,index2,index3,index4, index5);

		} else if(pipeFlag == 5) {
			multiPipe(arg, index1, 0, 0, 0, 0,0,index2,index3,index4,index5, index6);

		} else if(pipeFlag == 6) {
			multiPipe(arg, index1, 0, 0, 0, 0, index2,index3,index4,index5,index6, index7);

		} else if(pipeFlag == 7) {
			multiPipe(arg, index1, 0,0,0,index2, index3, index4, index5, index6,index7, index8);

		} else if(pipeFlag == 8) {
			multiPipe(arg, index1, 0,0,index2, index3, index4, index5, index6,index7,index8,index9);

		} else if(pipeFlag == 9) {
			multiPipe(arg, index1, 0,index2, index3, index4, index5, index6,index7,index8,index9,index10);

		} else if(pipeFlag == 10) {
			multiPipe(arg, index1, index2, index3, index4, index5, index6,index7,index8,index9,index10,index11);
		} else {
			fprintf(stderr,"Only ten pipes are allowed\n");
		}
	}else {
		if(input_redirect_flags != 0) {
			if((input_redirect_flags == 2 || input_redirect_flags == 3) && access(input_output_filenames[1],F_OK) == 0) {
				fprintf(stderr,"%s: File exists\n",input_output_filenames[1]);

			} else if(redirecterrorflag != 1){
				fflush(stdout);
				fflush(stderr);
				CHK(child = fork());
				if(child == 0) {

					//if '<<' is encountered following code will create a temporary file.
					if(doubleLessthan > 0 && counterDoubleLessThan == 1){
						createTempFile();
					}

					CHK(pipe(fileDes));
					fflush(stdout);
					fflush(stderr);
					CHK(grandchild = fork());
					if(grandchild == 0) {
						if(doubleLessthan > 0 && counterDoubleLessThan == 1){

							//this will take a pointer to the beginning of the file.
							rewind(tempfile);

							CHK(dup2(fileno(tempfile),STDIN_FILENO));
							CHK(close(fileno(tempfile)));
						}
						if(input_redirect_flags == 1 || input_redirect_flags == 3) {

							input_fd = open(input_output_filenames[0],O_RDONLY);
							if(input_fd < 0) {
								exit(2);
							}
							CHK(dup2(input_fd,STDIN_FILENO));
							CHK(close(input_fd));
						}
						CHK(dup2(fileDes[1], STDOUT_FILENO));
						CHK(close(fileDes[0]));
						CHK(close(fileDes[1]));
						CHK(execvp(arg[initIndex],arg + initIndex));
					}
					if(doubleLessthan > 0 && counterDoubleLessThan == 1) {
						for(;;) {
							pid_t pid1;
							CHK(pid1 = wait(NULL));
							if(pid1 == grandchild)
								break;
						}
					}
					if(input_redirect_flags == 2 || input_redirect_flags == 3) {
						output_fd = creat(input_output_filenames[1], OUTPUT_MODE);
						if(output_fd < 0) {
							exit(3);
						}

						CHK(dup2(output_fd,STDOUT_FILENO));
						CHK(close(output_fd));
					}
					CHK(dup2(fileDes[0], STDIN_FILENO));
					CHK(close(fileDes[0]));
					CHK(close(fileDes[1]));
					if(doubleLessthan > 0 && counterDoubleLessThan == 1){
						fclose(tempfile);
					}
					CHK(execvp(arg[pipeReadIndex],arg+pipeReadIndex));
					exit(0);
				}
			}
		} else {
			fflush(stdout);
			fflush(stderr);
			CHK(child = fork());
			if(child == 0) {
				if(doubleLessthan > 0 && counterDoubleLessThan == 1){
					createTempFile();
				}
				CHK(pipe(fileDes));
				fflush(stdout);
				fflush(stderr);
				CHK(grandchild = fork());
				if(grandchild == 0) {
					if(doubleLessthan > 0 && counterDoubleLessThan == 1){
						//This will take the pointer to the beginning of the tempfile.
						rewind(tempfile);

						CHK(dup2(fileno(tempfile),STDIN_FILENO));
						CHK(close(fileno(tempfile)));
					}

					CHK(dup2(fileDes[1], STDOUT_FILENO));
					CHK(close(fileDes[0]));
					CHK(close(fileDes[1]));
					if(execvp(arg[initIndex],arg + initIndex) == -1) {
						fprintf(stderr,"error in writting to the pipe\n");
					}
				}
				if(doubleLessthan > 0 && counterDoubleLessThan == 1) {
					for(;;) {
						pid_t pid1;
						CHK(pid1 = wait(NULL));
						if(pid1 == grandchild)
							break;
					}
				}

				CHK(dup2(fileDes[0], STDIN_FILENO));
				CHK(close(fileDes[0]));
				CHK(close(fileDes[1]));
				if(doubleLessthan > 0 && counterDoubleLessThan == 1){
					fclose(tempfile);
				}

				if(execvp(arg[pipeReadIndex],arg+pipeReadIndex)== -1) {
					fprintf(stderr,"error in reading from the pipe\n");

				}
				exit(0);
			}
		}
		if(pipeBackProcess != 1) {
			for(;;) {
				pid_t pid;
				CHK(pid = wait(NULL));
				if(pid == child)
					break;
			}
		}
		if(pipeBackProcess  == 1) {
			pipeBackProcess = 0;
			printf("%s [%d]\n",arg[pipeReadIndex],child);
			fflush(stdout);
		}
	}
}

/*
 * This method handles background processes
 */
void backgroundProcess(char *newargv[]) {
	pid_t childpid;
	int in_fd;

	flags = 1;
	newargv[count-1] = NULL;

	fflush(stdout);
	fflush(stderr);
	CHK(childpid = fork());
	if(childpid == 0) {
		in_fd = open("/dev/null",O_RDONLY);
		if(in_fd < 0) {
			exit(2);
		}
		CHK(dup2(in_fd,STDIN_FILENO));
		close(in_fd);
		CHK(execvp(newargv[initIndex],newargv + initIndex));
		exit(0);
	}

	printf("%s [%d]\n",newargv[initIndex],childpid);
	fflush(stdout);
}

/*
 * This method handles regular commands with arguments
 */
void execWithArgs (char *arg[]) {
	pid_t childpid;

	fflush(stdout);
	fflush(stderr);
	CHK(childpid = fork());
	if(childpid == 0) {
		CHK(dup2(fileDes[0],STDIN_FILENO));
		if((execvp(arg[initIndex],arg + initIndex)) == -1) {
			fprintf(stderr,"'%s' binary not found\n",arg[initIndex]);
			exit(1);
		}
		CHK(close(fileDes[0]));
		CHK(close(fileDes[1]));
	}

	for(;;) {
		pid_t pid;
		CHK(pid = wait(NULL));
		if(pid == childpid)
			break;
	}
}

/*
 * This method replace the ~username with the absolute path to the user's home directory and
 * stores it into the newargv and word array.
 * here newString is an character array which stores a pathname after first '/'
 * tokenCpy is an character array which stores a string contain 'username:'
 * here I am using pipe, where child will execute 'grep username: /etc/passwd' command and write
 * the output string containing the user information into the pipe.
 * Then parent will read from the pipe and then getline function will store that line, containing the
 * user's information, into the memory pointed by buffer.
 * After that absolute path to the user's home directory will be extracted from the string pointed by buffer.
 * then ~username will be replaced with the absolute path to the user's home directory.
 */
void tildeConversion(char string[]) {
	FILE *infile;
	int filedes[2];
	int numOfCharacters;
	char newString[strlen(string)];
	char tempString[strlen(string)];
	char tokenCpy[40];
	char *token2;
	char *token3;
	pid_t child;
	char *arg[STORAGE];
	char *buffer;
	int counter2 = 0;
	int counter = 0;
	size_t bufsize = 32;

	strcpy(newString,"\0");

	token2 = strtok(string,"/");

	strcpy(tokenCpy,token2);

	while(token2 != NULL) {
		if(counter2 > 0){
			if(counter2 > 1) {
				strcat(newString,"/");
			}
			strcat(newString,token2);
		}
		token2 = strtok(NULL,"/");
		counter2++;
	}

	if(strcmp(newString,"") != 0) {
		strcpy(tempString,newString);
		strcpy(newString,"/");
		strcat(newString,tempString);
	}

	strcat(tokenCpy,":");

	arg[0] = "grep";
	arg[1] = tokenCpy;
	arg[2] = "/etc/passwd";
	arg[3] = NULL;

	pipe(filedes);

	fflush(stderr);
	fflush(stdout);
	CHK(child = fork());

	if(child == 0) {
		CHK(dup2(filedes[1],STDOUT_FILENO));
		CHK(close(filedes[0]));
		CHK(close(filedes[1]));

		if((execvp(arg[0],arg)) == -1) {
			perror("execvp() error");
			exit(1);
		}
	} else {

		//malloc function will allocate the memory on the heap and program will access that memory via buffer pointer.
		buffer = (char *)malloc(bufsize * sizeof(char));
		if(buffer == NULL) {
			perror("unable to allocate buffer");
			exit(1);
		}

		CHK(dup2(filedes[0],STDIN_FILENO));
		CHK(close(filedes[1]));

		infile = fdopen(filedes[0],"r");
		if(infile == NULL) {
			perror("fdopen");
			exit(1);
		}

		/*getline function will read the input stream, infile in this case (i.e from the read end of the pipe)
		 * and store that input into the buffer.
		 */
		numOfCharacters = getline(&buffer,&bufsize,infile);
		if(numOfCharacters == -1) {
			fprintf(stderr,"%s Unknown user",tokenCpy);
			exit(1);
		}

		/*following code will separate the sixth field of the input line stored in the buffer
		 * and replace ~username with that sixth field which is the absolute path to the user's
		 * home directory. Then the new pathname will be stored into the newargv and word array.
		 */
		token3 = strtok(buffer,":");

		while(token3 != NULL) {
			if(counter == 5) {
				strcat(token3,newString);

				newargv[count] = token3;
				strcpy(word[count],token3);
			}
			token3 = strtok(NULL, ":");
			counter++;
		}

		if(fclose(infile) != 0) {
			perror("fclose");
		}


		for(;;) {
			pid_t pid;
			CHK(pid = wait(NULL));
			if(pid == child)
				break;
		}
	}
}
/*
 * This method handles input and output redirection when input_redirect_flags are set.
 *  and when input_redirect_back flag is set, it also handles background processes.
 *
 */
void input_redirect(char *newargv[]) {
	pid_t kidpid;
	pid_t childpid;

	if(ouputRedirectCount > 1) {
		fprintf(stderr,"Ambigous ouput redirect\n");
		ouputRedirectCount = 0;
		redirecterrorflag = 1;
	}

	if(inputRedirectCount > 1) {
		fprintf(stderr,"Ambigous input redirect\n");
		inputRedirectCount = 0;
		redirecterrorflag = 1;
	}

	if(strcmp(input_output_filenames[1],"&") == 0) {
		fprintf(stderr, "Missing name for redirect\n");
		redirecterrorflag = 1;
	}

	if((input_redirect_flags == 2 || input_redirect_flags == 3) && access(input_output_filenames[1],F_OK) == 0) {
		fprintf(stderr,"%s: File exists\n",input_output_filenames[1]);

	} else if(redirecterrorflag != 1){

		if(input_redirect_back == 1) {

			fflush(stdout);
			fflush(stderr);
			CHK(childpid = fork());

			if(childpid == 0) {
				int file[2];

				if(input_redirect_flags == 1 || input_redirect_flags == 3) {

					file[0] = open(input_output_filenames[0],O_RDONLY);
					if(file[0] < 0) {
						fprintf(stderr,"Invalid input file\n");
						exit(2);
					}

					CHK(dup2(file[0],STDIN_FILENO));
					CHK(close(file[0]));
				}


				if(input_redirect_flags == 2 || input_redirect_flags == 3) {

					file[1] = creat(input_output_filenames[1], OUTPUT_MODE);
					if(file[1] < 0) {
						fprintf(stderr,"Error creating output file\n");
						exit(3);
					}
					CHK(dup2(file[1],STDOUT_FILENO));
					CHK(close(file[1]));
				}

				CHK(execvp(newargv[initIndex],newargv + initIndex));
				CHK(close(file[1]));
				exit(0);
			}

		} else {
			fflush(stdout);
			fflush(stderr);
			CHK(kidpid = fork());
			if(kidpid == 0) {
				if(input_redirect_flags == 1 || input_redirect_flags == 3) {
					fileDes[0] = open(input_output_filenames[0],O_RDONLY);

					if(fileDes[0] < 0) {
						fprintf(stderr,"Invalid input file\n");
						exit(2);
					}
					CHK(dup2(fileDes[0],STDIN_FILENO));
					CHK(close(fileDes[0]));
				}

				if(input_redirect_flags == 2 || input_redirect_flags == 3) {
					fileDes[1] = creat(input_output_filenames[1], OUTPUT_MODE);
					if(fileDes[1] < 0) {
						fprintf(stderr,"Error creating output file\n");
						exit(3);
					}
					CHK(dup2(fileDes[1],STDOUT_FILENO));
					CHK(close(fileDes[1]));
				}
				CHK(execvp(newargv[initIndex],newargv + initIndex));
				CHK(close(fileDes[1]));
			}
		}

		if(input_redirect_back != 1) {
			for(;;) {
				pid_t pid1;
				CHK(pid1 = wait(NULL));
				if(pid1 == kidpid)
					break;
			}
		}
	}
}

/*
 * This method handles input redirection when '<<' encounters.
 * This method will create a temporary file and then read the content from stdin until it encounters
 * the string pattern provided right after '<<' symbol and store that content into the temporary file.
 * In order to read the data from stdin, I have used getline function, which reads from stdin and stores the content into the buffer.
 * here I have forked a child process which will take the input from the temporary file instead of stdin  and execute the command.
 */
void doubleRedirection(char *arg[]) {
	FILE *temp;
	pid_t child;
	int fd,in_fd;
	char *buffer;
	size_t size;
	int numOfCharacters;


	size = STORAGE;

	if(ouputRedirectCount > 1) {
		fprintf(stderr,"Ambigous ouput redirect\n");
		ouputRedirectCount = 0;
		redirecterrorflag = 1;
	}

	temp = tmpfile();
	if(temp == NULL) {
		perror("tmpfile");
		exit(1);
	}

	strcat(patternToSearch[0],"\n");

	while(!feof(stdin)) {

		numOfCharacters = getline(&buffer,&size,stdin);
		if(strcmp(patternToSearch[0], buffer) == 0) {
			buffer = NULL;
			break;
		}
		if(numOfCharacters != -1 && strcmp(patternToSearch[0], buffer) != 0){
			fputs(buffer,temp);
		}
	}

	if((input_redirect_flags == 2 || input_redirect_flags == 3) && access(input_output_filenames[1],F_OK) == 0) {
		fprintf(stderr,"%s: File exists\n",input_output_filenames[1]);

	} else if(redirecterrorflag != 1 &&(input_redirect_flags == 2 || input_redirect_flags == 3)){

		fflush(stdout);
		fflush(stderr);

		CHK(child = fork());
		if(child == 0) {
			//rewind function will take the pointer to the beginning of the file
			rewind(temp);
			in_fd = fileno(temp);

			CHK(dup2(in_fd,STDIN_FILENO));
			CHK(close(in_fd));

			if(input_redirect_flags == 2 || input_redirect_flags == 3) {
				fd = creat(input_output_filenames[1], OUTPUT_MODE);
				if(fd < 0) {
					fprintf(stderr,"Error creating an output file\n");
					exit(3);
				}

				CHK(dup2(fd,STDOUT_FILENO));
				CHK(close(fd));

			}
			CHK(execvp(newargv[initIndex],newargv + initIndex));
		}
	}

	if(input_redirect_flags == 0) {
		fflush(stdout);
		fflush(stderr);

		CHK(child = fork());
		if(child == 0) {
			//rewind function will take the pointer to the beginning of the file
			rewind(temp);
			in_fd = fileno(temp);

			CHK(dup2(in_fd,STDIN_FILENO));
			CHK(close(in_fd));

			CHK(execvp(newargv[initIndex],newargv + initIndex));

		}
	}
	for(;;) {
		pid_t pid1;
		CHK(pid1 = wait(NULL));
		if(pid1 == child)
			break;
	}
	fclose(temp);

}


/*
 * This method parse the input line into words and insert them into the newargv array.
 * input_output flag will set to 1 or 3 to store input file into different array than newargv.
 * input_output flag will set to 2 or 3 to store output file into different array than newargv.
 *
 * I have used following variables for,
 * 1. c: indicates length of a word stored in the storage array s.
 * 2. line: this is a character array which stores a word which starts with '~' symbol.
 * 3. s: this is a character array, in which getword method stores a new word whenever program calls a getword method.
 * 4. pipeIndexFinder: This flag will set in order to get the correct indices of the newargv array where all commands need for pipe redirection are stored.
 * 5. environFlag: This flag will set when 'environ' encountered.
 * 6. null: this is a character array used to print a null string when undefined environmental variable is given as a input to 'environ' command.
 *7. environ: this is a character pointer which points to the value of the environmental variable when 'environ' string encounters.
 *8. pointerForFirstChar$: this is a character pointer which points to the value of the environmental variable followed by $ symbol.
 *9. wrongEnvVal: This flag will set when an undefined environmental variable is encountered.
 *10.catCommandEncountered: This flag will set when cat command is encountered.
 *10.changeDirFlag: This flag will set when cd command is encountered.
 *11.nextCdArg: This is an integer which will count number of arguments given to the cd command.
 *12. dirName: This is a character array which will store a directory name given after cd command.
 *
 */
void parse() {
	int c;
	char line[STORAGE];
	char s[STORAGE];
	int pipeIndexFinder;
	int environFlag;
	char null[1];
	char *environ;
	char *pointerForFirstChar$;
	int wrongEnvVal;
	int catCommandEncountered;
	int changeDirFlag;
	int nextCdArg;
	char *dirName[2];
	int wordCounter = 0;
	int increament_counter = 0;
	int input_output = 0;

	nullCommandFound = 0;
	counterDoubleLessThan = 0;
	doubleLessthan = 0;
	initIndex = count;
	inputRedirectCount =0;
	ouputRedirectCount = 0;

	//afterTildeEncountered = 0;// it will set to avoid EOF after tilde conversion process completion
	tildeFlag = 0;
	wrongEnvVal = 0;
	environFlag = 0;
	changeDirFlag = 0;
	pipeIndexFinder = 0;
	catCommandEncountered = 0;

	if (lineCounter == 1) {
		lineCounter = 0;
	}

	if(pipeFlag != 0) {
		pipeFlag = 0;
	}
	if(backslashFlag == 1) {
		backslashFlag = 0;
	}

	if(	input_redirect_flags !=0) {
		input_redirect_flags = 0;
	}

	if(input_redirect_back == 1) {
		input_redirect_back = 0;
	}

	for(;;) {
		tildeFlag = 0;

		c = getword(s);

		strcpy(word[count],s);
		newargv[count] = word[count];

		if(doubleLessthan > 0) {
			if(doubleLessthan == 1) {
				counterDoubleLessThan++;
			}
			doubleLessthan++;
			if(doubleLessthan == 3) {
				strcpy(patternToSearch[0],word[count]);
			}
		}

		if(tildeFlag == 1 && c > 1)	 {
			strcpy(line,word[count]);
			token1 = strtok(line,"~");
			tildeConversion(token1);
			tildeFlag = 0;
			//afterTildeEncountered = 1;
		}

		if(c < 0 && c != -255) {
			pointerForFirstChar$ = getenv(newargv[count]);
			if(pointerForFirstChar$ == NULL) {
				fprintf(stderr,"%s: Undefined Variable\n",newargv[count]);
				wrongEnvVal = 1;
			}else {
				newargv[count] = pointerForFirstChar$;
				strcpy(word[count],pointerForFirstChar$);
			}
		}
		if(environFlag == 2 && (strcmp(newargv[count],"") == 0 ||strcmp(newargv[count],"&") == 0)) { //for environ command
			environ = getenv(newargv[count -1]);
			if(environ != NULL) {
				printf("%s\n",environ);
				fflush(stdout);
				environFlag++;
			}else {
				null[0] = '\0';
				printf("%d\n",null[0]);
				fflush(stdout);
				environFlag++;
			}
		} else if(environFlag == 2 && strcmp(newargv[count],"") != 0) {
			environFlag++;
		} else if(environFlag == 3 && strcmp(newargv[count],"") == 0 && strcmp(newargv[count - 1],"&") != 0) {
			if(setenv(newargv[count-2],newargv[count-1],1) != 0) {
				perror("setenv() error");
			}
		}else {
			if(environFlag == 3 && strcmp(newargv[count - 1],"&") != 0) {
				environFlag++;
				fprintf(stderr,"Too many arguments to environ\n");
			}
		}

		if(environFlag == 1) {
			environFlag++;
		}

		if(changeDirFlag == 1) {
			if(nextCdArg == 0) {
				dirName[0] = word[count];
			}
			nextCdArg++;

			if((count -1)== initIndex && strcmp(dirName[0],"") == 0) {
				changeDirFlag++;
				if(chdir(getenv("HOME")) != 0) {
					perror("chdir() error");
				} else {
					if(getcwd(cwd,sizeof(cwd)) != NULL) {
						if(strcmp(cwd,"/") == 0) {
							str = "/";}
						else {
							token = strtok(cwd, "/");

							while(token) {
								str = token;
								token = strtok(NULL,"/");
							}
						}
					} else {
						perror("getcwd() error");
					}
				}
			} else if(nextCdArg == 2 && (count -2)== initIndex  && (strcmp(newargv[count],"") == 0 ||strcmp(newargv[count],"&") == 0)) {
				changeDirFlag++;
				if(chdir(dirName[0]) != 0) {
					perror("chdir() error");

				} else {
					if(getcwd(cwd,sizeof(cwd)) != NULL) {
						if(strcmp(cwd,"/") == 0) {
							str = "/";
						} else {
							token = strtok(cwd, "/");

							while(token) {
								str = token;
								token = strtok(NULL,"/");
							}
						}
					} else {
						perror("getcwd() error");
					}
				}
			} else if((count -2)== initIndex) {
				changeDirFlag++;
				fprintf(stderr,"Too many arguments to chdir\n");
			}
		}

		if(strcmp(newargv[count],"cd") == 0) {
			changeDirFlag = 1;
			nextCdArg = 0;
		}

		if(strcmp(newargv[count],"environ") == 0) {
			environFlag = 1;
		}

		if(strcmp(newargv[count],"cat") == 0) {
			catCommandEncountered++;
		}
		if(input_output == 1) {
			strcpy(input_output_filenames[0],word[count]);
			input_output = 0;

		}

		if(input_output ==2) {

			strcpy(input_output_filenames[1],word[count]);
			input_output = 0;
		}
		if(strcmp(newargv[count],"<") == 0 || strcmp(newargv[count],">") == 0){
			wordCounter = 0;
		}
		if(strcmp(newargv[count],"|") == 0) {
			pipeIndexFinder = 1;
			pipeFlag++;
			pipeReadIndex = count + 1;

			if(pipeFlag == 1 && pipeIndexFinder == 1 && input_redirect_flags !=0 && wordCounter >= 2) {

				if(catCommandEncountered == 0){
					index1 = initIndex;
					index2 = count + 1;
					pipeIndexFinder = 0;

				}

				else if(catCommandEncountered != 0) {
					index1 = initIndex;
					index2 = count + 1;
					pipeIndexFinder = 0;
				}

			} else if(pipeFlag == 1 && pipeIndexFinder == 1 && input_redirect_flags == 0) {//no need to make linecounter zero because it already zero.
				if(catCommandEncountered == 0) {
					index1 = initIndex;
					index2 = count + 1;
					pipeIndexFinder = 0;
				}
				else if(catCommandEncountered != 0) {

					index1 = initIndex;
					index2 = count + 1;
					pipeIndexFinder = 0;
				}
			}

			if(pipeFlag == 2 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index3 = count + 1;

			} else if(pipeFlag == 2 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index3 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}

			if(pipeFlag == 3 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index4 = count + 1;

			} else if(pipeFlag == 3 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index4 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 4 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index5 = count + 1;

			} else if(pipeFlag == 4 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index5 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 5 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index6 = count + 1;

			} else if(pipeFlag == 5 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index6 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 6 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index7 = count + 1;

			} else if(pipeFlag == 6 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index7 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 7 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index8 = count + 1;

			} else if(pipeFlag == 7 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index8 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 8 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index9 = count + 1;

			} else if(pipeFlag == 8 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index9 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 9 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index10 = count + 1;

			} else if(pipeFlag == 9 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index10 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
			if(pipeFlag == 10 && pipeIndexFinder == 1 && input_redirect_flags == 0) {
				pipeIndexFinder = 0;
				lineCounter = 0;
				index11 = count + 1;

			} else if(pipeFlag == 10 && pipeIndexFinder == 1 && input_redirect_flags != 0 && wordCounter >= 2) {
				index11 = count + 1;
				pipeIndexFinder = 0;
				lineCounter = 0;
			}
		}

		if(redirecterrorflag == 1) {
			redirecterrorflag =0;
		}

		if(input_redirect_flags == 1|| input_redirect_flags == 2 || input_redirect_flags == 3) {
			increament_counter = 1;
		}
		wordCounter++;
		if(input_redirect_flags != 2 && strcmp(newargv[count],"<") == 0) {
			pipeIndexFinder = 1;
			input_output = 1;
			inputRedirectCount++;
			input_redirect_flags = 1;

			if(increament_counter == 1) {
				increament_counter = 0;
			}
		}

		if(input_redirect_flags == 2 && strcmp(newargv[count],"<") == 0) {
			pipeIndexFinder = 1;
			input_output = 1;
			inputRedirectCount++;
			input_redirect_flags = 3;
			if(increament_counter == 1) {
				increament_counter = 0;
			}
		}

		if(input_redirect_flags!= 1 && strcmp(newargv[count],">") == 0) {
			pipeIndexFinder = 1;
			input_output = 2;
			ouputRedirectCount++;
			input_redirect_flags = 2;
			if(increament_counter == 1) {
				increament_counter = 0;
			}
		}

		if(input_redirect_flags == 1 && strcmp(newargv[count],">") == 0) {
			pipeIndexFinder = 1;
			input_output = 2;
			ouputRedirectCount++;
			input_redirect_flags = 3;
			if(increament_counter == 1) {
				increament_counter = 0;
			}
		}

		if(flags == 1) {
			flags = 0;
		}
		if(c == 0) {

			if(doubleLessthan > 0) {

				if(strcmp(patternToSearch[0],"") == 0) {
					fprintf(stderr,"Missing name for redirect\n");

				} else if(counterDoubleLessThan > 1) {
					fprintf(stderr,"more than << found: ambiguous input redirect.\n");

				} else if(doubleLessthan > 0 && (input_redirect_flags == 1 || input_redirect_flags ==3)) {
					fprintf(stderr,"ambiguous input redirect.\n");

				} else if((input_redirect_flags == 2 || input_redirect_flags == 3)  && strcmp(input_output_filenames[1],"") == 0){
					fprintf(stderr,"Missing name for redirect\n");

				}else if(strcmp(newargv[initIndex],"") == 0 ) {
					fprintf(stderr,"Invalid null command\n");

				} else if(pipeFlag != 0) {
					if(strcmp(newargv[pipeReadIndex],"") == 0) {
						fprintf(stderr,"Invalid null command\n");

					}else {
						newargv[count] = NULL;
						count++;

						pipeRedirect(newargv);
						break;
					}
				}
				else {
					newargv[count] = NULL;
					count++;

					doubleRedirection(newargv);
					break;
				}
				newargv[count] = NULL;
				count++;

				break;
			}

			if(pipeFlag != 0) {

				if(newargv[count-1] != NULL && strcmp(newargv[count -1],"&" ) == 0 && backslashFlag == 0) {
					newargv[count-1] = NULL;
					pipeBackProcess = 1;
				} else if(strcmp(newargv[initIndex],"") == 0 || strcmp(newargv[pipeReadIndex],"") == 0) {
					fprintf(stderr,"Invalid null command\n");
					newargv[count] = NULL;
					count++;
					break;
				}
				newargv[count] = NULL;
				count++;
				if(input_redirect_flags == 1 || input_redirect_flags == 3) {
					if(strcmp(input_output_filenames[0],"") == 0) {
						fprintf(stderr,"Missing name for redirect\n");
						break;
					}
				}
				if(input_redirect_flags == 2 || input_redirect_flags == 3) {
					if(strcmp(input_output_filenames[1],"") == 0) {
						fprintf(stderr,"Missing name for redirect\n");
						break;
					}
				}

				pipeRedirect(newargv);
				break;
			}


			if(input_redirect_flags == 1 || input_redirect_flags ==2 || input_redirect_flags ==3) {

				if(count != initIndex && strcmp(newargv[count-1],"&") == 0 && backslashFlag == 0) {
					input_redirect_back = 1;
					newargv[count -1] = NULL;
				} else if(strcmp(newargv[initIndex],"") == 0) {
					fprintf(stderr,"Invalid null command\n");
					newargv[count] = NULL;
					count++;
					break;
				} else {
					newargv[count] = NULL;
					count++;
				}
				if(input_redirect_flags == 1 || input_redirect_flags == 3) {
					if(strcmp(input_output_filenames[0],"") == 0) {
						fprintf(stderr,"Missing name for redirect\n");
						break;
					}
				}
				if(input_redirect_flags == 2 || input_redirect_flags == 3) {
					if(strcmp(input_output_filenames[1],"") == 0) {
						fprintf(stderr,"Missing name for redirect\n");
						break;
					}
				}

				input_redirect(newargv);
				break;
			}

			if(strcmp(newargv[count-1],"&") == 0 && backslashFlag == 0 && environFlag == 0 && changeDirFlag == 0) {
				backgroundProcess(newargv);
				break;
			}
			else {
				if(changeDirFlag == 1) {
					changeDirFlag = 0;
				}
				newargv[count] = NULL;
				count++;
				break;
			}
		}
		if(((increament_counter == 1  && wordCounter > 2)|| (flags != 1 && input_redirect_flags != 1 && input_redirect_flags != 2 && input_redirect_flags != 3))&& (doubleLessthan == 0 || doubleLessthan > 3)) {
			if(lineCounter == 0 && pipeFlag != 0) {
				if(initIndex == count) {
					fprintf(stderr,"Invalid null command\n");
					nullCommandFound = 1;
					newargv[count] = NULL;
					count++;
					break;

				} else {
					newargv[count] = NULL;
					lineCounter = 1;
				}
			}
			count++;
		}
	}
	if(doubleLessthan == 0 && tildeFlag != 1 && wrongEnvVal == 0 && environFlag == 0 && changeDirFlag == 0 && pipeFlag == 0 && flags != 2 && flags !=1 && input_redirect_flags !=1 &&input_redirect_flags != 2 && input_redirect_flags != 3) {
		execWithArgs(newargv);
	}
}

/*
 * This method is for handling the signal.
 */
void signalHandler (int signum) {
}

/*
 * This method continuously printing the prompt and calling parse method until it receives EOF.
 */
int main(void) {
	char newChar;

	setpgid(0,0);
	(void)signal(SIGTERM,signalHandler);

	for(;;) {
		printf("%s:570: ",str);
		fflush(stdout);

		if(nullCommandFound == 1) {
			while((newChar = getchar()) != '\n') {
			}
		}

		while((newChar = getchar()) == '\n') {
			printf("%s:570: ",str);
			fflush(stdout);
		}

		if(doubleLessthan > 0 && strcmp(patternToSearch[0],"") != 0 && counterDoubleLessThan <= 1 && (input_redirect_flags != 3|| input_redirect_flags != 1)&& strcmp(input_output_filenames[1],"") != 0 && input_redirect_flags == 2) {
			while(newChar != EOF) {
				newChar = getchar();
			}
		}
		if(newChar == EOF) {
			break;
		}
		else {
			ungetc(newChar,stdin);
		}
		parse();
	}

	killpg(getpgrp(), SIGTERM);
	printf("p2 terminated.\n");
	fflush(stdout);
	exit(0);
}
