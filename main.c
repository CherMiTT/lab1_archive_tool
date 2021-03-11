#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void pack(char *file_names_arr[], int length, int start_index);
void unpack(char *file_name);

//Программа архивирует файлы без сжатия

int main(int argc, char* argv[])
{
	for(int i = 0; i < argc; i++)
	{
		printf("i = %d: %s", i, argv[i]);
	}

	//Парсим аршументы
	int opt;
	while ((opt = getopt(argc, argv, "pu:")) != -1)
	{
		switch(opt)
		{
			case 'p': //pack, после него идёт список файлов для архивации
				{
					printf("\npacking, optind = %d", optind);
					pack(argv, argc, optind);
					break;
				}
			case 'u': //unpack, после него один файл - архив для распаковки
				{
					printf("\nunpacking, optarg = %s", optarg);
					unpack(optarg);
					break;
				}
			case '?':
				{
					printf("\nInvalig input format");
				}
		}
	}
	return 0;
}

void unpack(char *file_name)
{
	printf("\nWithin unpacking function");
}

void pack(char *file_names_arr[], int length, int start_index)
{
	printf("\nIn the packing function now.");
	printf("\nFile names:");
	for(int i = start_index; i < length; i++)
	{
		printf("\n%s", file_names_arr[i]);
	}
}
