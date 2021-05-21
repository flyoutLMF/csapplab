
main : 
	cd A && make A
	cd C && make C	
	g++ main.cpp ./libA.so ./libB.so ./libC.so -o main
	
#下面的不用改
.PHONY : clean
cleanObject = main libA.so libC.so
clean :
	@rm ${cleanObject}  -f
