#include "pch.h"
#include <iostream>

int main()
{
    std::cout << "Hello World!\n"; 
	int c;
	//char *input = argv[1];
	FILE *input_file;

	input_file = fopen("c:\\Users\\v-masebo\\Desktop\\pg673.txt", "r");

	if (input_file == 0)
	{
		//fopen returns 0, the NULL pointer, on failure
		perror("Canot open input file\n");
		exit(-1);
	}
	else
	{
		int found_word = 0;
		int num_words = 0;
		char header_word[2];
		header_word[0] = 'h';
		header_word[1] = '1';
		char header_def[3];
		header_def[0] = 'd';
		header_def[1] = 'e';
		header_def[2] = 'f';
		char read_word[2];
		char read_def[3];

		//while ((c =fgetc(input_file)) != EOF )
		while (num_words < 100)
		{
			c = fgetc(input_file);

			if (c == '<')
			{
				c = fgetc(input_file);
				if (c == 'h')
					//fgets(read_word, 2, input_file);
					//putchar(read_word[0]);
					//putchar(read_word[1]);
					//if (strcmp(header_word, read_word) == 0)
				{
					putchar('$');
					c = fgetc(input_file);
					if (c == '1')
					{
						//word found...
						while ((c = fgetc(input_file)) != EOF)
						{
							if (c == '<')
								break;
							else
								putchar(c);
						}
						putchar('\n');

						while (((c = fgetc(input_file)) != EOF) && (found_word == 0))
						{
							if (c == '<')
							{
								c = fgetc(input_file);
								if (c == 'd')
								{
									putchar('#');
									c = fgetc(input_file);
									if (c == 'e')
									{
										c = fgetc(input_file);
										if (c == 'f')
										{

											//fgets(read_def, 3, input_file);
											//if (strcmp(header_def, read_def) == 0)
											//{
												//def found...
											while ((c = fgetc(input_file)) != EOF)
											{
												if (c == '<')
													break;
												else
													putchar(c);
											}
											putchar('\n');
											putchar('\n');
											num_words = num_words + 1;
											found_word = 1;
										}
									}
								}
							}
							
						}
						found_word = 0;
					}
				}
			}
		}
		//}
		//c = fgetc(input_file);
		//if (c == 'h')
		//{
		//	c = fgetc(input_file);
		//	if (c == '
		//if it's an alpha, convert it to lower case
		//if (isalpha(c))
	   // {
		 //   found_word = 1;
		   // c = tolower(c);
		   // putchar(c);
		//}
		//else {
		  //  if (found_word) {
			//    putchar('\n');
			  //  found_word=0;
			//}
		//}

   // }
	}

	fclose(input_file);

	printf("\n");

	return 0;
}