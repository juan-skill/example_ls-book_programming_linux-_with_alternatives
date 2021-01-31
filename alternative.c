#define _BSD_DEFAULT /* Get major() and minor() from <sys/types.h> */
#include <dirent.h>     /* opendir */
#include <sys/stat.h>   /* lstat or stat */
#include <sys/types.h>
#include <time.h>
#include "./lib/tlpi_hdr.h"
#include "./lib/error_functions.h"
#include "./lib/file_perms.h"

#define BUF_MAX 1024

static void
displayStatInfo(const struct stat *sb)
{
	printf("File type: ");
	switch (sb->st_mode & S_IFMT) {
	case S_IFREG: printf("regular file\n"); break;
	case S_IFDIR: printf("directory\n"); break;
	case S_IFCHR: printf("character device\n"); break;
	case S_IFBLK: printf("block device\n"); break;
	case S_IFLNK: printf("symbolic (soft) link\n"); break;
	case S_IFIFO: printf("FIFO or pipe\n"); break;
	case S_IFSOCK: printf("socket\n"); break;
	default: printf("unknown file type?\n"); break;
	}
	printf("Device containing i-node: major=%ld minor=%ld\n",
	       (long) (sb->st_dev), (long) (sb->st_dev));

	printf("I-node number: %ld\n", (long) sb->st_ino);

	if (sb->st_mode && (S_IRUSR && S_IWUSR && S_IXUSR))
		printf("Mode: %lo (%s)\n",
		       (unsigned long) sb->st_mode, filePermStr(sb->st_mode, 0));

	if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
		printf(" special bits set: %s%s%s\n",
		       (sb->st_mode & S_ISUID) ? "set-UID " : "",
		       (sb->st_mode & S_ISGID) ? "set-GID " : "",
		       (sb->st_mode & S_ISVTX) ? "sticky " : "");
	printf("Number of (hard) links: %ld\n", (long) sb->st_nlink);

	printf("Ownership: UID=%ld GID=%ld\n",
	       (long) sb->st_uid, (long) sb->st_gid);
	if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
		printf("Device number (st_rdev): major=%ld; minor=%ld\n",
		       (long) (sb->st_rdev), (long) (sb->st_rdev));
	printf("File size: %lld bytes\n", (long long) sb->st_size);
	printf("Optimal I/O block size: %ld bytes\n", (long) sb->st_blksize);
	printf("512B blocks allocated: %lld\n", (long long) sb->st_blocks);
	printf("Last file access: %s", ctime(&sb->st_atime));
	printf("Last file modification: %s", ctime(&sb->st_mtime));
	printf("Last status change: %s", ctime(&sb->st_ctime));
}


static void /* List all files in directory 'dirPath' */
listFiles(const char *dirpath)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	Boolean isCurrent; /* True if 'dirpath' is "." */
	struct stat sb;
	char filename[BUF_MAX];


	isCurrent = strcmp(dirpath, ".") == 0;

	dirp = opendir(dirpath);
	if (dirp == NULL) {
		errMsg("opendir failed on '%s'", dirpath);
		return;
	}

	/* For each entry in this directory, print directory + filename */
	for (;;) {
		errno = 0; /* To distinguish error from end-of-directory */

		dp = readdir(dirp);
		if (dp == NULL)
			break;

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue; /* Skip . and .. */

		if (!isCurrent)
			printf("%s", dirpath);

		sprintf(filename, "%s%s", dirpath, dp->d_name);
		printf("dirpath=%s %s\n", dirpath, filename);

		if (lstat(filename, &sb) == -1)
		{
			errExit("lstat");
		}

		displayStatInfo(&sb);
		printf("\n\n");
	}

	if (errno != 0)
		errExit("readdir");

	if (closedir(dirp) == -1)
		errMsg("closedir");
}


int
main(int argc, char *argv[])
{
	if (argc > 1 && strcmp(argv[1], "--help") == 0)
		usageErr("%s [dir...]\n", argv[0]);

	if (argc == 1) /* No arguments - use current directory */
		listFiles(".");
	else
		for (argv++; *argv; argv++)
		{
			printf("when %s\n", *argv);
			listFiles(*argv);
		}

	exit(EXIT_SUCCESS);
}
