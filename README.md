狸語　プログラミング言語
===========================
![Core Tests](https://github.com/kayleemann/pinpon/workflows/Core%20Tests/badge.svg?branch=master)

「git clone」してから「git submodule」のセットアップ
-----------------------------
```
  git clone https://github.com/kayleema/tanuki.git
  cd tanuki
  git submodule init
  git submodule update
```

狸語をビルド
-----------
cloneしたレポジトリのrootから
```
  mkdir build
  cd build
  cmake ..
  make tanuki
```

## インストール
ビルドした後で`make install`を実行。


狸語の必要なライブラリ
--------------
* C++11 compatible compiler
* cmake

### 「git submodule」ですでに導入されている依存ライブラリ
* fakeit (git submodule に入っている)
* googletest (git submodule に入っている)
* websocketpp (git submodule に入っている)

### OSXの場合は：
```
  xcode-select --install
  brew install cmake
```

### Linuxの場合は：
```
  apt-get install -y build-essential git cmake autoconf libtool pkg-config libasio-dev nlohmann-json3-dev
```


呼び方
--------
```
  ./tanuki <ファイル名.tnk>
```
説明メッセージを表示：
```
  ./tanuki -h
```

インストールすると、「狸」のaliasを作成されるから「tanuki」の代わりに「狸」でもかけます。


IDEとのインテグレーション（シンタックスハイライトとか）
--------------------
VSCodeとのインテグレーション：
  https://marketplace.visualstudio.com/items?itemName=tanuki1.pin

SublimeTextとのインテグレーション：
  (Sublime Text → Preferences → Browse Packages)をクリックして。Gitレポジトリの「tanuki.sublime-syntax」ファイルを「Packages/User/」にドラッグする。

テストする
---------
`build`のディレクトリの中から
```
  make tanuki_test && ./tanuki_test
```

`tests.tnk`ファイルに入っているテストを実行する：
```
  make tanuki && ./tanuki ../testpin/tests.tnk
```

GCCで`tanuki_test`をビルドする時に`cmake -DCMAKE_BUILD_TYPE=Debug ..`しないといけない。
fakeitのライブラリはGCCの`-O2`以上サポートしないだからです。

`codingquestions.tnk`ファイルに入っているテストを実行する：
```
  make tanuki && ./tanuki ../example/codingquestions.tnk
```

言語の説明書
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

末尾再帰（tail recursion）はオプティマイズされました。

### 数学
```
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
鈴木さん・名前＝「鈴木」
鈴木さん・年齢＝１２
鈴木さん【「名前」】

あ＝配列（１、２、３）
表示（あ【０】）

```
墨付きかっこで添字表記法する事が出来る

### 導入
```
＃ファイル名：＂私のモジュール.tnk＂
関数、フィボナッチ（あ）
　もし、あ＜＝１
　　返す、１
　その他
　　返す、フィボナッチ（あー１）＋フィボナッチ（あー２）
```
```
＃ファイル名：＂私のアプリ.tnk＂
導入、私のモジュール
表示（私のモジュール・フィボナッチ（２０））　＃６７６５
```

## 定義済み関数
**表示**
* 表示（表示したい事、他の事、…）

**データ構造**
* 辞書（キー：バリュー、キー２：バリュー２、…）→新しい辞書を作る
* それぞれ（辞書、関数）→辞書のアイテムそれぞれ
* 長さ（配列）→配列の長さを返す
* 配列調べ（配列、何番目）→○番目のバリュー
* 辞書調べ（辞書、キー）→バリュー
* 配列更新

**数学：**
* 足す
* 引く
* 割り算
* 掛ける
* イコール
* 比べ

**システム：**
* ファイル読む
* 評価
* エキステンション

パーフォマンス試験
------------------
```
cd build/
make tanuki
time ./tanuki ../example/codingquestions.tnk
```
結果：`./tanuki ../example/codingquestions.tnk  0.86s user 0.01s system 99% cpu 0.867 total`

細かい事：まだ実装されてない機能
------------------
Nativeエキステンションのテストする方法：
```
cd build/
make tanuki tanuki_dynamic
./tanuki ../example/importDynamicLibrary.tnk
```
希望の機能：
* フローティングポイントのタイプ。floating point calculations
* infix mod operator　今は関数のみ
* expression parenthesis with line wrapping (currently an underscore must be placed before a pair of parenthesis and no 
new lines are allowed in the middle of a statement)
* 文字列の中のエスケープコード。escape codes in strings
* multithreading
* network utilities
* `－変数名`


### （ピカ狸のGUIの場合だけ）必要なライブラリ
* asio (ピカ狸のGUIのためだけ)　このライブラリはgit submoduleに入っていません. 
「 https://think-async.com/ 」からインストール出来ます。
* npmやnodeなど… 
* nhlohman json

### （ピカ狸のGUIの場合だけ）簡単ビルド環境セットアップおすすめ：

OSXの場合は：（最近OSXのthink-asyncのbrewに入っているパッケージは問題ありそうですからソースからインストールした方はいい）
```
  brew tap nlohmann/json
  brew install nlohmann-json
  brew install cmake

  # https://think-async.com/Asio/Download.html からダウンロードしてください。
  cd ~/Downloads/asio-1.12.2
  ./configure --without-boost
  make
  sudo make install
```
