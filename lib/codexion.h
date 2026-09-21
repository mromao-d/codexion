# ifndef CONDEXION_H
# define CONDEXION_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <errno.h>

typedef struct s_dongles {
	pthread_mutex_t	dongle;
	
	int				dongle_id;
	int				is_free;
}	t_dongles;

typedef struct s_props t_props;

typedef struct s_coders {
	pthread_t	coder;
	pthread_t	cool_down;
	
	int			coder_id;
	time_t		time_to_burnout;

	int			nb_comp;

	t_dongles	**dongles;

	t_props		*props;
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
	int			*queue;
	int			queue_size;

	t_coders	*coders;
	t_dongles	*dongles;

	pthread_mutex_t	print;
	pthread_mutex_t	scheduler_mutex;
	pthread_cond_t	scheduler_cond;

	time_t		start_time;
	int			start;
	int			dead;
}	t_props;

// atoi
int 	ft_atoi(const char *nptr);

// utils
int		ft_strcmp(char *s1, char *s2);
time_t	get_current_time(void);

// props.c
t_props	*map_args(char *args[]);
int		validate_args(t_props **props);

// init_coders.c
void	map_coders(t_props *props);
void 	join_coders(t_props *props);

// coders.c
void	assign_dongles(t_props *props);
void	*coder_routine(void *arg);
void	*drop_dongles(void *arg);

// dongles.c
void	map_dongles(t_props *props);

// queue.c
void	add_coder_queue(t_coders *coder);
void	remove_coder_queue(t_coders *coder);

#endif
