NAME = webserv

CC = clang++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRC_NAME = webserv.cpp Config.cpp IPAddress.cpp Socket.cpp Server.cpp Session.cpp AllConfigs.cpp
HPP_FILES = Config.hpp IPAddress.hpp Socket.hpp Server.hpp Session.hpp AllConfigs.hpp
OBJ_PATH = ./obj/

OBJ_NAME = $(SRC_NAME:.cpp=.o)
OBJ = $(addprefix $(OBJ_PATH), $(OBJ_NAME))

$(OBJ_PATH)%.o: %.cpp $(HPP_FILES)

	@mkdir -p $(OBJ_PATH)
	@printf  "\r%-25s\033[34m[$<]\033[0m\n" "$(NAME): $@"
	@$(CC) $(FLAGS) -o $@ -c $<
	@printf "\033[A\033[2K"

all: $(NAME) $(HPP_FILES)

$(NAME): $(OBJ) $(HPP_FILES)

	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	@printf  "%-25s\033[32m[✔]\033[0m\n" "$(NAME): $@"

clean:

	@rm -rf $(OBJ_PATH)
	@printf  "%-25s\033[32m[✔]\033[0m\n" "$(NAME): $@"

fclean: clean

	@rm -f $(NAME)
	@printf  "%-25s\033[32m[✔]\033[0m\n" "$(NAME): $@"

re: fclean all

test: all
	@printf  "\033[32mTEST:\033[0m\n"
	./$(NAME)

leaks: all
	@leaks --atExit -- ./$(NAME)

.PHONY:				all clean fclean re test leaks
