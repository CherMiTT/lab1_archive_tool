#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

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

struct file_descr
{
	size_t file_size; //размер файла
	char file_name[80]; //имя файла
	//TODO: maybe the file itself?
};

void unpack(char *file_name)
{
	int arch = open(file_name, O_RDONLY);
	if (arch == -1)
	{
		printf("Some error has occured! Can't open archive.\n");
		return;
	}

	int file_count;
	read(arch, &file_count, sizeof(int));
	struct file_descr descr[file_count];

	for (int i = 0; i< file_count; i++)
	{
		if(read(arch, &descr[i],sizeof(struct file_descr))!=sizeof(struct file_descr))
		{
			printf("Incorrect file reading");
		}
		printf("File name: %s, file size = %ld\n", descr[i].file_name, descr[i].file_size);
	}

	for (int i = 0; i< file_count; i++)
	{
		char block[descr[i].file_size]; //TODO: разбить на блоки по 1024
		if(read(arch, &block,descr[i].file_size)!=descr[i].file_size)
		{
			printf("Incorrect file reading");
		}
		int file = open(descr[i].file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
		write(file, &block, descr[i].file_size);
		close(file);
	}

	close(arch);
}

void pack(char *file_names_arr[], int length, int start_index)
{
	//Debug
	printf("In the packing function now.\n");

	int max_file_count = 100;
	int file_count = 0;

	//Creating archive file
	int arch = open("test_archive.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
	if(arch == -1)
	{
		printf("Couldn't create or open archive!");
		return;
	}

	write(arch, &max_file_count, sizeof(int));

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
			struct file_descr descr;
			descr.file_size = file_stat.st_size;
			strcpy(descr.file_name, file_names_arr[i]);
			if(write(arch, &descr, sizeof(struct file_descr))!=sizeof(struct file_descr))
			{
				printf("Incorrect writing\n");	
			}
			file_count++;
			
		}
		else if(S_ISDIR(file_stat.st_mode)) //если это директория
		{
			printf("%s is a directory! AAAAAAAAAAAAAAAAAAAAAAAAAA!!!\n", file_names_arr[i]);
		}
	}

	for(int i = start_index; i < length; i++)
	{
		int nread;
		char block[1024];
		int e = stat(file_names_arr[i], &file_stat); //TODO: stat is slow! Change?
		if(e == -1) 
		{
			printf("Some error has occured! %s not found!\n", file_names_arr[i]);
		}
			
		if(S_ISREG(file_stat.st_mode)) //если это файл
		{	
			int file = open(file_names_arr[i], O_RDONLY); //TODO: catch error
			if (file == -1)
			{
				printf("Some error has occured! Can't open file.\n");
				return;
			}
			while((nread = read(file, block, sizeof(block))) > 0)
			{
				write(arch, block, nread);
				printf("%s",block);
			}
			close(file);
			
		}
		else if(S_ISDIR(file_stat.st_mode)) //если это директория
		{
			printf("%s is a directory! AAAAAAAAAAAAAAAAAAAAAAAAAA!!!\n", file_names_arr[i]);
		}
	}

	lseek(arch,SEEK_SET, 0); //TODO: проверить
	write(arch, &file_count, sizeof(int));

	close(arch);
}
