CFLAGS = -O3 -Wno-unused-result

CFG_CFLAGS = `sdl2-config --cflags` `pkg-config opencv --cflags`
CFG_LIBS = `sdl2-config --libs` `pkg-config opencv --libs`

ifneq (, $(shell Magick++-config --version))
CFG_CFLAGS += `Magick++-config --cppflags --cxxflags`
CFG_LIBS += `Magick++-config --libs`
else
CFG_CFLAGS += -D_BYTEIMAGE_NO_MAGICK
endif

CFLAGS += $(CFG_CFLAGS)

BIN = byteimage.o byteimage_sdl2.o bytevideo.o quality.o kernel.o
INC = byteimage.h byteimage_sdl2.h bytevideo.h quality.h kernel.h

all: byteimage-config libbyteimage.a

byteimage-config:
	echo "#!/bin/bash" > byteimage-config
	echo "CFLAGS=\""$(CFG_CFLAGS)"\"" >> byteimage-config
	echo "LIBS=\"-lbyteimage "$(CFG_LIBS)"\"" >> byteimage-config
	cat script.sh >> byteimage-config
	chmod a+x byteimage-config

libbyteimage.a: $(BIN)
	ar rcs libbyteimage.a $(BIN)

byteimage.o: byteimage.h byteimage.cpp
	$(CXX) -c byteimage.cpp $(CFLAGS)

byteimage_sdl2.o: byteimage.h byteimage_sdl2.h byteimage_sdl2.cpp
	$(CXX) -c byteimage_sdl2.cpp $(CFLAGS)

bytevideo.o: byteimage.h bytevideo.h bytevideo.cpp
	$(CXX) -c bytevideo.cpp $(CFLAGS)

quality.o: byteimage.h quality.h quality.cpp
	$(CXX) -c quality.cpp $(CFLAGS) 

kernel.o: byteimage.h kernel.h kernel.cpp
	$(CXX) -c kernel.cpp $(CFLAGS) 

clean: clean-tests
	rm -f *~ $(BIN) libbyteimage.a byteimage-config

install: byteimage-config libbyteimage.a $(INC)
	cp byteimage-config /usr/local/bin/byteimage-config
	cp libbyteimage.a /usr/local/lib/libbyteimage.a
	mkdir -p /usr/local/include/byteimage
	cp $(INC) -t /usr/local/include/byteimage

TESTS = tests/imgtest tests/sdl2test tests/vidtest tests/kerneltest

tests: $(TESTS)

tests/imgtest: tests/imgtest.cpp
	$(CXX) tests/imgtest.cpp -o tests/imgtest `byteimage-config --cflags --libs`

tests/sdl2test: tests/sdl2test.cpp
	$(CXX) tests/sdl2test.cpp -o tests/sdl2test `byteimage-config --cflags --libs`

tests/vidtest: tests/vidtest.cpp
	$(CXX) tests/vidtest.cpp -o tests/vidtest `byteimage-config --cflags --libs`

tests/kerneltest: tests/kerneltest.cpp
	$(CXX) tests/kerneltest.cpp -o tests/kerneltest `byteimage-config --cflags --libs`

clean-tests:
	rm -f tests/*~ $(TESTS)
