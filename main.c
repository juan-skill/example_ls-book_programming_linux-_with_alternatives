#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcmp */
#include <errno.h>  /* errno */
#include <sys/types.h>  /* opendir */
#include <dirent.h>     /* opendir */
#include <sys/stat.h>   /* lstat or stat */
#include <time.h>   /* ctime */
#include <unistd.h>  /* write */
#include <pwd.h>  /* getgrgid */
#include <grp.h> /* getpwuid */



#define FP_SPECIAL 1 /* Include set-user-ID, set-group-ID, and sticky
 bit information in returned string */

#define BUF_SIZE 1024
#define FLUSH -1
#define KILOBYTE 1024

#define STR_SIZE sizeof("rwxrwxrwx")

/**
 * _memset - fills memory with a constant byte
 *
 * @s: memory area to fill
 * @b: constant byte to fill the memory area
 * @n: n bytes of the memory area pointed to by s
 */
void *_memset(void *s, int b, unsigned int n)
{
        unsigned int i = 0;

        for (i = 0; i < n; i += 4)
        {
                ((char *) s)[i] = b;
        }

        return (s);
}


/**
 * _strlen - returns the length of a string
 * @s: string s
 * Return: length of string
 */
int _strlen(char *s)
{
        char *p = s;

        while (*s)
                s++;
        return (s - p);
}

/**
 * _strncat - concatenates two strings to n-character
 * @dest: copy destination
 * @src: copy source
 * Return: concatenated string
 */
char *_strncat(char *dest, char *src, int n)
{
        char *ptr = dest + _strlen(dest);

        while (*src && n--)
                *ptr++ = *src++;

        *ptr = 0;

        return (dest);
}

/**
 * _strcmp - compares two strings
 * @s1: string 1
 * @s2: string 2
 * Return: 1 if true, 0 if false
 */
int _strcmp(char *s1, char *s2)
{
        while (*s1)
        {
                if (*s1 != *s2)
                        break;
                s1++;
                s2++;
        }

        return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/**
 * _strdup - returns a pointer to a newly allocated space in memory,
 * which contains a copy of the string given as a parameter.
 * @str: string to be copied
 * Return: copied string
 */
char *_strdup(char *str)
{
        int i, len;
        char *copy;

        if (!str)
                return (NULL);
        len = _strlen(str);
        copy = malloc(sizeof(char) * len + 1);
        if (!copy)
                return (NULL);
        for (i = 0; i < len; i++)
                copy[i] = str[i];
        copy[i] = 0;
        return (copy);
}

/**
 * _putchar - writes the character c to stdout
 * @c: The character to print
 * Return: On success 1.
 * On error, -1 is returned, and errno is set appropriately.
 * Description: _putchar uses a local buffer of 1024 to call write
 * as little as possible
 */
int _putchar(char c)
{
        static char buf[KILOBYTE];
        static int i;

        if (c == FLUSH || i >= KILOBYTE)
        {
                write(1, &buf, i);
                i = 0;
        } else
        {
                buf[i] = c;
                i++;
        }
        return (1);
}

/**
 * _puts - prints a string to stdout
 * @str: pointer to the string to print
 * Return: number of chars written
 */
int _puts(char *str)
{
        register short i;

        for (i = 0; str[i]; i++)
                _putchar(str[i]);
        return (i);
}


char * /* Return ls(1)-style string for file permissions mask */
filePermStr(mode_t perm, int flags)
{
	static char str[STR_SIZE];
	snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
		 (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
		 (perm & S_IXUSR) ?
		 (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		 (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
		 (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
		 (perm & S_IXGRP) ?
		 (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
		 (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
		 (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
		 (perm & S_IXOTH) ?
		 (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
		 (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
	return str;
}

/**
 * main - Entry point
 *
 * Return: Always (0)
 */
int main(int argc, char *argv[])
{
        DIR *dir = NULL;
        struct dirent *entry = NULL;
        struct stat sb;
        char buffer[BUF_SIZE] = {0};
	char filename[BUF_SIZE] = {0};
	char *timed = NULL;
	char *timd = NULL;
	struct passwd *usr = NULL;
	struct group *grp = NULL;

        if (argc != 2)
                return (EXIT_FAILURE);

        dir = opendir(*(argv + 1));
        if (!dir)
        {
                sprintf(buffer, "%s: cannot access '%s'", *(argv + 0), *(argv + 1));
                perror(buffer);
                return (EXIT_FAILURE);
        }

        for (;;)
        {
		errno = 0;
                entry = readdir(dir);
                if (!entry)
                {
                        break;
                }

		sprintf(filename, "%s%s", *(argv + 1), entry->d_name);

		if (lstat(filename, &sb) == -1)
		{
			perror("lstat ");
		}

		if (sb.st_mode && (S_IRUSR && S_IWUSR && S_IXUSR))
		{
			timed = (char *) malloc(22);
			if (!timed)
			{
				return (EXIT_FAILURE);
			}

			_memset(timed, 0, 22);

			/* timd =  _strdup(ctime(&(sb.st_mtime)) + 4); */
			timd =  ctime(&(sb.st_mtime)) + 4;

			timed = _strncat(timed, timd, 12);

			usr = getpwuid(sb.st_uid);
			grp = getgrgid(sb.st_gid);

			printf("%s %s %s %lld %s %s\n",
			       filePermStr(sb.st_mode, 0), usr->pw_name, grp->gr_name,
			       (long long) sb.st_size, timed, entry->d_name);

			free(timed);
			/* free(timd); */
			_putchar(FLUSH);
		}
        }

        if (errno && !entry)
        {
                perror("readdir");
        }

        if (closedir(dir) == -1 )
        {
                perror("closedir");
        }

	/* free(time); */

        return (EXIT_SUCCESS);
}
