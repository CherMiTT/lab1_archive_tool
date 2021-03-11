#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

void pack(char *file_names_arr[], int length, int start_index);
void unpack(char *file_name);

//Программа архивирует файлы без сжатия

int main(int argc, char* argv[])
{
	//Парсим аргументы
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


//Формат файла архива: 
//первые 8 байт - число, sizeof(структуры дескриптора для этого файла)
//далее записывается этот дескриптор
//далее идут файлы подряд в порядке, указанном в дескрипторе

struct arch_descriptor
{
	int n; //число файлов
	char **file_names; //имена файлов
	int **file_sizes; //размеры файлов
};

void unpack(char *file_name)
{
	//Debug
	printf("\nWithin unpacking function");
}

void pack(char *file_names_arr[], int length, int start_index)
{
	//Debug
	printf("\nIn the packing function now.");
	printf("\nFile names:");
	for(int i = start_index; i < length; i++)
	{
		printf("\n%s", file_names_arr[i]);
	}

	//Creating archive file
	int arch = open("test_rachive.tar", O_WRONLY | O_CREAT);


	//Creating descriptor
	for(int i = start_index; i < length; i++)
	{

	}
}
