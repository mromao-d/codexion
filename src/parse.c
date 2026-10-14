#include "../lib/codexion.h"

static int	parse_number(const char *str)
{
	long	out;
	int		i;

	i = 0;
	out = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (-1);
	while (str[i] >= '0' && str[i] <= '9')
	{
		out = out * 10 + (str[i] - '0');
		if (out > 2147483647)
			return (-1);
		i++;
	}
	if (str[i])
		return (-1);
	return ((int)out);
}

static int	parse_scheduler(t_props *props, const char *str)
{
	if (strcmp(str, "edf") == 0)
		props->is_edf = 1;
	else if (strcmp(str, "fifo") == 0)
		props->is_edf = 0;
	else
		return (1);
	return (0);
}

int	parse_args(t_props *props, char **argv)
{
	props->number_of_coders = parse_number(argv[1]);
	props->time_to_burnout = parse_number(argv[2]);
	props->time_to_compile = parse_number(argv[3]);
	props->time_to_debug = parse_number(argv[4]);
	props->time_to_refactor = parse_number(argv[5]);
	props->number_of_compiles_required = parse_number(argv[6]);
	props->dongle_cooldown = parse_number(argv[7]);
	if (props->number_of_coders < 1 || props->time_to_burnout < 0
		|| props->time_to_compile < 0 || props->time_to_debug < 0
		|| props->time_to_refactor < 0
		|| props->number_of_compiles_required < 0
		|| props->dongle_cooldown < 0)
		return (1);
	return (parse_scheduler(props, argv[8]));
}
