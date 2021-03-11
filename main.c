#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void pack(char **file_names_arr);
void unpack(char *file_name);

//Программа архивирует файлы без сжатия

int main(int argc, char* argv[])
{
	//Парсим аршументы
	int opt;
	while ((opt = getopt(argc, argv, "pu:")) != -1)
	{
		switch(opt)
		{
			case 'p': //pack, после него идёт список файлов для архивации
				{
					printf("\npacking, optind = %d\n", optind);
					break;
				}
			case 'u': //unpack, после него один файл - архив для распаковки
				{
					printf("\nunpacking, optarg = %s\n", optarg);
					unpack(optarg);
					break;
				}
		}
	}
	return 0;
}

void unpack(char *file_name)
{
	printf("\nWithin unpacking function\n");
}

void pack(char **file_names_arr)
{

}
