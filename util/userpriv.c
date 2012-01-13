/*
 * Starts a program with different privileges with no need to spawn a new shell
 *
 * Copyright (C) 2007 - 2009 Nokia. All rights reserved.
 *
 * @author Philippe De Swert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <sysexits.h>

int main(int argc, char *argv[], char *envp[])
{
        int i;
        char *cmdline;
	char *cmd[8];
	pid_t pid;
	uid_t uid = 29999;
	gid_t gid = 29999;
//	uid_t uid = 1000;
//	gid_t gid = 1000;


	/* TODO: Deal nicely with the command line */
        cmdline = malloc(sizeof(char)*256);

        for (i = 1; i < argc; i++)
        {
                strcat(cmdline,argv[i]);
                strcat(cmdline, " ");
		cmd[i-1] = argv[i];
        }
	/* correctly terminate the cmd array */
	cmd[i] = NULL;


	printf("prepare for fork!\n");
	pid = fork();

	if (pid == -1)
	{
		printf("Forking failed! Exiting\n");
		exit(1);
	}
	else if (pid == 0)
	{
		struct passwd *pass;

		/* child process to be spawned */
		printf("Child process sucessfully spawned\n");

		/* no passwd entry no banana */
		pass = getpwuid(uid);
		if (pass == NULL)
			exit(EX_NOPERM);
		
		/* set group id */
		if (setgid(gid)) 
                        exit(EX_NOPERM);
		/* set supplementary groups */

		if(initgroups(pass->pw_name,gid))
			exit(EX_NOPERM);
		
		/* set user id */
                if (setuid(uid)) 
                        exit(EX_NOPERM);

                if (envp != NULL) 
		{
                        execve(argv[1], &argv[1], envp);
			perror("env? error");
			printf("errno = %d\n", errno);
		}
		else 
                        execvp(argv[1], cmd);

		/* Should only be seen if something goes wrong with the exec */	
                printf("execution failed!\n");

                exit(0);        
        } 
	else
	{
		/* end main process */
		printf("program %s should be forked. Exiting!\n", cmdline);

		exit(0);
	}
}


