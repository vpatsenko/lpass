#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

typedef struct Entry
{
	char *name;
	char *email;
	char *password;
} Entry;

char const *get_entry_cmd = "--get";
char const *put_entry_cmd = "--put";

Entry *get_entry_from_file(char *file_name, char *entry_name);
char *get_entry_name(char *line);
Entry *get_entry(char *line);
int write_entry_into_file(char *line);
char *truncate(char *line, char delim);
char *generate_pass_with_len(int len);

int main(int argc, char const **argv)
{
	errno = 0;
	if (argc != 3)
	{
		printf("you should provide --get or --put and entry name\n");
		exit(EXIT_SUCCESS);
	}

	char *cmd = argv[1];
	char *name = argv[2];

	if (strcmp(cmd, get_entry_cmd) == 0)
	{
		Entry *e = get_entry_from_file("passwords.txt", name);

		printf("%s\n", e->name);
		printf("%s\n", e->email);
		printf("%s\n", e->password);
	}

	if (strcmp(cmd, put_entry_cmd) == 0)
	{

		char *email = NULL;
		char *password_len = NULL;
		size_t len = 0;
		printf("enter email: ");

		if (getline(&email, &len, stdin) == -1)
		{
			printf("failed to read line\n");
			exit(EXIT_FAILURE);
		}

		printf("enter password len: ");
		if (getline(&password_len, &len, stdin) == -1)
		{
			printf("failed to read line\n");
			exit(EXIT_FAILURE);
		}

		int pass_len_int = atoi(password_len);
		if (pass_len_int == 0)
		{
			printf("failed to convert password_len to int or password_len equals to zero");
			exit(EXIT_FAILURE);
		}

		char line_to_write[len * 2];
		char *name_truncated = truncate(name, '\n');
		char *email_truncated = truncate(email, '\n');
		char *pass = generate_pass_with_len(pass_len_int);

		strcat(line_to_write, name);
		strcat(line_to_write, " ");
		strcat(line_to_write, email);
		strcat(line_to_write, " ");
		strcat(line_to_write, pass);

		int err = write_entry_into_file(line_to_write);
		if (err == -1)
		{
			perror("write_entry_into_file(): error writing into file: ");
			exit(EXIT_FAILURE);
		}
	}

	// char *p = generate_pass_with_len(30);
	// printf("%s\n", p);

	return 0;
}

Entry *get_entry_from_file(char *file_name, char *name_need)
{
	FILE *fptr = fopen(file_name, "a+");
	if (!fptr)
	{
		printf("failed to opent file");
		exit(1);
	}

	char *line = NULL;
	size_t len = 0;
	size_t nread;
	Entry *e;

	while ((nread = getline(&line, &len, fptr)) != -1)
	{
		char *entry_name = get_entry_name(line);
		if (strcmp(entry_name, name_need) == 0)
		{
			e = get_entry(line);
			// free(line);
			break;
		}
	}

	fclose(fptr);
	return e;
}

char *get_entry_name(char *line)
{

	char *entry_name = (char *)malloc(sizeof(char) * sizeof(line) / sizeof(line[0]));

	for (int i = 0; line[i] != '\0'; i++)
	{
		if (line[i] == ' ')
		{
			break;
		}
		entry_name[i] = line[i];
	}

	return entry_name;
}

Entry *get_entry(char *line)
{
	Entry *e = (Entry *)malloc(sizeof(Entry));

	int n = 0;
	int words_counter = 0;

	char *name = NULL;
	char *email = NULL;
	char *password = NULL;

	for (int i = 0; line[i] != '\0'; i++)
	{
		if (line[i] == ' ' || line[i] == '\n')
		{
			switch (words_counter)
			{
			case 0:
				name = &line[n];
				line[i] = '\0';
				n = i + 1;
				words_counter++;
				continue;
			case 1:
				email = &line[n];
				line[i] = '\0';
				n = i + 1;
				words_counter++;
				continue;
			case 2:
				password = &line[n];
				line[i] = '\0';
				n = i + 1;
				words_counter++;
				continue;
			}
		}
	}

	e->email = email;
	e->name = name;
	e->password = password;
	return e;
}

int write_entry_into_file(char *line)
{

	FILE *fptr;
	fptr = fopen("passwords.txt", "a+");
	if (!fptr)
	{
		return -1;
	}

	int err = fputs(line, fptr);
	if (err == -1)
	{
		fclose(fptr);
		return -1;
	}

	err = fputc('\n', fptr);
	if (err == -1)
	{
		fclose(fptr);
		return -1;
	}

	fclose(fptr);
	return 0;
}

char *truncate(char *line, char delim)
{
	for (int i = 0; line[i] != '\0'; i++)
	{
		if (line[i] == delim)
		{
			line[i] = '\0';
		}
	}

	return line;
}

char *generate_pass_with_len(int len)
{
	time_t some_time = time(NULL);
	srand((unsigned int)(some_time));

	char *pass = (char *)malloc(sizeof(char) * len);
	int i = 0;

	for (i = 0; i < len; i++)
	{
		pass[i] = 33 + rand() % 94;
	}
	pass[i] = '\0';

	return pass;
}