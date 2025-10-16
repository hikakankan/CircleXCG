# コンパイラとツール
CC = m68k-xelf-g++

# 出力ファイル名
TARGET = CirclXCG.x

# コンパイルオプション
CFLAGS = -Os -DUSE_X68000 -DPACKED_STRUCT -MMD -MP

# ソースファイル
SRCS = CircleXCG.cpp console.cpp DNString.cpp wintox.cpp CommandInterpreter.cpp

# オブジェクトファイル
OBJS = $(SRCS:.cpp=.o)

# ビルドルール
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

-include $(OBJS:.o=.d)
