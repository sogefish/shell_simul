#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<ctype.h>
#include<signal.h>
#include<fcntl.h>

void shell(int);
void reset(void);
static void split(char *cmd);
static char* args[512];

int zone = 0;
int fd[2];
pid_t pid;
int ret;
int fd_multiple = 0;
char cwd[1024];

void reset()
{
	zone = 0;
	fd_multiple = 0;
	fd[0] = 0;
	fd[1] = 0;
	args[0] = 0;
}

static void removelastspace(char* s)
{
	char* tmp = strchr(s, '\0');
	tmp = tmp-1;
	if(strcmp(tmp," ") == 0)
	{
		*tmp= '\0';
		removelastspace(s);
	}
}

void change_directory(char* d)
{   
  int change = chdir(d);
  //printf("\t%s\n", d);
  if(change < 0){ perror("invalid_directory"); }
}

void pipeSequence(char *args[], int pipesCount,int k) 
{
	int bg = -1;
	int i;
	char* args2[k];

	ret = pipe(fd);
	if(ret == -1){
		perror("pipe");
		return;
	}
	pid = fork();
	int pid_id = getpid(); 
	if(pid == -1)
	{
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)					// Child Process
	{
		dup2(fd_multiple,0);
		if(zone < pipesCount){
			dup2(fd[1],1);
		}
		close(fd[0]);

		int j;
		if(strcmp(args[k-1], "&") == 0)
		{
			printf("[%i]\n", pid_id);
			bg = 1; 
			k--;


			for(i = 0; i < k; i++)
			{
				args2[i] = args[i];
			}
		}

		if(bg == -1)
		{
			for(j = 0; j < k; j++)
			{
				if(strcmp(args[j], ">") == 0)
				{
					args[j] = NULL;
					j = j+1;

					close(1);

					char fn[100];
					strncpy(fn,args[j],99);

					int lfd = open(fn, O_RDWR | O_TRUNC);
					
					if(lfd == -1)
					{
						open(fn, O_CREAT | O_RDWR | O_APPEND);
					}
					args[j] = NULL;
				}
				else if(strcmp(args[j], "<") == 0)
				{
					args[j] = NULL;
					j = j+1;
						
					close(0);
						
					char fn[100];
					strncpy(fn,args[j],99);

					int lfd=open(fn, O_RDONLY);
						
					if(lfd == -1)
					{
						perror("invalid_file");
						exit(0);
					}
					args[j] = NULL;
				}
				else if(strcmp(args[j], ">>") == 0)
				{
					args[j] = NULL;
					j = j+1; 	
					
					close(1);

					char fn[100];
					strncpy(fn,args[j],99);

					int lfd = open(fn, O_RDWR | O_APPEND);

					if(lfd == -1)
					{
						open(fn, O_CREAT | O_RDWR | O_APPEND);
					}
					args[j] = NULL;
				}
				else if(strcmp(args[j], "cd") == 0)
				{
					change_directory(args[j+1]);
					exit(0);
				}		
			}

			ret = execvp(args[0], args);
		}
		else
		{	
			for(j = 0; j < k; j++)
			{
				if(strcmp(args2[j], ">") == 0)
				{
					args2[j] = NULL;
					j = j+1;

					close(1);

					char fn[100];
					strncpy(fn,args2[j],99);

					int lfd = open(fn, O_RDWR | O_TRUNC);
					
					if(lfd == -1)
					{
						open(fn, O_CREAT | O_RDWR | O_APPEND);
					}
					args2[j] = NULL;
				}
				else if(strcmp(args2[j], "<") == 0)
				{
					args2[j] = NULL;
					j = j+1;
						
					close(0);
						
					char fn[100];
					strncpy(fn,args2[j],99);

					int lfd=open(fn, O_RDONLY);
						
					if(lfd == -1)
					{
						perror("invalid_file");
						exit(0);
					}
					args2[j] = NULL;
				}
				else if(strcmp(args2[j], ">>") == 0)
				{
					args2[j] = NULL;
					j = j+1; 	
					
					close(1);

					char fn[100];
					strncpy(fn,args2[j],99);

					int lfd = open(fn, O_RDWR | O_APPEND);

					if(lfd == -1)
					{
						open(fn, O_CREAT | O_RDWR | O_APPEND);
					}
					args2[j] = NULL;
				}
				else if(strcmp(args2[j], "cd") == 0)
				{
					change_directory(args2[j+1]);
					exit(0);
				}		
			}			

			bg = -1;
		}
		if(ret == -1)
		{
			perror("Error");	
			exit(0);	
		}

	}
    else				// Parent Process
    {
		if(bg != 1){
			wait(NULL);
		}
		close(fd[1]);
		fd_multiple = fd[0];
		bg = -1;
	
    }
}

void prompt()
{
  char shell[1000];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
          strcpy(shell, "DKim:");
          strcat(shell, cwd);
          strcat(shell, ":$ ");

          printf("%s", shell);
        }
   else
       perror("getcwd() error");

}

int main(){
	printf("Operating Systems Final Project: Shell\n");

	signal(SIGINT, shell);
	shell(0);
	return 0;
}

void shell(int signal)
{
		char command[1024];
		int flag = 1;
		printf("\n");

		do
		{
      prompt();
			scanf("%[^\n]%*c",command);
			removelastspace(command);

			int pipesCount = 0;
            int i;
			for(i = 0; i < strlen(command); i++){
				if(command[i] == '|'){
					pipesCount++;			
				}
			}

			char pipeCommands[pipesCount+1][100];
			int j = 0, k = 0;
			for(i = 0; i < strlen(command); i++){
				if(command[i] == '|'){
					pipeCommands[j][k-1] = '\0';
					j++;
					k = 0;									
				}
				else if(command[i] == ' '){
					pipeCommands[j][k] = '$';
					k++;			
				}
				else{
					pipeCommands[j][k] = command[i];
					k++;
	
				}			
			}
			pipeCommands[j][k]  = '\0';

			for(i = 0; i < pipesCount+1; i++)
			{
				char *args[10];	
				int k = 0;
				if(i == 0){
					args[0]=&pipeCommands[i][0];
					k++;
				}

				unsigned int size = strlen(pipeCommands[i]);
				for(j = 0; j < size; j++)
				{
					if(pipeCommands[i][j] == '$'){
							pipeCommands[i][j] = '\0';
							args[k] = &pipeCommands[i][j+1];
							k++;			
					}
				}
				args[k] = NULL;

				if(strcmp(args[0], "exit") == 0)
				{
					kill(0, SIGKILL);
					exit(0);
				}

				pipeSequence(args, pipesCount,k);
				zone++;
			}

			reset();	

	}while(flag == 1);
}