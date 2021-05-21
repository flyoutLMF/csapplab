# 在这里写你想写的代码
gcc -shared -fPIC -o libhit.so hit.c -ldl
export LD_PRELOAD=$PWD/libhit.so
# 也可以修改密码
Task5_Passwd=weak_password
# 不要改动下面的代码
echo $Task5_Passwd
echo $Task5_Passwd | $PWD/login

unset LD_PRELOAD
rm *.so
