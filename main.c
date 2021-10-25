#include "header.h"

int		ft_strlen(char *str)
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
	int i = 0;
	int nbr_arg = 0;
	int is_pipe = 0;
	int is_next = 0;
	int pid;
	int pipe_fd[2];
	int previous_fd = -1;
	int waiter_pid;

	while(++i <= ac)
	{
		//printf("av[i] = %s strcmp = %d\n", av[i], strcmp(av[i], "|") != 0);
		if(av[i] && strcmp(av[i], "|") != 0 && strcmp(av[i], ";") != 0)
			nbr_arg++;
		else if(nbr_arg != 0)
		{
			if(av[i])
			{
				is_pipe = strcmp(av[i], "|") == 0 ? 1 : 0;
				is_next = strcmp(av[i], ";") == 0 ? 1 : 0;
				av[i] = NULL;
			}
			if(is_pipe)
				if(pipe(pipe_fd) == -1)
					ft_exit("error: fatal\n");
			if((pid = fork()) == -1)
				ft_exit("error: fatal\n");
			if(pid == 0)//in son
			{
				if(previous_fd != -1)
					if(dup2(previous_fd, 0) == -1)
						ft_exit("error: fatal\n");
				if(is_pipe)
				{
					close(pipe_fd[0]);
					if(dup2(pipe_fd[1], 1) == -1)
						ft_exit("error: fatal\n");
				}
				if(execve(av[i - nbr_arg], &av[i - nbr_arg], envp) < 0)
				{
					ft_putstr_err("error: cannot execute ");
					ft_putstr_err(av[i - nbr_arg]);
					ft_exit("\n");
				}
			}
			else
			{
				if(waitpid(-1, &waiter_pid, 0) == -1)
					ft_exit("error: fatal\n");
				if(is_pipe)
				{
					close(pipe_fd[0]);
					previous_fd = pipe_fd[1];
				}
			}
			nbr_arg = 0;
		}
	}
	return (0);
}