# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = 

# 定义源文件和目标文件

RELAY_SRC = dns_relay/main.c dns_relay/parser.c dns_relay/checkhosts.c common/utils.c
RELAY_OBJ = $(RELAY_SRC:.c=.o)
RELAY_TARGET = ../bin/dns_relay

TEST_SRC = test/test.c common/utils.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = ../bin/test

# 默认目标
all: $(RELAY_TARGET)

test: $(TEST_TARGET)

# 编译 DNS client
$(CLIENT_TARGET): $(CLIENT_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	rm $(CLIENT_OBJ)

# 编译 DNS relay
$(RELAY_TARGET): $(RELAY_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	rm $(RELAY_OBJ)

# 编译对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成文件
clean:
	rm -f $(CLIENT_OBJ) $(RELAY_OBJ) $(SERVER_OBJ)
	rm -f $(CLIENT_TARGET) $(RELAY_TARGET) $(SERVER_TARGET)

# 伪目标
.PHONY: all clean
