#include "microshell.h"

int	ft_strlen(char *str)
{
	int i = 0;
	while(str[i])
		i++;
	return (i);	
}

void	ft_putstr_err(char *str)
{
	write(2, str, ft_strlen(str));
}

void	ft_exit(char *str)
{
	ft_putstr_err(str);
	exit(1);
}

int main(int ac, char *av[], char *envp[])
{
	int pipe_fd[2];
	int is_pipe = 0;
	int previous_fd = -1;
	int nbr_arg = 0;
	int i = 1;
	int pid;

	if(ac == 1)
		return (1);
	while(i <= ac)
	{
		if(av[i] && strcmp(av[i], "|") != 0 && strcmp(av[i], ";") != 0)
			nbr_arg++;
		else if(nbr_arg != 0)
		{
			if(av[i])
			{
				is_pipe = strcmp(av[i], "|") == 0 ? 1 : 0;
			}
			av[i] = NULL;
			if(strcmp(av[i - nbr_arg], "cd") == 0)
			{
				if(nbr_arg == 2)
				{
					if((chdir(av[i - nbr_arg + 1]) == -1))
					{
						ft_putstr_err("error: cd: cannot change directory to ");
						ft_putstr_err(av[i - nbr_arg + 1]);
						ft_putstr_err("\n");
					}
				}
				else
					ft_putstr_err("error: cd: bad arguments\n");
			}
			else
			{
				if(is_pipe)
					if(pipe(pipe_fd) == -1)
						ft_exit("error: fatal\n");
				if((pid = fork()) == -1)
					ft_exit("error: fatal\n");
				if(pid == 0)
				{
					if(previous_fd != -1)
						if(dup2(previous_fd, 0) == -1)
							ft_exit("error: fatal\n");
					if(is_pipe)
						if(dup2(pipe_fd[1], 1) == -1)
							ft_exit("error: fatal\n");;
					if(execve(av[i - nbr_arg], &av[i - nbr_arg], envp) == -1)
					{
						ft_putstr_err("error: cannot execute ");
						ft_putstr_err(av[i - nbr_arg]);
						ft_exit("\n");
					}
				}
				else
				{
					waitpid(-1, 0, 0);
					close(pipe_fd[1]);
					if(previous_fd != -1)
						close(previous_fd);
					if(is_pipe)
						previous_fd = pipe_fd[0];
					if(!is_pipe)
						previous_fd = -1;
				}
			}
			nbr_arg = 0;
		}
		is_pipe = 0;
		i++;
	}
	while(42);
}

