
export const tanukiLang = {
    'function': [
        {
            lookbehind: true,
            pattern: /(関数、)[^（]*(?=（)/,
            greedy: true
        },
    ],
    'specialcomment': /＃＃＃.*/,
    'comment': /＃.*/,
    'string': /「[^」]*」/,
    'builtin': /表示|配列|辞書|試験一覧|自分|確認|それぞれ/,
    'number': /－?[１２３４５６７８９０]+/,
    'operator': /[＋－＊／・〜＝＞＜]/,
    'punctuation': /[（）【】、]/,
    'keyword': /関数|もし|あるいは|その他|返す/,
};
