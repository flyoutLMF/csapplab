#include "function0.h"

void func0(){
    # ifdef DEBUG
    printDebug();
    # else
    print();
    # endif
}