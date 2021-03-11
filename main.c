#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

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
					printf("packing, optind = %d\n", optind);
					pack(argv, argc, optind);
					break;
				}
			case 'u': //unpack, после него один файл - архив для распаковки
				{
					printf("unpacking, optarg = %s\n", optarg);
					unpack(optarg);
					break;
				}
			case '?':
				{
					printf("Invalig input format\n");
				}
		}
	}
	return 0;
}


//Формат файла архива: 
//первые 8 байт - число, sizeof(структуры дескриптора для этого файла)
//далее записывается этот дескриптор
//далее идут файлы подряд в порядке, указанном в дескрипторе

//struct arch_descriptor
//{
//	int n; //число файлов
//	int **file_sizes; //размеры файлов	
//	char **file_names; //имена файлов
//};

struct file_descr
{
	size_t file_size; //размер файла
	char file_name[80]; //имя файла
	//TODO: maybe the file itself?
};

void unpack(char *file_name)
{
	//Debug
	printf("Within unpacking function\n");
}

void pack(char *file_names_arr[], int length, int start_index)
{
	//Debug
	printf("In the packing function now.\n");

	int max_file_count = 100;
	int file_count = 0;

	//Creating archive file
	int arch = open("test_rachive.tar", O_WRONLY | O_CREAT);
	if(arch == -1)
	{
		printf("Couldn't create or open archive!");
		break;
	}

	write(

	//Creating descriptor and filling archive
	struct stat file_stat;
	for(int i = start_index; i < length; i++)
	{
		int e = stat(file_names_arr[i], &file_stat); //TODO: stat is slow! Change?
		if(e == -1) 
		{
			printf("Some error has occured! %s not found!\n", file_names_arr[i]);
		}
			
		if(S_ISREG(file_stat.st_mode)) //если это файл
		{	
			//int file = open(file_names_arr[i], O_RDONLY); //TODO: catch error
			printf("File name = %s, file size = %ld\n", file_names_arr[i], file_stat.st_size);
			
		}
		else if(S_ISDIR(file_stat.st_mode)) //если это директория
		{
			printf("%s is a directory! AAAAAAAAAAAAAAAAAAAAAAAAAA!!!\n", file_names_arr[i]);
		}
	}
}
