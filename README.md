ピンポン　プログラミング言語
===========================

Compilation
-----------
```
  mkdir build
  cd build
  cmake ..
  make
  ./pinpon
```

テストする
---------
```
  cd build
  CTEST_OUTPUT_ON_FAILURE=1 make test
```

or:
```
  make pinpon_test && ./pinpon_test
```

combined cmake and make and run test:
```
  cmake .. && make pinpon_test && ./pinpon_test
```

呼び方
--------
```
  ./pinpon <ファイル名.pin>

  ./pinpon ../関数.pin
```

Formal Grammar (incomplete)
----------------------
terminals:
```
  LPAREN, RPAREN, COMMA, SYMBOL, END, START, SPACE, NEWL, INDENT,
  DEDENT, NUMBER, FUNC, RETURN, IF, ELSE, STRING, DOT, ...
```

productions:
```
  text => NEWLINE statement
  text => statement text
  text => EPSILON

  statement => FUNC COMMA SYMBOL LPAREN param_list RPAREN NEWL INDENT text DEDENT
  statement => RETURN return_value
  statement => if_statement
  statement => SYMBOL ASSIGN infix_expression
  statement => infix_expression

  param_list => SYMBOL [COMMA SYMBOL]* | EPSILON
  if_statement => IF COMMA infix_expression NEWL INDENT text DEDENT
  if_statement => IF COMMA infix_expression NEWL INDENT text DEDENT ELSE NEWL INDENT text DEDENT
  return_value => infix_expression
  return_value => EPSILON

  infix_expression => infix_comparison_expression
  infix_comparison_expression => infix_additive_expression {==,!=,>,<,>=,<=} infix_additive_expression
  infix_additive_expression => expression {+,-} expression [ {+,-} expression ]...

  expression => STRING
  expression => SYMBOL
  expression => SYMBOL expression_tail
  expression => NUMBER
  expression => NUMBER expression_tail

  expression_tail => LPAREN arg_list RPAREN expression_tail
  expression_tail => EPSILON

  arg_list => expression [COMMA expression]* | EPSILON
```

まだ実装されてない事
------------------
* keyword argument unpacking with defaults
* variable length non-keyword arguments
* dynamic dict lookup
* dynamic dict set
* short form method definition
* short form method call
* correct infix operators for multiplication and division

Interactive Mode Notes
--------------
```
  brew install gtk+3
  export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:/usr/local/opt/libffi/lib/pkgconfig"
```
