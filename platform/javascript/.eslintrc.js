module.exports = {
	env: {
		browser: true,
		es2021: true,
	},
	extends: [
		'airbnb-base',
	],
	parserOptions: {
		ecmaVersion: 12,
	},
	rules: {
		'func-names': 'off',
		// Use tabs for consistency with the C++ codebase.
		indent: ['error', 'tab'],
		'max-len': 'off',
		'no-alert': 'off',
		'no-bitwise': 'off',
		'no-console': 'off',
		'no-continue': 'off',
		'no-self-assign': 'off',
		'no-tabs': 'off',
		'no-param-reassign': 'off',
		'no-plusplus': 'off',
		'no-unused-vars': 'off',
		'prefer-destructuring': 'off',
		'prefer-rest-params': 'off',
		'prefer-spread': 'off',
		// Messes up with copyright headers in source files.
		'spaced-comment': 'off',
	},
};
