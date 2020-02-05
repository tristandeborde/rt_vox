# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: blefeuvr <blefeuvrstudent.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/09/25 17:14:19 by cmaxime           #+#    #+#              #
#    Updated: 2019/11/25 17:36:57 by blefeuvr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = rt_vox

BASEPATH = $(CURDIR)

include colors.mk
include files.mk

LIBS = glm GL
SRC_FILES = $(addprefix src/,$(SRC))

OBJ := $(subst src,.obj,$(subst .c,.o,$(subst .cpp,.o,$(SRC_FILES))))
VPATH := lib/GL
CXX := g++
CXXINCLUDE := -Iinclude $(LIBS:%=-Ilib/%/include)
CXXFLAGS := -g -Wall -Wextra -Werror -std=c++11
CXXIGNORE := -Wno-unreachable-code-return -Wno-unused-function \
			-Wno-reserved-id-macro -Wno-unused-parameter
CXX_FLAGS_DEPS := -MD

CXXSRC := $(CXXINCLUDE) $(CXXFLAGS) $(CXXIGNORE) $(CXX_FLAGS_DEPS) -D BASEPATH=\"$(BASEPATH)\"

LIB := -ldl -lglfw

all : $(NAME)

$(NAME): .obj $(OBJ)
	$(MAKE) compile

compile: .obj $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LIB) -D BASEPATH=\"$(BASEPATH)\"
	echo "$(NAME) compiled"

.obj:
	mkdir .obj

.obj/%.o: src/%.cpp
	$(CXX) -o $ $(CXXSRC) -c $<
	printf "\\t%sCC%s\\t%s\\n" "$(_GREEN)" "$(_END)" "$<"

#add dependencies files (*.d)
DEP_FILES = $(SRC:%.cpp=%.d)
-include $(DEP_FILES)

.PHONY:install
install:
	install/install_libs.sh

.PHONY:clean
clean:
	rm -rf .obj
	printf "%s clean\\n" "$(NAME)"

.PHONY:fclean
fclean: clean
	rm -f $(NAME)
	printf "%s fclean\\n" "$(NAME)"

re: 
	$(MAKE) fclean 
	$(MAKE) all


.PHONY: all clean fclean re libft compile
