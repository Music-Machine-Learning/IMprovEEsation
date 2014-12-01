COREDIR:=core
SOURCE:=reportITA
CORES:=$(wildcard ${COREDIR}/*.tex)

all: ${SOURCE}.pdf

.PHONY: all clean

%.pdf: ${CORES} ${SOURCE}.tex
	pdflatex ${SOURCE}.tex && rm *.aux *.log *.out

clean:
	rm *.aux *.log *.out *.pdf core/*.aux core/*.out 2> /dev/null || true
