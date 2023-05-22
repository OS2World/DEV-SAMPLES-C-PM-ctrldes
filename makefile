# make makefile
#
# Tools used:
#  Compile::Resource Compiler
#  Compile::GNU C
#  Make: make
all : driver.exe

driver.exe : driver.obj controls.lib driver.def
	gcc -Zomf -Zmap -lcontrols driver.obj driver.def -o driver.exe
	
driver.obj : driver.c
	gcc -Wall -Zomf -c -O2 driver.c -o driver.obj

controls.dll : controls.obj controls.def
	gcc -Zdll -Zomf -Zmap  controls.obj controls.def -o controls.dll
	
controls.obj : controls.c controls.h
	gcc -Wall -Zomf -c -O2 controls.c -o controls.obj

controls.lib : controls.obj controls.def controls.dll
       $(shell emximp -o controls.lib controls.def)

clean :
	rm -rf *exe *res *obj *lib *dll *map