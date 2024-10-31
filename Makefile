# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: amakinen <amakinen@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/26 13:32:33 by amakinen          #+#    #+#              #
#    Updated: 2024/10/31 18:08:09 by amakinen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minitalk

# Directories
OBJDIR := obj
SRCDIR := src

# Project files and targets
SRCS_C := $(addprefix $(SRCDIR)/,\
	client_main.c \
	send.c \
	util_parse_int.c \
	util_strlen.c \
)
SRCS_S := $(addprefix $(SRCDIR)/,\
	server_main.c \
	receive.c \
	util_num_to_str.c \
)
SRCS_SHARED := $(addprefix $(SRCDIR)/,\
	signals.c \
	util_write.c \
)
OBJS_C := $(SRCS_C:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJS_S := $(SRCS_S:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJS_SHARED := $(SRCS_SHARED:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJS := $(OBJS_C) $(OBJS_S) $(OBJS_SHARED)
BINS := client server
client: $(OBJS_C) $(OBJS_SHARED)
server: $(OBJS_S) $(OBJS_SHARED)
$(NAME): client server
.PHONY: $(NAME)

# Generic utility targets
.DEFAULT_GOAL := all

.PHONY: all clean fclean re

all: $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(BINS)

re: fclean all

# Default compiler flags that apply to all targets
def_CFLAGS := -Wall -Wextra -Werror
def_CPPFLAGS := -MMD -MP -I include

# Add sanitizer flags if requested
ifneq (,$(strip $(SANITIZE)))
	def_CFLAGS += -g -fsanitize=$(SANITIZE)
	def_LDFLAGS += -g -fsanitize=$(SANITIZE)
endif

# Combine default def_FLAGS, target specific tgt_FLAGS and user-supplied FLAGS
# into one _FLAGS variable to be used in recipes
flagvars = CFLAGS CPPFLAGS LDFLAGS LDLIBS
$(foreach v,$(flagvars),$(eval _$v := $$(strip $$(def_$v) $$(tgt_$v) $$($v))))

# Recipe command to ensure directory for target exists
mktargetdir = @mkdir -p $(@D)

# Default recipes for each type of target
$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(mktargetdir)
	$(CC) $(_CPPFLAGS) $(_CFLAGS) -c $< -o $@

$(BINS):
	$(mktargetdir)
	$(CC) $(_LDFLAGS) $^ $(_LDLIBS) -o $@

# Inform make that object files don't need to be remade if the requested
# targets are up to date with respect to the source files.
.SECONDARY: $(OBJS)

# Dependency files to handle #include dependencies
DEPS = $(OBJS:.o=.d)
-include $(DEPS)
