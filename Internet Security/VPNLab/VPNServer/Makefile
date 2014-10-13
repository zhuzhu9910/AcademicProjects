INC=/usr/local/ssl/include/
LIB=/usr/local/ssl/lib/

all:
	gcc -I $(INC) -L $(LIB) -o vpnserver vpnserver.c utilities.c datautilities.c servctl.c -lssl -lcrypto -ldl -g
