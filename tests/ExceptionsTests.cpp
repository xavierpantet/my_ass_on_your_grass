#include "ExceptionsTests.h"

using namespace std;

void returnError(int i){
  throw Exception(i);
}

void testError1(){
  try {
    returnError(ERR_INVALID_CMD);
  }
  catch(Exception& e){
    e.print_error();
  }
}

void test(int i){
  returnError(i);
}

void testError2(){
  try {
    test(ERR_LOGIN_REQUIRED);
  }
  catch(Exception& e){
    e.print_error();
  }
}
