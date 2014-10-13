INC=/usr/local/ssl/include/
LIB=/usr/local/ssl/lib/

all:
	gcc -I $(INC) -L $(LIB) -o vpnclient vpnclient.c utilities.c datautilities.c clictl.c -lssl -lcrypto -ldl -g
