.DEFAULT_GOAL=all

NAME=bmp_typewriter
CC=cc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -MMD -MP -MF $(DEPS_DIR)/$*.d

RM=rm -fr

SRCS=\
	 serialize.c \
	 util.c \
	 letter.c \

OBJS_DIR=.objs
OBJS=$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))

DEPS_DIR=.deps
DEPS=$(addprefix $(DEPS_DIR)/, $(SRCS:.c=.d))
-include $(DEPS)

ifeq ($(MAKECMDGOALS),debug)
	CXXFLAGS+=-g3 -fsanitize=address
endif

all : $(NAME)

$(NAME): $(OBJS)
	$(LINK.c) $(OUTPUT_OPTION) $(OBJS)

$(OBJS_DIR)/%.o: %.c | $(OBJS_DIR) $(DEPS_DIR)
	$(COMPILE.c) -o $@ $<

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(DEPS_DIR):
	mkdir -p $(DEPS_DIR)

debug: all

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(OBJS_DIR) $(DEPS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
