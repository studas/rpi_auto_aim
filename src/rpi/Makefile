CC = g++
CPPFLAGS = -O3 -std=c++17 `pkg-config --cflags --libs opencv4`
SRCS = main.cpp ui.cpp process.cpp capture.cpp pantilt.cpp control_error.cpp
OBJS = $(SRCS:.cpp=.o)
BINNARY_NAME = auto_aim

all: $(BINNARY_NAME)

$(BINNARY_NAME): $(OBJS)
	$(CC) $(OBJS) -o $(BINNARY_NAME) $(CPPFLAGS)

run: $(BINNARY_NAME)
	libcamera-vid --inline --width 640 --height 480  --timeout 0 -o - | ./${BINNARY_NAME}

clean:
	rm -f *.o $(BINNARY_NAME)
