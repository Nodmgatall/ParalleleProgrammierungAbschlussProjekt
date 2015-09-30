OUT = Ausarbeitung

all: pdf
	evince ${OUT}.pdf

pdf: ${OUT}.tex
	cd build/ && pdflatex -shell-escape ../${OUT}.tex && mv ${OUT}.pdf ../
