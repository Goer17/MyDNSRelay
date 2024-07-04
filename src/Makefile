# 定义编译器和编译选项
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = 

# 定义源文件和目标文件

RELAY_SRC = dns_relay/main.c dns_relay/parser.c dns_relay/checkhosts.c common/utils.c dns_relay/cache.c common/trie.c common/logger.c
RELAY_OBJ = $(RELAY_SRC:.c=.o)
RELAY_TARGET = ../bin/myrelay

TEST_SRC = dns_relay/parser.c test/test.c common/utils.c dns_relay/cache.c common/trie.c common/logger.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = ../bin/test

# 默认目标
all: $(RELAY_TARGET)

test: $(TEST_TARGET)

# 编译 DNS relay
$(RELAY_TARGET): $(RELAY_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	rm $(RELAY_OBJ)

$(TEST_TARGET): $(TEST_OBJ)
	mkdir -p ../bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	rm $(TEST_OBJ)

dns_relay/cache.o: common/trie.o
common/trie.o: dns_relay/parser.o

# 编译对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成文件
clean:
	rm -f $(CLIENT_OBJ) $(RELAY_OBJ) $(SERVER_OBJ)
	rm -f $(CLIENT_TARGET) $(RELAY_TARGET) $(SERVER_TARGET)

# 伪目标
.PHONY: all clean
