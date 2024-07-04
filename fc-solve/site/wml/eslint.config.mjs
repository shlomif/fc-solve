import path from "node:path";
import { fileURLToPath } from "node:url";
import js from "@eslint/js";
import { FlatCompat } from "@eslint/eslintrc";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const compat = new FlatCompat({
    baseDirectory: __dirname,
    recommendedConfig: js.configs.recommended,
    allConfig: js.configs.all
});

export default [...compat.extends("google"), {
    languageOptions: {
        ecmaVersion: 7,
        sourceType: "script",
    },

    rules: {
        camelcase: "off",
        indent: ["error", 4],
        "new-cap": "off",
        "no-invalid-this": "off",
        "no-throw-literal": "off",
        "no-unused-vars": "off",
        quotes: "off",
        "require-jsdoc": "off",
    },
}];