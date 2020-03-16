const HtmlWebPackPlugin = require("html-webpack-plugin");

module.exports = {
    output: {
        publicPath: "/",
    },
    module: {
        rules: [
            {
                test: /\.(js|jsx)$/,
                exclude: /node_modules/,
                use: {
                    loader: "babel-loader"
                }
            },
            {
                test: /\.html$/,
                use: [
                    {
                        loader: "html-loader"
                    }
                ]
            },
            {
                test: /\.css$/i,
                use: ['style-loader', 'css-loader'],
            },
            {
                test: /\.(png|jpe?g|gif)$/i,
                use: [
                    {
                        loader: 'file-loader',
                    },
                ],
            },
            {
                test: /\.(pin)$/i,
                use: [
                    {
                        loader: 'file-loader',
                        options: {
                            name: '[path][name].[ext]',
                        },
                    },
                ],
            },
        ]
    },
    plugins: [
        new HtmlWebPackPlugin({ template: "./public/index.html", filename: "./index.html", }),
        new HtmlWebPackPlugin({ template: "./public/index.html", filename: "./edit/index.html", }),
        new HtmlWebPackPlugin({ template: "./public/index.html", filename: "./interract/index.html", }),
    ],
    devServer: {
        historyApiFallback: true
    }
};