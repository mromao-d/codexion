#include "../lib/codexion.h"

t_props	*map_args(char *args[])
{
	t_props	*props;

	props = malloc(sizeof(t_props));
	if (!props)
		return (NULL);
	(*props).number_of_coders = ft_atoi(args[1]);
	(*props).time_to_burnout = ft_atoi(args[2]);
	(*props).time_to_compile = ft_atoi(args[3]);
	(*props).time_to_debug = ft_atoi(args[4]);
	(*props).time_to_refactor = ft_atoi(args[5]);
	(*props).number_of_compiles_required = ft_atoi(args[6]);
	(*props).dongle_cooldown = ft_atoi(args[7]);
	(*props).scheduler = args[8];
	(*props).coders = malloc(sizeof(t_coders) * (*props).number_of_coders);
	(*props).dongles = malloc(sizeof(t_dongles) * (*props).number_of_coders);
	(*props).start_time = get_current_time();
	return (props);
}

int	validate_args(t_props **props)
{
	if (((*props)->number_of_coders == -1) || ((*props)->time_to_burnout == -1) ||
		((*props)->time_to_compile == -1) || ((*props)->time_to_debug == -1) ||
		((*props)->time_to_refactor == -1) || ((*props)->number_of_compiles_required == -1) ||
		((*props)->dongle_cooldown == -1))
		return (1);
	if (ft_strcmp((*props)->scheduler, "FIFO") != 0 && ft_strcmp((*props)->scheduler, "LIFO") != 0)
		return (1);
	return (0);
}
