# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tdeborde <tdeborde@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/09/25 17:14:19 by cmaxime           #+#    #+#              #
#    Updated: 2019/09/26 13:51:04 by tdeborde         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = rt_vox

BREW_PATH = $(HOME)/.brew
BASEPATH = $(CURDIR)

include colors.mk
include files.mk

SRC_FILES = $(addprefix src/,$(SRC))

OBJ := $(subst src,.obj,$(subst .cpp,.o,$(SRC_FILES)))

CXX := g++
CXXINCLUDE := -I include 
CXXFLAGS := -g -Wall -Wextra -Werror # -Weverything
CXXIGNORE := -Wno-unreachable-code-return -Wno-unused-function \
			-Wno-reserved-id-macro -Wno-unused-parameter
CXX_FLAGS_DEPS := -MD

ifeq ($(shell [ -d $(BREW_PATH) ]; echo $$?), 0) 
    CXXINCLUDE += -isystem $(BREW_PATH)/include
endif

CXXSRC := $(CXXINCLUDE) $(CXXFLAGS) $(CXXIGNORE) $(CXX_FLAGS_DEPS) -D BASEPATH=\"$(BASEPATH)\"

LIB := -lm
ifeq ($(shell [ -d $(BREW_PATH) ]; echo $$?), 0) 
	LIB += -L $(BREW_PATH)/lib
endif
LIB += -lglfw

# LIB := $(LIB) -lmlx -framework OpenGL -framework AppKit

all : $(NAME)

$(NAME): .obj $(OBJ)
	@$(MAKE) compile

compile: .obj $(OBJ)
	@echo "$(LIB)"
	@$(CXX) $(OBJ) -o $(NAME) $(LIB) -D BASEPATH=\"$(BASEPATH)\"
	@echo "$(BOLD)[ $(NAME) ] compiled$(NORMAL)"

.obj:
	@mkdir .obj

.obj/%.o: src/%.cpp
	@printf "\\t%sCC%s\\t" "$(_GREEN)" "$(_END)"
	@$(CXX) -o $@ $(CXXSRC) -c $<
	@printf "%s\\n" "$<"

#add dependencies files (*.d)
DEP_FILES = $(SRC:%.cpp=%.d)
-include $(DEP_FILES)

clean:
	@rm -rf .obj
	@printf "%s clean\\n" "$(NAME)"

fclean: clean
	@rm -f $(NAME)
	@printf "%s fclean\\n" "$(NAME)"

re: 
	@$(MAKE) fclean 
	@$(MAKE) all


.PHONY: all clean fclean re libft compile
