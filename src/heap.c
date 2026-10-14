#include "../lib/codexion.h"

static int	req_before(t_req a, t_req b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.id < b.id);
}

static void	sift_up(t_dongle *dongle, int i)
{
	t_req	tmp;
	int		parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!req_before(dongle->heap[i], dongle->heap[parent]))
			break ;
		tmp = dongle->heap[parent];
		dongle->heap[parent] = dongle->heap[i];
		dongle->heap[i] = tmp;
		i = parent;
	}
}

static void	sift_down(t_dongle *dongle, int i)
{
	t_req	tmp;
	int		child;

	while (2 * i + 1 < dongle->heap_size)
	{
		child = 2 * i + 1;
		if (child + 1 < dongle->heap_size
			&& req_before(dongle->heap[child + 1], dongle->heap[child]))
			child++;
		if (!req_before(dongle->heap[child], dongle->heap[i]))
			break ;
		tmp = dongle->heap[child];
		dongle->heap[child] = dongle->heap[i];
		dongle->heap[i] = tmp;
		i = child;
	}
}

void	heap_push(t_dongle *dongle, long key, int id)
{
	dongle->heap[dongle->heap_size].key = key;
	dongle->heap[dongle->heap_size].id = id;
	dongle->heap_size++;
	sift_up(dongle, dongle->heap_size - 1);
}

void	heap_remove(t_dongle *dongle, int id)
{
	int	i;

	i = 0;
	while (i < dongle->heap_size && dongle->heap[i].id != id)
		i++;
	if (i == dongle->heap_size)
		return ;
	dongle->heap_size--;
	if (i == dongle->heap_size)
		return ;
	dongle->heap[i] = dongle->heap[dongle->heap_size];
	sift_down(dongle, i);
	sift_up(dongle, i);
}
