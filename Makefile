#basics to compile the code
NAME = mini_serv
CC = cc
CFLAGS = -Wall -Werror -Wextra 

#arguments for the run target:
ARGS = "8080"

#sources
SRCS = mini_serv.c \
       mini_serv_helpers.c

OBJS = $(SRCS:%.c=obj/%.o)

all: $(NAME)

#to create a program:
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	@echo -- prog created, try it by using ./mini_serv

obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf obj
	@echo -- Deleting All .o

fclean: clean
	@rm -f $(NAME)
	@echo -- Deleting executables

re: fclean all

run: fclean all
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && $(NAME) $(ARGS)

debug: CFLAGS += -DDEBUG -g
debug: fclean all
	@echo "$(BOLD)$(YELLOW)Debug build complete.$(RESET)"

debugrun: debug
	@echo
	@PATH=".$${PATH:+:$${PATH}}" && $(VALGRIND) $(NAME) $(ARGS)

.PHONY: all run clean fclean re
