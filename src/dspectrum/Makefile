make:
	make dspectrum

dspectrum: gl3w
	g++ dspectrum.cpp core/core.cpp *.o -o dspectrum -lglfw -lGL -ldl -lfftw3 -lm

dspectrum-unit-test: gl3w
	g++ dspectrum.cpp core/core.cpp unit-tests/dspectrum.cpp *.o -o dspectrum -lglfw -lGL -ldl -lfftw3 -lm

gl3w: gl3w.c
	gcc -o gl3w.o -c gl3w.c

clean:
	rm -rf dspectrum
	rm -rf *.o
	rm -rf gl3w.c
