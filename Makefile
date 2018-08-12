# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrassokh <mrassokh@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/12 09:31:31 by mrassokh          #+#    #+#              #
#    Updated: 2017/06/03 14:38:10 by mrassokh         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =	npuzzle
#[----------------------------------HEADERS-----------------------------------]#

HEADERS_PATH =	./includes/

INC =-I$(HEADERS_PATH)

#[---------------------------------LIBRARIES----------------------------------]#

LIBPATH = ./static_libs/

LIBFT = -lft

LIBS = $(LIBFT)
LIBFTPATH = $(LIBPATH)libft
LIBFTINC = $(LIBFTPATH)/includes
LIBSINC = -I$(LIBFTINC)
LIBSDIR = -L$(LIBPATH)
LIBRIRIES = $(LIBS) $(LIBSINC) $(LIBSDIR)
#[--------------------------------COMPILATION---------------------------------]#

CC = gcc
CCXX = clang++
FLAGS	= -Wall -Wextra -Werror -std=c++14 -O3

#[----------------------------------SOURCES-----------------------------------]#

SRC_PATH 	= ./src/

ROOT = main.cpp

FUNCTIONS = State.cpp NPuzzle.cpp CustomExeptions.cpp Parser.cpp


SRC = $(ROOT) $(FUNCTIONS)
OBJ_DIR = ./obj/
OBJ = $(addprefix $(OBJ_DIR),$(SRC:.cpp=.o))


#[-----------------------------------COLORS-----------------------------------]\
#

GREEN                   =       \033[32m
RESET                   =       \033[39m
YELLOW                  =       \033[33m
RED                     =       \033[31m


#[------------------------------------MAKE------------------------------------]\
#
.PHONY: make_libs

vpath %.cpp $(SRC_PATH)
vpath %.hpp $(HEADERS_PATH)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "\033[0;32mCompile program ...\033[0m"
	@ $(CCXX) $(FLAGS) -o $(NAME) $(OBJ) $(INC) $(RUNFLAGS) -g
	@ echo "$(GREEN)[$(NAME)]$(RESET)"

$(OBJ_DIR)%.o: %.cpp
	@echo "\033[0;32mCreating object file\033[0m \033[31m$@\033[0m"
	@ $(CCXX) $(FLAGS) -c $< -o $@  $(INC) -g
	@ printf ' $(GREEN)[add]$(RESET) $<\n'

$(OBJ): | $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

clean:
	@ rm -f $(OBJ)
	@ rm -rf $(OBJ_DIR)
	@ echo "$(YELLOW)[cleanRootDirectory]$(RESET)"

fclean: clean
	@ rm -f $(NAME) $(SDLLIB) $(SFMLLIB) $(NCURSESLIB)
	@ echo "$(RED)[fclean]$(RESET)"

re: fclean all
