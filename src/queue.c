#include "../lib/codexion.h"

static time_t   get_deadline(t_props *props, int coder_id)
{
    return (props->coders[coder_id - 1].last_compile_start
            + props->time_to_burnout / 1000);
}

static void sift_up(t_props *props, int i)
{
    int parent;
    int tmp;

    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (get_deadline(props, props->queue[parent])
            <= get_deadline(props, props->queue[i]))
            break ;
        tmp = props->queue[parent];
        props->queue[parent] = props->queue[i];
        props->queue[i] = tmp;
        i = parent;
    }
}

static void sift_down(t_props *props, int i)
{
    int left;
    int right;
    int smallest;
    int tmp;

    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        smallest = i;
        if (left < props->queue_size
            && get_deadline(props, props->queue[left])
               < get_deadline(props, props->queue[smallest]))
            smallest = left;
        if (right < props->queue_size
            && get_deadline(props, props->queue[right])
               < get_deadline(props, props->queue[smallest]))
            smallest = right;
        if (smallest == i)
            break ;
        tmp = props->queue[smallest];
        props->queue[smallest] = props->queue[i];
        props->queue[i] = tmp;
        i = smallest;
    }
}

void    add_coder_queue(t_coders *coder)
{
    int i;

    i = 0;
    while (i < coder->props->queue_size)
    {
        if (coder->props->queue[i] == coder->coder_id)
            return ;
        i++;
    }
    coder->props->queue[coder->props->queue_size] = coder->coder_id;
    coder->props->queue_size++;
    if (ft_strcmp(coder->props->scheduler, "edf") == 0)
        sift_up(coder->props, coder->props->queue_size - 1);
    // fifo: append only, order preserved
}

void    remove_coder_queue(t_coders *coder)
{
    int i;

    if (coder->props->queue_size == 0)
        return ;
    if (ft_strcmp(coder->props->scheduler, "edf") == 0)
    {
        // heap remove-root: put last element at root, sift down
        coder->props->queue[0] = coder->props->queue[coder->props->queue_size - 1];
        coder->props->queue_size--;
        if (coder->props->queue_size > 0)
            sift_down(coder->props, 0);
    }
    else
    {
        // fifo: shift array left
        i = 1;
        while (i < coder->props->queue_size)
        {
            coder->props->queue[i - 1] = coder->props->queue[i];
            i++;
        }
        coder->props->queue_size--;
    }
}
