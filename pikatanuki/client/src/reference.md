# 使い方説明

## Table of Contents

# 言語の使い方

## 変数
```
名前＝「鈴木」
年齢＝１２
挨拶＝名前＋「と申します」
```
## 関数
```
＃呼ぶ
表示（「こんにちは」、「世界」）

＃作る
関数、関数名（引数１、引数２）
　返す、引数１＋引数２
```

末尾再帰（tail recursion）はオプティマイズされました。

## 数学
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

## もし文
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

## データ構造
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

## 導入
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

-------

# 定義済み関数

## Basicな関数
### 表示
`表示（＊引数）`

**引数**：表示したい事

**返す**：無

**例文**
```
表示（「こんにちは」）＃「こんにちは」を表示
表示（「こんにちは」、「世界」）＃「こんにちは世界」を表示
表示（「結果は」、８２／２＋１、「です」）＃「結果は４２です」を表示する
```

## データ構造についての関数
### 辞書
`辞書（＊＊引数、〈親〉）`

**引数**：新しい辞書のキーとバリュー

**親**：新しい辞書の親。辞書を調べる時にキーは入ってない場合、親に任せる。

**返す**：新しい辞書

**例文**
```
あ＝辞書（あ：１、い：２、う：２＋２、え：「おはよう」）
表示（あ・え）＃「おはよう」を表示する

子供＝辞書（あ：１、あ）
表示（子供・あ、子供・い）＃「１おはよう」を表示する
```

### それぞれ
`それぞれ（入力、入力関数）`

**入力**：配列や辞書など

**入力関数**：この関数を何回も一番めの配列とかの中身のものを渡して呼ぶ。

**返す**：無

### 長さ
引数：配列

返す：配列の長さを返す

### 配列調べ
引数：配列、何番目

返す：○番目のバリュー

### 辞書調べ
引数：辞書、キー

返す：バリュー

### 配列更新

## 数学関数
### 足す
説明はまだ

### 引く
説明はまだ

### 割り算
説明はまだ

### 掛ける
説明はまだ

### イコール
説明はまだ

### 比べ
説明はまだ


## システム関数
### ファイル読む
説明はまだ

### 評価
説明はまだ

### エキステンション
説明はまだ

