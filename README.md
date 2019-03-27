ピンポン　プログラミング言語
===========================

Compilation
-----------
  mkdir build
  cd build
  cmake ..
  make
  ./pinpon

テストする
---------
  cd build
  CTEST_OUTPUT_ON_FAILURE=1 make test

or:
  make pinpon_test && ./pinpon_test

combined cmake and make and run test:
  cmake .. && make pinpon_test && ./pinpon_test

呼び方
--------
  ./pinpon <ファイル名.pin>

  ./pinpon ../例文.pin
