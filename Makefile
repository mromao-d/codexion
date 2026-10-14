NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRC_DIR	= src
HDR		= lib/codexion.h
SRC		= $(SRC_DIR)/codexion.c \
		  $(SRC_DIR)/parse.c \
		  $(SRC_DIR)/init.c \
		  $(SRC_DIR)/heap.c \
		  $(SRC_DIR)/dongle.c \
		  $(SRC_DIR)/coder.c \
		  $(SRC_DIR)/monitor.c \
		  $(SRC_DIR)/utils.c

OBJ		= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
