
main : 
	cd \A && make A
	cd \C && make C
	g++ main.cpp C/libC.a B/libB.a A/libA.a -o main
	

#下面的不用改
.PHONY : clean
cleanObject = main A/A.o A/libA.a C/C.o C/libC.a
clean :
	@rm ${cleanObject}  -f
