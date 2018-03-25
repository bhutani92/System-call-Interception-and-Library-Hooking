# System-call-Interception-and-Library-Hooking
Adding extension to prevent against ransomwares by duplicating any files overwritten by a application / process.

------------------------------------------------------------------------------------------------------------------------------------------

The extension developed creates backup copies of files before they are overwritten. This can be very effective against ransomewares. To avoid creating unnecssary backup copies, the backup is not created when files are opened in read-only mode.

The various system calls that can effect the file contents are -
1. open - Open files in various modes with various flags
2. openAt - Open files in various modes with various flags
3. creat - open a file in trunc mode if it already exisits
4. link - can be used while file is renamed or linked
5. linkAt - can be used while file is renamed or linked
6. unlink - can be used when file is deleted
7. open_by_handle_at - obtain handle for a pathname and open file via a handle
8. memfd_create - create anonymous file
9. mknod - create a special or ordinary file
10. mknodat - create a special or ordinary file relative to fd
11. rename - rename a file
12. renameat - rename a file relative to fd
13. truncate - truncate a file to a specified length

I am creating backup files for most commonly used system calls i.e. open, openat, creat, link, linkat, unlink, unlinkat. For all other system calls, the exploit/backup program code is similar and backup files needs to be created in the same way.
These set of system calls are used by many editors to create, rename, overwrite or delete a file. 

fflush system call is used when redirecting the output from stdout to a file. This can also be hooked eventually to save a backup if the file already exists. Currently it has not been implemented.

Currently backups for regular files are created. However, if required backups for all files like symbolic links and other special files can also be created.
All the SystemCalls mentioned above are modified and object code is located in backupFiles.so to create a backup when file is opened in write-only mode, renamed or deleted. However, when file attributes are changed or permissions are altered we are not creating any copies. If we intend to create copies when the permissions are altered, we can use stat system call to first check for file permissions and then create a backup file with same permissions/attributes and then copy the contents. As this function call has much of its code duplicated to open where we create a backup file simply, this has not been added currently.
Also, linkat systemcall is similar to link system call and hence this is also not tweaked currently.

The backup directory is always located in $HOME as .backup folder. All backup files are appended with timestamps so as that the file names are unique and backup copies do not interfere with each other.

Currently, I have assumed $HOME directory contains the location of home directory and is not modified by malicious attacker. If the $HOME directory is left blank, we can simply check and place the backup directory in root. Other measures can be taken to make sure $HOME points to a valid user directory.
As the backup folder is mostly stored in cloud, we can safely assume the above and ignore the condition checks for this assignment.

I have made sure the program works with Editors as well for bonus points. The underlying principle remains the same when working with editors. All editors use the same set of system calls to request writing to a file.

------------------------------------------------------------------------------------------------------------------------------------------

Execution -

export LD_PRELOAD=""
make
export LD_PRELOAD=/path_to_backupFiles.so/
