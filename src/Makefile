# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# 定义源文件和目标文件
CLIENT_SRC = dns_client/main.c dns_client/client.c common/utils.c
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
CLIENT_TARGET = ../bin/dns_client

RELAY_SRC = dns_relay/main.c dns_relay/relay.c common/utils.c
RELAY_OBJ = $(RELAY_SRC:.c=.o)
RELAY_TARGET = ../bin/dns_relay

SERVER_SRC = dns_server/main.c dns_server/server.c common/utils.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_TARGET = ../bin/dns_server

# 默认目标
all: $(CLIENT_TARGET) $(RELAY_TARGET) $(SERVER_TARGET)

# 编译 DNS client
$(CLIENT_TARGET): $(CLIENT_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译 DNS relay
$(RELAY_TARGET): $(RELAY_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译 DNS server
$(SERVER_TARGET): $(SERVER_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 编译对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成文件
clean:
	rm -f $(CLIENT_OBJ) $(RELAY_OBJ) $(SERVER_OBJ)
	rm -f $(CLIENT_TARGET) $(RELAY_TARGET) $(SERVER_TARGET)

# 伪目标
.PHONY: all clean
