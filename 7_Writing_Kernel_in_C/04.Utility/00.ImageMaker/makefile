#The list for performing builds
all: ImageMaker

#Build ImageMaker
ImageMaker: ImageMaker.c
	gcc -o $@ $<

#clear remaining files except source files.
clean:
	rm -f ImageMaker
