#include "../lib/codexion.h"

int ft_print(char *str)
{
	while (*str)
	{
		write(1, &(*str), 1);
		str++;
	}
	return (1);
}

void	ft_print_args(t_props **props) {
	// printf("configs are: %i; %i; %i\n",
	// 	(*props)->number_of_coders, (*props)->time_to_burnout, (*props)->time_to_compile);
	printf("Number of Coders is: %i\n", (*props)->number_of_coders);
	for (int i = 0; i < (*props)->number_of_coders; i++)
		printf("This is Coder %i\n", (*props)->coders[i].coder_id);
	return ;
}

int	ft_free_all(t_props	*props)
{
	int	i;

	i = -1;
	while (++i < props->number_of_coders)
	{
		pthread_mutex_destroy(&props->dongles[i].dongle);
		free(props->coders[i].dongles);
	}
	free(props->coders);
	free(props->dongles);
	free(props);
	return (0);
}

void	ft_print_coders_dongles(t_props *props) {
	for (int i = 0; i < props->number_of_coders; i++) {
		printf("coder %i dongles are: %i, %i",props->coders[i].coder_id, props->coders[i].dongles[0].dongle_id, props->coders[i].dongles[1].dongle_id);
		printf("\n");
	}
	return ;
}

int main(int argc, char *argv[])
{
	t_props	*props;

	if (argc != 9)
		return (ft_print("Wrong number of args\n"));
	props = map_args(argv);
	if (validate_args(&props))
	{
		ft_print("Wrong args\n");
		free(props);
		return (1);
	}
	map_dongles(props);
	map_coders(props);
	// ft_print_coders_dongles(props);
	return (ft_free_all(props));
}
