BINNARY_NAME=auto_aim

all: main.o ${BINNARY_NAME}

%.o:	%.cpp
	g++ -o $@ -c $^ `pkg-config --cflags --libs opencv4`

${BINNARY_NAME}: main.o
	g++ -o $@ $^ `pkg-config --cflags --libs opencv4`

run:
	libcamera-vid --inline --width 640 --height 480 --nopreview --timeout 0 -o - | ./${BINNARY_NAME}

clean:
	rm -rf ${BINNARY_NAME}
