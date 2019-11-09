
#include <stdio.h>
#include <stdlib.h>

int main ()
{
  FILE * pFile;
  pFile = fopen ("myfile.txt","w");
  if (pFile!=NULL)
  {
    fputs ("fopen example",pFile);
    fclose (pFile);
  }

  FILE * inFile = fopen ("link_test.txt", "r");  //rb?
  FILE * outFile = fopen ("links.htm", "w");
  FILE * testFile = fopen ("links2.htm", "w");
  char *code = malloc(1000);
  size_t n = 0;
  int c;

  while ((c = fgetc(inFile)) != EOF)
  {
    //if ((char) c == '[')
	//code[n++] = '<
    code[n++] = (char) c;
    code[n++] = 'a';
  }
char array[] = "YOUR TEXT HERE";

    // don't forget to terminate with the null character
    code[n] = '\0';
fputs (code, outFile);
fputs (array, testFile);
fputs ("another", testFile);
fclose(inFile);
fclose(outFile);
fclose(testFile);
  //fseek(f, 0, SEEK_END);
  //long fsize = ftell(f);


    //if (file == NULL)
        //return NULL; //could not open file        

  return 0;
}


//fseek(f, 0, SEEK_SET);  //same as rewind(f);

//char *string = malloc(fsize + 1);
//fread(string, fsize, 1, f);
//fclose(f);

//string[fsize] = 0;