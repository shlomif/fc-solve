// const Clean = require("clean-webpack-plugin");
const path = require("path");
// const HtmlWebpackPlugin = require("html-webpack-plugin");
const TerserPlugin = require("terser-webpack-plugin");

const real_terser_instance = new TerserPlugin({
    test: /\.js$/,
    terserOptions: {
        output: {
            comments: false,
        },
    },
    extractComments: false,
});

const terser_instance = real_terser_instance;

function rel(...args)
{
    return path.resolve(__dirname, ...args);
}

module.exports = {
    optimization: {
        minimize: true,
        minimizer: [terser_instance],
    },
    entry: {
        main: rel("index.js"),
    },
    output: {
        path: rel("dist"),
        filename: "tot.js",
    },
    externals: {
        $: "jQuery",
        jquery: "jQuery",
    },
    module: {
        rules: [
            {
                test: /\\.css$/,
                use: [
                    // [style-loader](/loaders/style-loader)
                    { loader: "style-loader" },
                    // [css-loader](/loaders/css-loader)
                    {
                        loader: "css-loader",
                        options: {
                            modules: true,
                        },
                    },
                    // [sass-loader](/loaders/sass-loader)
                    { loader: "sass-loader" },
                ],
            },
            { test: /\\.ts$/, use: "ts-loader" },
        ],
    },

    mode: "production",
    plugins: [
        // new Clean(["dist"]),
        // new ExtractTextPlugin("app.[hash].css"),
        /*
        new HtmlWebpackPlugin({
                              filename: 'index.html',
                              title: 'jQuery UI Autocomplete demo, built with webpack'
        })*/
    ],
};
