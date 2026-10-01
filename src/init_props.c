#include "../lib/codexion.h"

t_props	*map_args(char *args[])
{
	t_props	*props;

	props = malloc(sizeof(t_props));
	if (!props)
		return (NULL);
	(*props).number_of_coders = ft_atoi(args[1]);
	(*props).time_to_burnout = ft_atoi(args[2]) * 1000;
	(*props).time_to_compile = ft_atoi(args[3]) * 1000;
	(*props).time_to_debug = ft_atoi(args[4]) * 1000;
	(*props).time_to_refactor = ft_atoi(args[5]) * 1000;
	(*props).number_of_compiles_required = ft_atoi(args[6]);
	(*props).dongle_cooldown = ft_atoi(args[7]) * 1000;
	(*props).scheduler = args[8];
	(*props).coders = malloc(sizeof(t_coders) * (*props).number_of_coders);
	(*props).dongles = malloc(sizeof(t_dongles) * (*props).number_of_coders);
	(*props).queue = malloc(sizeof(int) * (*props).number_of_coders);
	(*props).queue_size = 0;
	(*props).start = 0;
	(*props).dead = 0;
	pthread_mutex_init(&props->print, 0);
	pthread_mutex_init(&props->scheduler_mutex, 0);
	pthread_cond_init(&props->scheduler_cond, 0);
	return (props);
}

int	validate_args(t_props **props)
{
	if (((*props)->number_of_coders == -1) || ((*props)->time_to_burnout == -1) ||
		((*props)->time_to_compile == -1) || ((*props)->time_to_debug == -1) ||
		((*props)->time_to_refactor == -1) || ((*props)->number_of_compiles_required == -1) ||
		((*props)->dongle_cooldown == -1))
		return (1);
	if (ft_strcmp((*props)->scheduler, "fifo") != 0 && ft_strcmp((*props)->scheduler, "edf") != 0)
		return (1);
	return (0);
}
