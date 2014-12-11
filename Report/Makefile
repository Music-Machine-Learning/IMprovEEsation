COREDIR:=core
SOURCE:=reportITA
CORES:=$(wildcard ${COREDIR}/*.tex)

all: ${SOURCE}.pdf

.PHONY: all clean

# Execute pdflatex 2 times, workaround to get working \label and \ref tags.
%.pdf: ${CORES} ${SOURCE}.tex
	pdflatex ${SOURCE}.tex && pdflatex ${SOURCE}.tex && (rm *.log *.out || true)

clean:
	rm *.aux *.log *.out *.pdf core/*.aux core/*.out 2> /dev/null || true
