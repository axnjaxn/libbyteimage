BIN = types.o byteimage.o bitimage.o component.o hslimage.o matrix.o spline.o ransac.o quality.o kernel.o template.o render.o palette.o
INC = types.h byteimage.h bitimage.h component.h hslimage.h matrix.h spline.h ransac.h quality.h kernel.h template.h render.h palette.h
TESTS = tests/imgtest tests/cholestest tests/svdtest tests/nulltest

CFG_CFLAGS = -std=c++11
CFG_LIBS =

ifneq (, $(shell sdl2-config --version))
BIN += byteimage_sdl2.o
INC += byteimage_sdl2.h
CFG_CFLAGS += `sdl2-config --cflags`
CFG_LIBS += `sdl2-config --libs`
TESTS += tests/sdl2test tests/kerneltest tests/templatetest tests/matrixtest tests/splinetest tests/ransactest tests/rendertest tests/beziertest tests/palettetest tests/hulltest
else
CFG_CFLAGS += -D_BYTEIMAGE_NO_SDL2
endif

ifneq (, $(shell pkg-config opencv --modversion))
BIN += video.o
INC += video.h
CFG_CFLAGS += `pkg-config opencv --cflags`
CFG_LIBS += `pkg-config opencv --libs`
TESTS += tests/vidtest
else
CFG_CFLAGS += -D_BYTEIMAGE_NO_OPENCV
endif

ifneq (, $(shell pkg-config Magick++ --modversion))
CFG_CFLAGS += `pkg-config Magick++ --cflags`
CFG_LIBS += `pkg-config Magick++ --libs`
else
CFG_CFLAGS += -D_BYTEIMAGE_NO_MAGICK
endif

ifneq (, $(shell freetype-config --version))
BIN += font.o plotter.o
INC += font.h plotter.h
CFG_CFLAGS += `freetype-config --cflags`
CFG_LIBS += `freetype-config --libs`
TESTS += tests/fonttest tests/graphtest tests/morphtest tests/componenttest
else
CFG_CFLAGS += -D_BYTEIMAGE_NO_FREETYPE
endif

CFLAGS = -O3 -Wno-unused-result $(CFG_CFLAGS)

all: byteimage-config libbyteimage.a

byteimage-config:
	echo "#!/bin/bash" > byteimage-config
	echo "INC=\""$(INC)"\"" >> byteimage-config
	echo "CFLAGS=\""$(CFG_CFLAGS)"\"" >> byteimage-config
	echo "LIBS=\"-lbyteimage "$(CFG_LIBS)"\"" >> byteimage-config
	cat script.sh >> byteimage-config
	chmod a+x byteimage-config

libbyteimage.a: $(BIN)
	ar rcs libbyteimage.a $(BIN)

types.o: types.h types.cpp
	$(CXX) -c types.cpp $(CFLAGS)

byteimage.o: types.h byteimage.h byteimage.cpp
	$(CXX) -c byteimage.cpp $(CFLAGS)

byteimage_sdl2.o: byteimage.h byteimage_sdl2.h byteimage_sdl2.cpp
	$(CXX) -c byteimage_sdl2.cpp $(CFLAGS)

video.o: byteimage.h video.h video.cpp
	$(CXX) -c video.cpp $(CFLAGS)

bitimage.o: byteimage.h bitimage.h bitimage.cpp
	$(CXX) -c bitimage.cpp $(CFLAGS)

component.o: bitimage.h component.h component.cpp
	$(CXX) -c component.cpp $(CFLAGS)

hslimage.o: byteimage.h hslimage.h hslimage.cpp
	$(CXX) -c hslimage.cpp $(CFLAGS)

matrix.o: matrix.h matrix.cpp
	$(CXX) -c matrix.cpp $(CFLAGS) 

spline.o: matrix.h spline.h spline.cpp
	$(CXX) -c spline.cpp $(CFLAGS) 

ransac.o: matrix.h ransac.h ransac.cpp
	$(CXX) -c ransac.cpp $(CFLAGS)

quality.o: byteimage.h quality.h quality.cpp
	$(CXX) -c quality.cpp $(CFLAGS) 

kernel.o: byteimage.h kernel.h kernel.cpp
	$(CXX) -c kernel.cpp $(CFLAGS) 

template.o: byteimage.h template.h template.cpp
	$(CXX) -c template.cpp $(CFLAGS) 

render.o: byteimage.h matrix.h render.h render.cpp
	$(CXX) -c render.cpp $(CFLAGS) 

font.o: byteimage.h font.h font.cpp
	$(CXX) -c font.cpp $(CFLAGS) -Wno-parentheses

plotter.o: byteimage.h matrix.h font.h render.h plotter.h plotter.cpp
	$(CXX) -c plotter.cpp $(CFLAGS)

palette.o: byteimage.h palette.h palette.cpp
	$(CXX) -c palette.cpp $(CFLAGS)

clean: clean-tests
	rm -f *~ $(BIN) libbyteimage.a byteimage-config

install: byteimage-config libbyteimage.a $(INC)
	cp byteimage-config /usr/local/bin/byteimage-config
	cp libbyteimage.a /usr/local/lib/libbyteimage.a
	mkdir -p /usr/local/include/byteimage
	cp $(INC) -t /usr/local/include/byteimage

tests: $(TESTS)

tests/imgtest: tests/imgtest.cpp
	$(CXX) tests/imgtest.cpp -o tests/imgtest -g `byteimage-config --cflags --libs`

tests/sdl2test: tests/sdl2test.cpp
	$(CXX) tests/sdl2test.cpp -o tests/sdl2test -g `byteimage-config --cflags --libs`

tests/vidtest: tests/vidtest.cpp
	$(CXX) tests/vidtest.cpp -o tests/vidtest -g `byteimage-config --cflags --libs`

tests/kerneltest: tests/kerneltest.cpp
	$(CXX) tests/kerneltest.cpp -o tests/kerneltest -g `byteimage-config --cflags --libs`

tests/templatetest: tests/templatetest.cpp
	$(CXX) tests/templatetest.cpp -o tests/templatetest -g `byteimage-config --cflags --libs`

tests/matrixtest: tests/matrixtest.cpp
	$(CXX) tests/matrixtest.cpp -o tests/matrixtest -g `byteimage-config --cflags --libs`

tests/splinetest: tests/splinetest.cpp
	$(CXX) tests/splinetest.cpp -o tests/splinetest -g `byteimage-config --cflags --libs`

tests/ransactest: tests/ransactest.cpp
	$(CXX) tests/ransactest.cpp -o tests/ransactest -g `byteimage-config --cflags --libs`

tests/rendertest: tests/rendertest.cpp
	$(CXX) tests/rendertest.cpp -o tests/rendertest -g `byteimage-config --cflags --libs`

tests/beziertest: tests/beziertest.cpp
	$(CXX) tests/beziertest.cpp -o tests/beziertest -g `byteimage-config --cflags --libs`

tests/cholestest: tests/cholestest.cpp
	$(CXX) tests/cholestest.cpp -o tests/cholestest -g `byteimage-config --cflags --libs`

tests/svdtest: tests/svdtest.cpp
	$(CXX) tests/svdtest.cpp -o tests/svdtest -g `byteimage-config --cflags --libs`

tests/nulltest: tests/nulltest.cpp
	$(CXX) tests/nulltest.cpp -o tests/nulltest -g `byteimage-config --cflags --libs`

tests/fonttest: tests/fonttest.cpp
	$(CXX) tests/fonttest.cpp -o tests/fonttest -g `byteimage-config --cflags --libs`

tests/graphtest: tests/graphtest.cpp
	$(CXX) tests/graphtest.cpp -o tests/graphtest -g `byteimage-config --cflags --libs`

tests/morphtest: tests/morphtest.cpp
	$(CXX) tests/morphtest.cpp -o tests/morphtest -g `byteimage-config --cflags --libs`

tests/componenttest: tests/componenttest.cpp
	$(CXX) tests/componenttest.cpp -o tests/componenttest -g `byteimage-config --cflags --libs`

tests/palettetest: tests/palettetest.cpp
	$(CXX) tests/palettetest.cpp -o tests/palettetest -g `byteimage-config --cflags --libs`

tests/hulltest: tests/hulltest.cpp
	$(CXX) tests/hulltest.cpp -o tests/hulltest -g `byteimage-config --cflags --libs`

clean-tests:
	rm -f tests/*~ $(TESTS)
