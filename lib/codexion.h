# ifndef CONDEXION_H
# define CONDEXION_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_dongles {
	pthread_mutex_t	dongle;
	
	int				dongle_id;
	int				is_free;
}	t_dongles;

typedef struct s_coders {
	pthread_t	coder;
	
	int			coder_id;
	time_t		start_time;

	t_dongles	*dongles;
}	t_coders;

typedef struct s_props {
	int 		number_of_coders;
	int 		time_to_burnout;
	int 		time_to_compile;
	int 		time_to_debug;
	int 		time_to_refactor;
	int 		number_of_compiles_required;
	int 		dongle_cooldown;
	char		*scheduler;

	t_coders	*coders;
	t_dongles	*dongles;

	time_t		start_time;
}	t_props;

// atoi
int 	ft_atoi(const char *nptr);

// utils
int		ft_strcmp(char *s1, char *s2);
time_t	get_current_time(void);

// props.c
t_props	*map_args(char *args[]);
int		validate_args(t_props **props);

// coders.c
void	map_coders(t_props *props);
void 	join_coders(t_props *props);

// dongles.c
void	map_dongles(t_props *props);

#endif