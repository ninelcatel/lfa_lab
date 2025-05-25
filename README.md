Temele de laborator pentru LFA:

DFA si NFA sunt realizate in Python.

PDA si Masina Turing sunt realizate in C++ cu elemente de OOP.

Fisierele de tip text sunt inputuri ce reprezinta automate, fisierele sursa in sine simuleaza automatele respective.

Pentru a rula fiecare program este necesar sa fie dat ca  input numele fisierului (! cu tot cu extensia formatului)  

Rulat si verificat pe Arch Linux versunea 6.14.2 insa ar trebui sa functioneze pe orice fel de distributie (Nu este verificat pe windows dar ar trebui sa functioneze)

In terminalul folderului dorit: 

git clone https://github.com/ninelcatel/lfa_lab.git

cd lfa_lab

pt c++:

g++ <file.cpp> -o <executable name> && ./<executable name> [< "filename.format"]

pt python

python DFA_NFA.py [< "filename.format"]
