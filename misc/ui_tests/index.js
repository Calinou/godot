const fs = require('fs');
const path = require('path');
const childProcess = require('child_process');
const { screen, imageResource, mouse, keyboard, sleep, Key, Button, Point, straightTo, left, right, up, down } = require('@nut-tree/nut-js');
require('@nut-tree/template-matcher');

// Path to the compiled Godot editor binary to run.
const GODOT_EDITOR_BINARY_PATH = `${__dirname}/../../bin/godot.linuxbsd.tools.64.llvm`;

// Path to the self-contained file to create.
const SELF_CONTAINED_FILE_PATH = `${path.dirname(GODOT_EDITOR_BINARY_PATH)}/._sc_`;

// Very long timeout duration. Use when `waitFor()` should never realistically timeout.
const NO_TIMEOUT = 1000000;

// Returns the center Point of the region specified.
// This function is used instead of nut.js's centerOf as nut.js returns a Promise<Point>.
// That makes it less convenient to use compared to this function.
function centerOf(region) {
	return new Point(region.left + region.width * 0.5, region.top + region.height * 0.5);
}

// Presses a key, then releases it immediately.
async function pressKeyOnce(key) {
	await keyboard.pressKey(key);
	await keyboard.releaseKey(key);
}

// Presses a mouse button, then releases it immediately.
async function pressMouseButtonOnce(button) {
	await mouse.pressButton(button);
	await mouse.releaseButton(button);
}

// Moves the mouse and clicks on the region specified.
async function click(region) {
	await mouse.setPosition(centerOf(region));
	await pressMouseButtonOnce(Button.LEFT);
}

// Moves the mouse and double-clicks on the region specified.
async function doubleClick(region) {
	await mouse.setPosition(centerOf(region));
	await pressMouseButtonOnce(Button.LEFT);
	await sleep(150);
	await pressMouseButtonOnce(Button.LEFT);
}

(async () => {
	// Maximize typing speed to make tests complete faster.
	keyboard.config.autoDelayMs = 0;
	screen.config.resourceDirectory = `${__dirname}/images`;
	// Set to `false` in production.
	screen.config.autoHighlight = true;

	// Remove existing self-contained mode data.
	fs.rmSync(`${path.dirname(GODOT_EDITOR_BINARY_PATH)}/editor_data`, { recursive: true, force: true });
	console.log('INFO: Removed existing self-contained editor data.');

	// Enable self-contained mode to avoid conflict with user settings.
	fs.openSync(SELF_CONTAINED_FILE_PATH, 'w');
	console.log('INFO: Self-contained mode enabled.');

	console.log('INFO: Running Godot editor binary...\n');
	// Force English language to ensure that image matching works.
	childProcess.execFile(GODOT_EDITOR_BINARY_PATH, ['--language', 'en']);

	try {
		console.log('Project manager: Dismissing welcome dialog...');
		// Wait for the "You don't have any projects" dialog to appear,
		// then dismiss it using the keyboard.
		await screen.waitFor(imageResource('project_manager/welcome_dialog.png'), NO_TIMEOUT);
		await pressKeyOnce(Key.Escape);

		console.log('Project manager: Clicking on New Project...');
		const newProjectButton = await screen.find(imageResource('project_manager/new_project_button.png'));
		await click(newProjectButton);

		console.log('Project manager: Entering project name...');
		// Add an unique value to the project name so that it never exists already.
		await keyboard.type(`UI Test ${Math.floor(new Date().getTime() / 1000)}`);
		// Focus Create Folder button and press it using the keyboard.
		await pressKeyOnce(Key.Tab);
		await pressKeyOnce(Key.Enter);

		console.log('Project manager: Clicking on Create & Edit...');
		const createAndEditButton = await screen.find(imageResource('project_manager/create_and_edit_button.png'));
		await click(createAndEditButton);
	} catch (err) {
		console.error(err);
	}

	// Disable self-contained mode once tests have run.
	fs.rmSync(SELF_CONTAINED_FILE_PATH);
	console.log('\nINFO: Self-contained mode disabled.');
})();
