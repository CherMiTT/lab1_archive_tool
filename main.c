#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

void pack(char *file_names_arr[], int length, int start_index);
void unpack(char *file_name);
void pack_fdescr(int arch, char file_name[80], size_t file_size, int *file_count, int parent_id);
void pack_ddestr(int arch, char file_name[80], int *file_count, int parent_id);
void pack_file(int arch, char file_name[80]);
void unpack_file(int arch, char file_name[80],size_t file_size, int parent_id, int *cur_id, int nested_directory_array[20], int *index);
void pack_dir (int arch, char file_name[80]);
void unpack_dir(int arch, char file_name[80], int id, int *cur_id, int parent_id, int nested_directory_array[20], int *index);

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
					//printf("packing, optind = %d\n", optind);
					pack(argv, argc, optind);
					break;
				}
			case 'u': //unpack, после него один файл - архив для распаковки
				{
					//printf("unpacking, optarg = %s\n", optarg);
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
	bool is_file; //True - файл, false - директория
	int id; //Нумерация начинаетя с 0
	size_t file_size; //размер файла/дирпектории (у директории размер 0)
	char file_name[80]; //имя файла
	int parent_id; //-1 - нет родителя
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

	int cur_id = 0;
	int nested_directory_array[20]; //TODO; redo 20
	int index = 0;

	for (int i = 0; i< file_count; i++)
	{
		if(read(arch, &descr[i],sizeof(struct file_descr))!=sizeof(struct file_descr))
		{
			printf("Incorrect file reading");
		}
		//printf("File name: %s, file size = %ld, file id = %d, parent's id = %d\n", descr[i].file_name, descr[i].file_size, descr[i].id, descr[i].parent_id);
	}

	for (int i = 0; i< file_count; i++)
	{
		if (descr[i].is_file)
		{
			unpack_file(arch, descr[i].file_name, descr[i].file_size, descr[i].parent_id, &cur_id, nested_directory_array, &index); //TODO: current id
		}
		else
		{
			unpack_dir(arch, descr[i].file_name, descr[i].id, &cur_id, descr[i].parent_id, nested_directory_array, &index);
		}
	}

	close(arch);
	printf("Unpacked\n");
}

void unpack_file(int arch, char file_name[80],size_t file_size, int parent_id, int *cur_id, int nested_directory_array[20], int *index)
{
	//printf("Unpacking file %s\n. curr_id = %d, parent_id = %d\n", file_name, *cur_id, parent_id);
	while(*cur_id != parent_id)
	{
		chdir("..");
		//printf("curr_id = %d, parent_id = %d, parent = %d\n", *cur_id, parent_id, nested_directory_array[(*index)]);
		*cur_id = nested_directory_array[(*index)--];
	}

	char block[file_size]; //TODO: разбить на блоки по 1024
	if(read(arch, &block,file_size)!=file_size)
	{
		printf("Incorrect file reading");
	}
	int file = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
	write(file, &block, file_size);
	close(file);
}

void unpack_dir(int arch, char file_name[80], int id, int *cur_id, int parent_id, int nested_directory_array[20], int *index)
{
	//printf("Unpacking directory %s\n. curr_id = %d, parent_id = %d\n", file_name, *cur_id, parent_id);
	while(*cur_id !=parent_id)
	{
		chdir("..");
		*cur_id = nested_directory_array[(*index)--];
	}

	mkdir(file_name,O_CREAT| O_TRUNC | S_IRUSR| S_IWUSR);
	chdir(file_name);
	nested_directory_array[++(*index)] = parent_id;
	*cur_id = id;
}

void pack(char *file_names_arr[], int length, int start_index)
{
	//printf("In the packing function now.\n");

	int file_count = 0;

	//Creating archive file
	int arch = open("test_archive.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR); //TODO: change name
	if(arch == -1)
	{
		printf("Couldn't create or open archive!");
		return;
	}

	write(arch, &file_count, sizeof(int));

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
			pack_fdescr(arch, file_names_arr[i], file_stat.st_size, &file_count, 0);
			
		}
		else if(S_ISDIR(file_stat.st_mode)) //если это директория
		{
			pack_ddestr(arch, file_names_arr[i], &file_count, 0);
		}
	}

	//Writing data into archive
	for(int i = start_index; i < length; i++)
	{
		int e = stat(file_names_arr[i], &file_stat);
		if(e == -1) 
		{
			printf("Some error has occured! %s not found!\n", file_names_arr[i]);
		}
			
		if(S_ISREG(file_stat.st_mode)) //если это файл
		{	
			pack_file(arch, file_names_arr[i]);		
		}
		else if(S_ISDIR(file_stat.st_mode)) //если это директория
		{
			pack_dir(arch, file_names_arr[i]);
		}
	}

	//writing true count of files instead of default value in the bgning of the file
	lseek(arch,SEEK_SET, 0);
	//printf("%d\n",file_count);
	write(arch, &file_count, sizeof(int));

	close(arch);
	printf("Packed\n");
}

void pack_fdescr(int arch, char file_name[80], size_t file_size, int *file_count, int parent_id) //creates and writes file descriptor in hte archive
{
	struct file_descr descr;
	descr.file_size = file_size;
	strcpy(descr.file_name, file_name);
	descr.is_file = true;
	descr.id = *file_count + 1;
	descr.parent_id = parent_id;
	if(write(arch, &descr, sizeof(struct file_descr))!=sizeof(struct file_descr))
	{
		printf("Couldnt write file descriptor in the archive.\n");	
	}
	(*file_count)++;
}

void pack_ddestr(int arch, char file_name[80], int *file_count, int parent_id) //creates and writes directory descriptor in the archive
{
	struct file_descr descr;
	descr.file_size = 0;
	strcpy(descr.file_name, file_name);
	descr.is_file = false;
	descr.id = *file_count + 1;
	descr.parent_id = parent_id;
	if(write(arch, &descr, sizeof(struct file_descr))!=sizeof(struct file_descr))
	{
		printf("Couldn't write directory descriptor in the archive.\n");	
	}
	(*file_count)++;

	DIR *d;
	struct dirent *entry;
	struct stat statbuf;
	if ((d = opendir(file_name)) == NULL)
	{
		printf("Error occured! Can't open directory.");
	}
	chdir(file_name);
	//printf("In directory %s\n", file_name);
	while ((entry = readdir(d)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if(S_ISREG(statbuf.st_mode))
		{
			pack_fdescr(arch, entry->d_name, statbuf.st_size, file_count, descr.id);
		}
		else if (S_ISDIR(statbuf.st_mode)) 
		{
			/* Находит каталог, но игнорирует . и .. */
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;
			pack_ddestr(arch, entry->d_name, file_count, descr.id);
		}
	}
	chdir("..");
	closedir(d);
}

void pack_dir (int arch, char file_name[80]) //adds content of directory to the archive
{
	DIR *d;
	struct dirent *entry;
	struct stat statbuf;
	if ((d = opendir(file_name)) == NULL)
	{
		printf("Error occured! Can't open directory.");
	}
	chdir(file_name);
	while ((entry = readdir(d)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		if(S_ISREG(statbuf.st_mode))
		{
			pack_file(arch, entry->d_name);	
		}
		else if (S_ISDIR(statbuf.st_mode)) 
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;
			pack_dir(arch, entry->d_name);	
		}
	}
	chdir("..");
	closedir(d);
}

void pack_file(int arch, char file_name[80]) //adds file's data to the archive
{
	int nread;
	char block[1024];
	int file = open(file_name, O_RDONLY);
	if (file == -1)
	{
		printf("Some error has occured! Can't open file.\n");
		return;
	}
	while((nread = read(file, block, sizeof(block))) > 0)
	{
		write(arch, block, nread);
	}
	close(file);
}
