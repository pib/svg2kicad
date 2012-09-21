all: svg2kicad

svg2kicad:
	gcc -o svg2kicad svg_kicad.c -lm