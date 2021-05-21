debug=False

main0:function0.cpp function1.cpp shared.cpp
	@#添加编译命令
	g++ main0.cpp function0.cpp function1.cpp shared.cpp -o main0
	
main1:function0.cpp function1.cpp shared.cpp
	@#添加编译命令

ifeq ($(debug),True)
	@#g++ -c shared.cpp -DDEBUG -o shared.o
	@#g++ main1.cpp function0.cpp function1.cpp shared.o -o main1
	g++ main1.cpp function0.cpp function1.cpp shared.cpp -DDEBUG -o main1
else
	g++ main1.cpp function0.cpp function1.cpp shared.cpp -o main1
endif
	
#下面的不用改
.PHONY : clean
cleanObject = main0 main1 *.o
clean :
	@rm ${cleanObject}  -f
