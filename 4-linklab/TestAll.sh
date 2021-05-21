echo 
echo Task0
cd Task0
make clean
make main
./main
make clean

echo 
echo Task1
cd ../Task1
make clean
make main0
./main0
make main1
./main1
make clean
make main1 debug=True
./main1
make clean

echo 
echo Task2
cd ../Task2
make clean
make main
size main
./main
make clean

echo 
echo Task3
cd ../Task3
make clean
make main
size main
./main
make clean

echo 
echo Task4
cd ../Task4
make clean
make main
./main
make clean

echo 
echo Task5
cd ../Task5
sh Task5.sh
