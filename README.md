ピンポン狸語　プログラミング言語
===========================

ビルド
-----------
```
  mkdir build
  cd build
  cmake ..
  make
```

テストする
---------
```
  make pinpon_test && ./pinpon_test
```
`.pin`ファイルに入っているテストを実行する：
```
  make pinpon && ./pinpon ../testpin/tests.pin
```

呼び方
--------
```
  ./pinpon <ファイル名.pin>
```
Helpメッセージを表示：
```
  ./pinpon -h
```

Formal Grammar (incomplete)
----------------------
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

まだ実装されてない機能
------------------
* keyword argument unpacking with defaults
* variable length non-keyword arguments
* dynamic dictionary lookup
* dynamic dictionary set
* short form method definition
* short form method call
* correct infix operators for multiplication and division
* expression parenthesis with line wrapping

説明書
-----
### 変数：
```
名前＝「鈴木」
年齢＝１２
挨拶＝名前＋「と申します」
```
### 関数：
```
＃呼ぶ
表示（「こんにちは」、「世界」）

＃作る
関数、関数名（引数１、引数２）
　返す、引数１＋引数２
```

### 数学
```$xslt
表示（１＋５ー４）
表示（１＋５＜＝１０－２）
```
使える演算子：
* ＝＝
* ！＝
* ＞＝
* ＜＝
* ＞
* ＜
* ＋
* －

*⚠️マイナスについては注意‼︎：色々な似てる文字があるから気をつけてください*
* 「ー」（カタカナ）と「－」（全角ハイフン）と「一」（いち）と「-」（半角ハイフン）があります
* マイナスとして使う時に、全角ハイフン鹿使えません。

### もし文
```
関数、アルコル飲んでもいい（年齢）
　もし、年齢＞＝２０
　　表示（「はい！」）
　あるいは、年齢＝＝１９
　　表示（「いいえ、後一年」）
　その他
　　表示（「いいえ」）

アルコル飲んでもいい（２０）　＃はい！
アルコル飲んでもいい（１８）　＃いいえ
```

### データ構造
一番大切なデータ構造は「辞書」と言います：
```
鈴木さん＝辞書（）
鈴木さん・名前＝鈴木
鈴木さん・年齢＝１２
```

### 導入
```
＃ファイル名：＂私のモジュール.pin＂
関数、フィボナッチ（あ）
　もし、あ＜＝１
　　返す、１
　その他
　　返す、フィボナッチ（あー１）＋フィボナッチ（あー２）
```
```
＃ファイル名：＂私のアプリ.pin＂
導入、私のモジュール
表示（私のモジュール・フィボナッチ（２０））　＃６７６５
```