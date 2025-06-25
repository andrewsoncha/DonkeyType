#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define MX 500
#define COLOR_MX 3

char colorCodes[COLOR_MX][10] = {KNRM, KBLU, KRED}; 
void clearScreen(){
    printf("\033[H\033[J");
}

void printChar(char **lineArr, int lineN, int* lineLen, char *userInputArr, int *colorArr, int userLineI, int userLength){
	printf("printChar: %d %d\n",userLineI, userLength);
	int currentColor = 0;

	printf("%s", KWHT);
	if(userLineI>0){
		printf("%s\n\n",lineArr[userLineI-1]);
	}

	printf("%s",KNRM);
	if(userLength < lineLen[userLineI]){
		for(int i=0;i<lineLen[userLineI];i++){
			if(i==userLength){
				printf("%s",KGRN);
				printf("%c",lineArr[userLineI][i]);
				printf("%s",KNRM);
			}
			else{
				printf("%c",lineArr[userLineI][i]);
			}
		}
	}
	printf("\n");

	for(int i=0;i<=userLength;i++){
		//printf("i:%d  colorArr:%d\n",i,colorArr[i]);
		if(currentColor != colorArr[i]){
			//printf("in if!\n");
			currentColor = colorArr[i];
			//printf("setting to color: %d\n",currentColor);
			printf("%s", colorCodes[currentColor]);
		}
		printf("%c", userInputArr[i]);
	}
	printf("\n");

	printf("%s", KWHT);
	if(userLineI<lineN-1){
		printf("%s\n\n",lineArr[userLineI+1]);
	}
	return;
}


char* readFromFile(char* filePath, int readOption, long *size){
	FILE *fp;
	if(readOption==0){
		fp = fopen(filePath, "r");
		printf("opening filePath: %s   r\n",filePath);
	}
	else{
		fp = fopen(filePath, "rb");
		printf("opening filePath: %s    rb\n",filePath);
	}
	if(fp==NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);
	printf("size: %ld\n",*size);
	fseek(fp, 0, SEEK_SET);
	char *fcontent = malloc(*size+2);
	printf("fread started!\n");
	fread(fcontent, 1, *size,  fp);
	printf("fread ended!\n");
	//printf("fcontent: %s\n",fcontent);
	strcat(fcontent, "\01"); // adding \1 to indicate end of file/string when decoding
	*size++; // Adding 1 to strLen as the Last \0 won't be counted by strlen later
	fclose(fp);
	return fcontent;
}

char** splitByLine(char* wholeStr, int wholeStrLen, int* lineN, int** lineLen){
	char** lineArr;
	int localLineN = 0;
	for(int i=0;i<wholeStrLen;i++){
		if(wholeStr[i]=='\n'){
			localLineN++;
		}
	}
	*lineN = localLineN;
	int* localLineLen = malloc(sizeof(int)*localLineN);
	lineArr = malloc(sizeof(char*)*localLineN);
	int lineI = 0;
	int lineLenI = 0;
	for(int i=0;i<wholeStrLen;i++){
		if(wholeStr[i]=='\n'){
			lineArr[lineI] = malloc(sizeof(char)*(lineLenI+1));
			memset(lineArr[lineI], 0, lineLenI+1);
			strncpy(lineArr[lineI], wholeStr+i-lineLenI, lineLenI);
			localLineLen[lineI] = lineLenI;
			lineI++;
			lineLenI = 0;
		}
		lineLenI++;
	}
	*lineLen = localLineLen;
	return lineArr;
}



int main(int argc, char* argv[])
{


    char *charArr;
    char *userInputArr;
    int *colorArr;
    long length = 0;
    int escapeFlag = 0;

    if(argc<2){
	    printf("Usage: typing_game [FILE_PATH]\n");
	    return 1;
    }

    charArr = readFromFile(argv[1], 0, &length);

    char **splitText;
    int lineN;
    int* lineLen;
    splitText = splitByLine(charArr, (int)length, &lineN, &lineLen);
    printf("lineN: %d\n",lineN);
    for(int i=0;i<lineN;i++){
	   printf("lineLen[%d]: %d\n",i,lineLen[i]);
	   printf("splitText[%d]: %s\n\n",i,splitText[i]);
    }

    int maxLineLen = 0;
    for(int i=0;i<lineN;i++){
	    if(maxLineLen<lineLen[i]){
		    maxLineLen = lineLen[i];
	    }
    }
    printf("maxLineLen: %d\n",maxLineLen);
    userInputArr = malloc(maxLineLen);

    memset(userInputArr, 0, maxLineLen);
    userInputArr[0] = "_";
    colorArr = malloc(sizeof(int)*maxLineLen);
    for(int i=0;i<maxLineLen;i++){
	    colorArr[i] = 0;
    }

    int correctN = 0;

    printf("length: %d\n", length);
    int lineI = 0;
    int cursorI = 0;

    printf("Typing game! Press any key to start!\n");
    system("stty -echo");
    system("stty raw");
    getc(stdin);
    system("stty cooked");
    system("stty echo");

    clock_t start = clock();

    clearScreen();

    printChar(splitText, lineN, lineLen, userInputArr, colorArr, lineI, cursorI+1);

    while(lineI < lineN){
	    system("stty -echo");
	    system("stty raw");
	    char charInput = getc(stdin);
	    system("stty cooked");
	    system("stty echo");
	    clearScreen();
	    printf("entered char:%d  %c   correctInput:%d %c\n", charInput, charInput, splitText[lineI][cursorI], splitText[lineI][cursorI]);

	    if(charInput!=8){
		    if(charInput==splitText[lineI][cursorI]){
			    colorArr[cursorI] = 1;
			    correctN++;
		    }
		    else{
			    colorArr[cursorI] = 2;
		    }
	    }
	    if(charInput==127){ //backspace key
		    if(cursorI>0){
			    cursorI-=2;
		    }
	    }
	    if(charInput == '\n'){
		    clearScreen();
		    printf("You pressed line break! You FUCKING IDIOT!\n");
		    
		    memset(userInputArr, 0, maxLineLen);
		    lineI++;
		    cursorI = 0;
	    }

	    if(charInput == 27){ //escape key
		    escapeFlag = 1;
		    break;
	    }

	    userInputArr[cursorI] = charInput;
	    userInputArr[cursorI+1] = '_';

	    printf("starting printChar\n");
	    printChar(splitText, lineN, lineLen, userInputArr, colorArr, lineI, cursorI+1);
	    cursorI++;

	    if(cursorI >= lineLen[lineI]){
		    memset(userInputArr, 0, maxLineLen);
		    lineI++;
		    cursorI = 0;
	    }
    }
    clock_t end = clock();
    if(escapeFlag==0){
	    printf("Correct: %d   Incorrect: %d   Accuracy: %d.%d%%\n", correctN, length - correctN, (correctN*100/length), (correctN*10000/length)%100);
	    printf("Time: %.3lf Seconds\n", (double)(end - start)/1000);
    }
    else{
	    printf("Game escaped!\n");
    }
    free(charArr);
    for(int i=0;i<lineN;i++){
	    free(splitText[i]);
    }
    free(splitText);
    free(lineLen);
    free(userInputArr);
    free(colorArr);
    return 0;
}
