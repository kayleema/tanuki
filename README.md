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

  ./pinpon ../関数.pin

Formal Grammar (incomplete)
----------------------
terminals: 
  LPAREN, RPAREN, COMMA, SYMBOL, END, START, SPACE, NEWL, INDENT,
  DEDENT, NUMBER, FUNC, RETURN, IF, ELSE, STRING, DOT

productions:
  text => NEWLINE statement
  text => statement text
  text => EPSILON

  statement => FUNC COMMA SYMBOL LPAREN param_list RPAREN NEWL INDENT text DEDENT
  statement => RETURN return_value
  statement => if_statement
  statement => SYMBOL ASSIGN expression
  statement => expression

  param_list => SYMBOL [COMMA SYMBOL]* | EPSILON
  if_statement => IF COMMA expression NEWL INDENT text DEDENT
  if_statement => IF COMMA expression NEWL INDENT text DEDENT ELSE NEWL INDENT text DEDENT
  return_value => expression
  return_value => EPSILON

  expression => expression_fns + expression
  expression => expression_fns - expression
  expression => expression_fns
  expression_fns => SYMBOL
  expression_fns => SYMBOL expression_fns_tail
  expression_fns => SYMBOL expression_fns_tail
  expression_fns => NUMBER
  expression_fns => NUMBER expression_fns_tail
  expression_fns_tail => LPAREN arg_list RPAREN expression_fns_tail
  expression_fns_tail => EPSILON

  arg_list => expression [COMMA expression]* | EPSILON

