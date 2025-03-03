/***************************************************************************//**

  @file         myshell.c

  @author       Alexis Naranjo
  
  @ID		008347411

  @date         Monday,  30 September 2024

  @Purpose      Creates a shell called myshell that runs on linux operating systems.

*******************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
* Makes the shell name myshell the default shell name, makes terminator > the default terminator,
*intializes and keeps track of the number of new names being created, makes arrays for
*old names and new names to keep track of when they are created.
*/
char *shellname = "myshell";
char *terminator = ">";
int numberOfNewnames = 0;
char* oldNames[10] = {};
char* newNames[10] = {};

/*
*Function Declarations for builtin shell commands:
*/
int lsh_cd(char **args);
int lsh_help(char **args);
int stop(char **args);
int setshellname(char **args);
int setterminator(char **args);
int newname(char **args);
int listnewnames(char **args);
int savenewnames(char **args);
int readnewnames(char **args);

/*
*List of builtin commands, followed by their corresponding functions.
*/
char *builtin_str[] = {
  "cd",
  "help",
  "stop",
  "setshellname",
  "setterminator",
  "newname",
  "listnewnames",
  "savenewnames",
  "readnewnames"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &stop,
  &setshellname,
  &setterminator,
  &newname,
  &listnewnames,
  &savenewnames,
  &readnewnames
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
*Builtin function implementations.
*/

/*
*@Purpose 	Builtin command: change directory.
*
*@Parameters 	List of args.  args[0] is "cd".  args[1] is the directory.
*
*@Return Always returns 1, to continue executing.
*
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/*
*@Purpose	Builtin command: print help.
*
*@Parameters	List of args.  Not examined.
*
*@Return 	Always returns 1, to continue executing.
*
*/
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/*
*@Purpose 	Builtin command: exit.
*
*@Parameters 	List of args.  Not examined.
*
*@Return 	Always returns 0, to terminate execution.
*
*/
int stop(char **args)
{
  return 0;
}

/*
 *@Purpose	Sets the shell name to whatever the user wants the shell name to be
 * 		and sets the shell name back to the default of myshell if only setshellname is called.
 *
 *@Parameters	Takes an array of strings with args[0] being the setshellname command and
 *		args[1] being the name the user wishes to rename the shell name with.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 */
int setshellname(char **args)
{
	if (args[1] == NULL)
		shellname = "myshell";
	else
		shellname = args[1];
	return 1;
}

/*
 *@Purpose	Sets the terminator to whatever the user wants the terminator to be
 *		and sets the terminator back to the default of > if only setterminator is called.
 *
 *@Parameters	Takes an array of strings with args[0] being the setterminator command and
 *		args[1] being the terminator the user wishes to rename the terminator to.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 *
 */ 
int setterminator(char **args)
{
	if (args[1] == NULL)
	{
		terminator = ">";
	}
	else
	{
		terminator = args[1];
	}
	return 1;

}

/*
 *@Purpose	Creates a new name for a command and stores up to 10 in it's respective array
 *		of old or new. Also will delete a new name that hasn't been saved and reverts 
 *		it back to its old name if a second arguments isn't passed.
 *
 *Parameters	Takes an array of strings with args[0] being the newname command, args[1]
 *		being the command you would like to rename, and args[2] being the name
 *		you would like to rename the command to.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 *
 */
int newname(char **args)
{

	if (args[1] == NULL)
	{
		fprintf(stderr, "lsh: expected argument to \"newname\"\n");
	}
	if (args[2] == NULL)
	{
		for (int i = 0; i < numberOfNewnames; i++)
		{
			if (strcmp(newNames[i], args[1]) == 0)
			{
				for (int j = i; j < numberOfNewnames - 1; j++)
				{
					newNames[j] = newNames[j + 1];
					oldNames[j] = oldNames[j + 1];
				}
				numberOfNewnames--;
				break;
			}
		}
		
	}
	else
	{
		newNames[numberOfNewnames] = args[1];
		oldNames[numberOfNewnames] = args[2];
		numberOfNewnames++;
	}
	return 1;
}

/*
 *@Purpose	Prints the list of new names and old names in the arrays newNames and oldNames.
 *
 *@Parameters	Takes an array of strings with args[0] being the listnewnames command.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 *
 */
int listnewnames(char **args)
{
	for (int i = 0; i < numberOfNewnames; i++)
	{
		printf(" %s %s\n", newNames[i], oldNames[i]);
	}

	return 1;
}

/*
 *@Purpose	Saves all the new and old names that are in the array newNames and oldNames
 *		to a file named by the user.
 *
 *@Parameters	Takes an array of strings with args[0] being the savenewnames command and
 *		args[1] being the name that the user wants to call the file where
 *		the old and new names are to be stored from the arrays oldNames and newNames.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 *
 */
int savenewnames(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "lsh: expected argument to \"savenewnames\"\n");
	}
	else
	{
		FILE *file = fopen(args[1], "w");
		for (int i = 0; i < numberOfNewnames; i++)
		{
			fprintf(file, " %s %s\n", newNames[i], oldNames[i]);
		}
		fclose(file);
	}
	return 1;
}

/*
 *Purpose	Reads the names in the file created by the user with the savenewnames
 *		command and prints them.
 *
 *@Parameters	Takes and array of strings with args[0] being the readnewnames command and
 *		args[1] being the name of the file that the user wants the shell to read from
 *		that holds the names saved.
 *
 *@Return	Returns 1 to allow the shell to keep executing commands.
 *
 */
int readnewnames(char **args)
{
	char new[100], old[100];
	FILE *file = fopen(args[1], "r");
	if (args[1] == NULL)
	{
		fprintf(stderr, "lsh: expected argument to \"readnewnames\"\n");
	}
	else
	{
		if (file == NULL)
		{
			fprintf(stderr, "lsh: the file does not exist \"readnewnames\"\n");
			return 1;
		}
		while (fscanf(file, " %s %s", new, old) == 2)
		{
			newNames[numberOfNewnames] = new;
			oldNames[numberOfNewnames] = old;
			numberOfNewnames++;
			printf(" %s %s\n", new, old);
		}
	}

	return 1;
}

/**
*@Purpose	Launch a program and wait for it to terminate.
*
*@Parameters	Null terminated list of arguments (including program).
*
*@Return 	Always returns 1, to continue execution.
*
*/
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/*
*@Purpose 	Execute shell built-in or launch program.
*
*@Parameters	Null terminated list of arguments.
*
*@Return 	1 if the shell should continue running, 0 if it should terminate
*/
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < numberOfNewnames; i++)
  {
	  if (strcmp(args[0], newNames[i]) == 0)
	  {
		  args[0] = oldNames[i];
	  }
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/*
*@Purpose 	Read a line of input from stdin.
*
*@Parameters	N/A.
*
*@Return 	The line from stdin.
*
*/
char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We received an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/*
*@Purpose 	Split a line into tokens (very naively).
*
*@Parameters 	line The line.
*
*@Return 	Null-terminated array of tokens.
*
*/
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/*
*@Purpose 	Loop getting input and executing it.
*
*@Parameters	N/A
*
*@Return	N/A
*
*/
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("%s ", shellname);
    printf("%s ", terminator);
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    //free(line);
    //free(args);
  } while (status);
}

/*
*@Purpose 	Main entry point.
*
@Parameters 	argc Argument count. argv Argument vector.
*
*@Return 	status code
*
*/
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
