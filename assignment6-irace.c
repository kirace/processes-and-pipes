/*
	Kevin Irace
	Prof. Signorile (T Th 1:30pm)
	April 14, 2015
	Assignment 6
	file: assignment6-irace.c

*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#define MAXLINE 100
#define TRUE 1
#define FALSE 0

void process1();
void process2();
void process3();
int test_eol(char a);
int test_eof(char a);
char * returnLine(FILE *f);
char * changeCaps(char * a);
int fd1[2];//p1 -> p2
int fd2[2];//p2 -> p1
int fd3[2];//p2 -> p3
int fd4[2];//p3 -> p2
int fd5[2];// END OF FILE MESSAGE???

int main(void)
{
	int n,s, status1, status2, status3;
	pid_t pid1;
	pid_t pid2;
	pid_t pid3;
	
	int i;
	if (pipe(fd1) < 0)
		fprintf(stderr, "pipe error \n");
	if (pipe(fd2) < 0)
		fprintf(stderr, "pipe error \n");
	if (pipe(fd3) < 0)
		fprintf(stderr, "pipe error \n");
	if (pipe(fd4) < 0)
		fprintf(stderr, "pipe error \n");

	pid1 = fork();
	if (pid1 == 0){
		puts("create process 1");
		process1();
		exit(0);
	}
	pid2 = fork();
	if (pid2 == 0){
		puts("create process 2");
		process2();
		exit(0);
	}
	pid3 = fork();
	if (pid3 == 0){
		puts("create process 3");
		process3();
		exit(0);
	}
	waitpid(pid1, &status1, 0); //?????
	waitpid(pid2, &status2, 0);
	waitpid(pid3, &status3, 0);
	//sleep(20);
	printf("All processes complete! Output written to 'mobydicknew.txt'\n");
	return 0;
}
void process1(){ //reads in file
	
	FILE *fp;
	int n;
	char text[MAXLINE];
	char feedback[10];
	char * currentLine;

	fp = fopen("mobydick.txt" , "r");
        
	puts("start process 1");
	
	currentLine = returnLine(fp);
	if(currentLine == NULL){
		printf("Error: Empty File.");
		return;
	}
	while((currentLine != NULL)){
		strcpy(text,currentLine);
		write(fd1[1],text, MAXLINE);
		n = read(fd2[0],feedback, 10); 
		//printf("process 2 feedback received %d: \n",n);
		currentLine = returnLine(fp);
	}
	write(fd1[1], "stop", MAXLINE);
	fclose(fp);
	puts("end process 1");
	exit(0);
}

void process2(){ //changes the case of the text
	char oLine[MAXLINE],nLine[MAXLINE];
	char * newLine;
	int n1, n2;
	char feedback[10];

	puts("start process 2");

	//n1 = read(fd1[0], oLine, MAXLINE);
	//write(fd2[1], "hi", 10);
	//printf("%i\n", n1);
	//printf("process 1 text line received %d: \n",n1);
	while((n1 = read(fd1[0], oLine, MAXLINE)) != 0){
	    	if(strcmp("stop", oLine)==0){
		   write(fd3[1], "stop", MAXLINE);
		   n2 = read(fd4[0], feedback, MAXLINE);
		   exit(0);
		}
		newLine = changeCaps(oLine);
		strcpy(nLine, newLine);
		n2 = read(fd4[0], feedback, 10);
		write(fd3[1], nLine, MAXLINE);
		write(fd2[1], "hi", 10);
		//printf("process 3 feedback received %d: \n",n2);
	}
	puts("end process 2");
	exit(0);
}

void process3(){ //writes to file
	FILE *fp2;
	int n;
	char text[MAXLINE];
	char feedback[10];

	fp2 = fopen("mobydicknew.txt", "w");

	puts("start process 3");

	//n = read(fd3[0], text, MAXLINE);
	//printf("process 2 text line received %d: \n",n);
	write(fd4[1], "hi", 10);
	while((n = read(fd3[0], text, MAXLINE)) != 0){
		//printf("process 2 text line received %d: \n",n);
		if(strcmp("stop", text) == 0){
			fclose(fp2);
			exit(0);
		}
		//printf("%s\n", text);
		fprintf(fp2, "%s\n", text);
		write(fd4[1], "hi", 10);
	}
	puts("end process 3");
	exit(0);
}

/*---------------------- HELPER FUNCTIONS --------------------------------------------------------*/

char * returnLine(FILE *fp){  //returns line or NULL (if at end of file)
  char c;
  char *line;
  int size=0;
  line = NULL;
  line = realloc(line,sizeof(char));
  c = fgetc(fp);  //read in first char of first line of the file
  if (test_eof(c))  //at end of the file
    return NULL;
  while (!test_eol(c)){//read the line
    *(line+size) = c;
    size++;
    line = realloc(line,sizeof(char)* (size+1));
    c = fgetc(fp);
  }
  *(line+size) = '\0';
  return line;
}

char * changeCaps(char *l){// switches the case of each letter
  int x = 0;
  char *line;
  line = l;
  
  while(*(line + x) != '\0'){
	if( *(line + x) >= 'A' && *(line + x) <= 'Z'){
		*(line + x) = tolower(*(line + x));
	}
	else if( *(line + x) >= 'a' && *(line + x) <= 'z'){
		*(line + x) = toupper(*(line + x));
	}
	x++;
  }
  
  return line;
}
   

int test_eol(char a){//checks if end of line
  if (a == '\n')
    return TRUE;
  else
    return FALSE;
}

int test_eof(char a){//checks if end of file
  if (a == EOF)
    return TRUE;
  else
    return FALSE;
}
