NAME=	programmequifaitdestrucs
SRC=	programmequifaitdestrucs.c
OBJ=	$(SRC:.c=.o)
CC=gcc
CFLAGS=-W -Wall 
DEPS = programmequifaitdestrucs.h
LIBS=	-lcrypto
RM= @rm -fv
$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBS)
clean:
	$(RM) $(OBJ)
