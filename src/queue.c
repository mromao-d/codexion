#include "../lib/codexion.h"

void	add_coder_queue(t_coders *coder)
{
	int	i;

	i = 0;
	while (i < coder->props->queue_size)
	{
		if (coder->props->queue[i] == coder->coder_id)
			return ;
		i++;
	}
	coder->props->queue[i] = coder->coder_id;
	coder->props->queue_size++;
}

void	remove_coder_queue(t_coders *coder)
{
	int	i;

	i = 1;
	if (coder->props->queue_size == 0)
		return ;
	while (i < coder->props->queue_size)
	{
		coder->props->queue[i - 1] = coder->props->queue[i];
		i++;
	}
	coder->props->queue_size--;
}

// void	edf_order_queue(t_props *props)
// {
// 	int aux = 0;
// 	int	i;

// 	i = 1;
// 	while (i <= props->queue_size)
// 	{
// 		if (props->queue[i] < props->queue[i - 1])
// 		{
// 			aux = props->queue[i];
// 			props->queue[i] = props->queue[i - 1];
// 			props->queue[i - 1] = aux;
// 			i = 0;
// 		}
// 		i++;
// 	}
// 	return ;
// }

// void	deque(t_coders *coder)
// {
// 	int	i;

// 	i = 1;
// 	while (i < coder->props->number_of_coders - 1 && coder->props->queue[i] != -1)
// 		coder->props->queue[i - 1] = coder->props->queue[i];
// 	coder->props->queue[i] = -1;
// }
