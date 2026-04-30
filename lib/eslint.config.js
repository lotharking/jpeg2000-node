const tsPlugin = require('@typescript-eslint/eslint-plugin')
const tsParser = require('@typescript-eslint/parser')
const importPlugin = require('eslint-plugin-import')
const prettierPlugin = require('eslint-plugin-prettier')
const eslintConfigPrettier = require('eslint-config-prettier')

module.exports = [
  {
    ignores: ['build/**', 'node_modules/**', 'eslint.config.js'],
  },
  {
    files: ['src/**/*.ts', 'tests/**/*.ts'],
    plugins: {
      '@typescript-eslint': tsPlugin,
      import: importPlugin,
      prettier: prettierPlugin,
    },
    languageOptions: {
      parser: tsParser,
      parserOptions: {
        project: 'tsconfig.eslint.json',
        tsconfigRootDir: __dirname,
        sourceType: 'module',
      },
    },
    settings: {
      'import/resolver': {
        typescript: {
          project: 'tsconfig.eslint.json',
        },
      },
    },
    rules: {
      ...tsPlugin.configs.recommended.rules,
      ...eslintConfigPrettier.rules,
      'prettier/prettier': 'error',
      'arrow-body-style': 'off',
      'prefer-arrow-callback': 'off',
      '@typescript-eslint/interface-name-prefix': 'off',
      '@typescript-eslint/explicit-function-return-type': 'off',
      '@typescript-eslint/explicit-module-boundary-types': 'off',
      '@typescript-eslint/no-explicit-any': 'off',
      'import/no-cycle': 'error',
      'import/newline-after-import': ['error', { count: 1 }],
      'import/order': [
        'error',
        {
          groups: ['type', ['builtin', 'external'], 'parent', 'sibling', 'index'],
          alphabetize: { order: 'asc' },
          'newlines-between': 'always',
        },
      ],
    },
  },
]
