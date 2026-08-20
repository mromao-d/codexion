#include "../lib/codexion.h"

void	map_dongles(t_props *props)
{
	int	i;

	i = -1;
	while (++i < props->number_of_coders)
	{	
		pthread_mutex_init(&props->dongles[i].dongle, 0);
		props->dongles[i].dongle_id = i;
		props->dongles[i].is_free = 1;
	}
	return ;
}
