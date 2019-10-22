/* Student Name: Sanchita Kanade
 * Class: CS570
 * Due Date: 11/28/2018
 * File Name:p2.c
 * Assignment: program4
 */


#include "getword.h"
#include "p2.h"

/*
 * getword() gets one word(a string) from the input stream then it stores that word into the storage array
 * and returns an integer value which usually indicates length of the string provided by the stdin.
 * The return value can be -255, 0 or negative string length depending upon following conditions:
 * If EOF is encountered then return value will be -255 and getword() will leave an empty string into the storage array.
 * If a newline or a semicolon encountered then return value will be 0 and getword() will leave an empty string into the storage array.
 * If '$' is the first character of a word then it will not be considered part of the word
 * and getword() will put remaining string into the storage array, in this case return value will be
 * negative length of the string stored in the array.
 * If '~' is the first character of a word then getword() will put a string representing the path to user's home diretory into the storage array,
 * after that getword() will append remaining characters of the word to the storage array and return length of the string stored in the array.
 * Metacharacters ('<', '>', '|', '&' , '<<' and ';'), space, newline and EOF are act as a delimiter for a word.
 * If any metacharacter (i.e. '<', '>', '|', '&' or '<<') is encountered at the begining of a word
 * then getword() will construct a string containing one metacharacter, store it into the array
 * and return length of the string stored in the array.
 * There is one exception to the above rule: if backslash precedes a metacharacter then that metacharacter will not act as a word delimiter
 * instead it will be considered part of a word, in this case getword() will add whole word into the array except backslash character and
 * return length of the word stored in the array.
 *
 */
int getword(char *w) {
	int i;
	char *p;
	int newChar = 0;
	int wordSize = 0;
	int multiplier = 1;

	*w = 0;

	/* code provided below reads the next character from input stream using getchar() and
	 * populates the array with that character by using the provided pointer(w) which points to begining of an array
	 * until the word size is larger than STORAGE-1 or until EOF is encountered.
	 */
	while(wordSize != STORAGE-1 && (newChar = getchar()) != EOF) {

		if(wordSize == 0) {

			/*
			 * following while loop skips leading white spaces at the begining of the word with the help of getchar().
			 * getchar() function reads and returns next character until it skips all the leading spaces
			 * at the begining of the word.
			 */
			while(newChar == ' ') {
				newChar = getchar();
			}

			if(newChar == EOF) {
				return -255;
			}

			/*
			 *  following if statement populates the storage array with a string representing the path to user's home diretory
			 *  using getenv() function when it encounters metacharacter '~' at the begining of a word.
			 *  getenv() fuction searches for the environmental variable 'HOME' and
			 *  if it exists return a pointer to the value of the 'HOME', which is a string
			 *  containing path to the user's home directory.
			 */
			if((newChar == '~')) {

				if((newChar = getchar()) == ' ' || newChar =='\n' || newChar == EOF || newChar == '<' || newChar == '>' ||newChar == '|' || newChar =='&'|| newChar == ';' || newChar == '$') {
					ungetc(newChar, stdin);
					p = getenv("HOME");
					for(i = 0; p[i]; i++) {
						*w = p[i];
						wordSize++;
						w++;
					}
				} else {
					// this flag will set if '~' is the first character of the word.
					tildeFlag = 1;
					ungetc(newChar, stdin);
					*w = '~';
					wordSize++;
					w++;
				}
				newChar = getchar();
				if(newChar == EOF) {
					*w = '\0';
					return wordSize * multiplier;
				} else if(newChar == '\n'){
					ungetc(newChar,stdin);
					*w = '\0';
					return wordSize * multiplier;
				}
			}

			/*
			 * following if statement constructs a string containing one metacharacter and
			 * put that string into the array when any metacharacter appears at the begining of a word.
			 * metacharacter may be '<' or '<<' or '|' or '&' or'>'
			 *
			 */
			if(newChar == '<' || newChar == '>' || newChar == '|' || newChar == '&')  {
				*w = newChar;
				wordSize++;
				w++;

				if(newChar == '<') {

					/*
					 * if the metacharacter '<' is appeared at the begining of a word then following code
					 * will check whether the next character is again the same character '<' or not.
					 * if the next character is same as before then it will be added to the array and
					 * and string length will be returned.
					 * Otherwise, that character will be pushed back into the input stream using ungetc()
					 * so that we can read the character again in next call to getword() and
					 * then only one metacharacter '<' will be stored into the storage array and
					 * method will return string length 1.
					 */
					newChar = getchar();
					if(newChar == '<') {

						//setting a flag to indicate that double less than appeared
						doubleLessthan = 1;
						*w = newChar;
						wordSize++;
						w++;
					}
					else {
						ungetc(newChar, stdin);
					}
				}
				*w = '\0';
				return wordSize * multiplier;
			}

			/*
			 * if a semicolon or a newline encountered at the begining of a word or after (one or more) blanks then
			 * the following if statement will leave an empty string into the storage array and return string length zero.
			 */
			if(newChar == ';' || newChar == '\n') {
				return 0;
			}

			/* When a metacharacter '$' is encountered at the begining of a word it won't be considered part of the word
			 * and getword() will return negative length of the remaining string.
			 * In order to return a negative string length we have to set up multiplier as -1 if word starts with a metacharacter '$'.
			 * we are using getchar() to skip character '$' and to get a next character.
			 */

			if(newChar == '$') {

				//if there is a EOF right after the '$' character that means length of the string is zero.
				if((newChar = getchar()) == EOF) {
					return 0;
				}
				multiplier = -1;
			}
		}

		//after processing leading white spaces and '$' we save the next character into the character array.
		*w = newChar;

		//following metacharacters ';', '<', '>', '|', '&' , a space and a newline acts as a word delimiter.
		if(newChar == ' '|| newChar == '\n' || newChar == ';' ||newChar == '<' || newChar == '>' || newChar == '|' || newChar == '&') {

			/* we are using ungetc() fuction to read following characters again in the next call to getword().
			 * In the next call to getword() if a newline or a semicolon is encountered
			 * then we will leave an empty string in the storage array and return 0.
			 * If any metacharacter ('<', '>', '|' or '&') is encounterd in the next call to getword(),
			 * then we will put that metacharacter into the storage array so, getword() will construct a string
			 * containing one meta character and then return the string length.
			 */
			if(newChar == '\n' || newChar == ';'|| newChar == '<'|| newChar == '>'|| newChar == '|' || newChar == '&') {

				ungetc(newChar,stdin);

			}
			//following line terminates a string.
			*w = '\0';

			//return negative or positive wordsize depend upon start of the word with character '$'
			return wordSize * multiplier;
		}

		/*
		 * following if statement ignores backslash character using getchar() method
		 * which returns next character in the input stream.
		 * if a backslash '\' precedes any metacharacter then that metacharacter
		 * will be treated as a normal character by the following code
		 * and it will be added into the array.
		 */
		if(newChar == '\\' ) {
			newChar = getchar();
			if(newChar == '&') {
				backslashFlag = 1;
			}
			*w = newChar;

			/*
			 * if a backslash precedes a newline then the following if statement treats a newline like a space.
			 * and terminates the word.
			 */
			if(newChar == '\n') {
				newChar = getchar();
				*w = newChar;
			}

			if(newChar == EOF) {
				*w = '\0';
				return wordSize * multiplier;
			}
		}

		wordSize++;
		w++;
	}

	//following if statement terminates a word if it is longer than the size of the storage array.
	if(wordSize == STORAGE-1) {
		*w = '\0';
		return wordSize * multiplier;
	}

	//following code handles premature EOF
	if(wordSize != 0 && newChar == EOF) {
		/* in case of premature EOF, put null character into the array,
		 * which is used by printf in p1.c to determine the end of the string
		 */
		*w = '\0';

		return 0;
	}

	//return -255 when the program encounters to EOF while the word size is 0.
	return -255;
}


