/*************************************************************************/
/*  test_file_access.h                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef TEST_FILE_ACCESS_H
#define TEST_FILE_ACCESS_H

#include "core/os/file_access.h"
#include "test_utils.h"

namespace TestFileAccess {

// TEST_CASE("[FileAccess] Read binary file") {
// 	const String path = OS::get_singleton()->get_executable_path().get_base_dir().plus_file("tests/data/test_file_access_read.bin");
// 	FileAccess *f = FileAccess::open(path, FileAccess::READ);
// }

TEST_CASE("[FileAccess] Write binary file") {
	const String path = OS::get_singleton()->get_cache_path().plus_file("test_file_access_write.bin");
	FileAccess *f = FileAccess::open(path, FileAccess::WRITE);

	f->store_8(0xdd);
	f->store_16(0xffff);
	f->store_32(0x0006'0d07);
	f->store_64(0xdead'beef'cafe'c0de);
	f->store_pascal_string("This string has its length encoded next to it.\tUnicode characters for testing: × é ❤ 😛\n");
	f->store_float(Math_PI);
	f->store_double(Math_TAU);
	f->store_real(Math_E);

	Vector<uint8_t> buf;
	buf.push_back(0);
	buf.push_back(1);
	buf.push_back(2);
	buf.push_back(3);
	buf.push_back(4);
	buf.push_back(5);
	uint8_t *w = buf.ptrw();
	// Store the buffer length first so it can be determined in the binary file when reading it back.
	f->store_8(buf.size());
	f->store_buffer(w, buf.size());

	f->close();
	memdelete(f);
}

// TEST_CASE("[FileAccess] Read text file") {
// 	const String path = OS::get_singleton()->get_executable_path().get_base_dir().plus_file("tests/data/test_file_access_read.txt");
// 	FileAccess *f = FileAccess::open(path, FileAccess::READ);
// }

TEST_CASE("[FileAccess] Write text file") {
	const String path = OS::get_singleton()->get_cache_path().plus_file("test_file_access_write.txt");
	FileAccess *f = FileAccess::open(path, FileAccess::WRITE);
}

TEST_CASE("[FileAccess] CSV read") {
	FileAccess *f = FileAccess::open(TestUtils::get_data_path("translations.csv"), FileAccess::READ);

	Vector<String> header = f->get_csv_line(); // Default delimiter: ","
	REQUIRE(header.size() == 3);

	Vector<String> row1 = f->get_csv_line(",");
	REQUIRE(row1.size() == 3);
	CHECK(row1[0] == "GOOD_MORNING");
	CHECK(row1[1] == "Good Morning");
	CHECK(row1[2] == "Guten Morgen");

	Vector<String> row2 = f->get_csv_line();
	REQUIRE(row2.size() == 3);
	CHECK(row2[0] == "GOOD_EVENING");
	CHECK(row2[1] == "Good Evening");
	CHECK(row2[2] == ""); // Use case: not yet translated!

	// https://github.com/godotengine/godot/issues/44269
	CHECK_MESSAGE(row2[2] != "\"", "Should not parse empty string as a single double quote.");

	f->close();
	memdelete(f);
}
} // namespace TestFileAccess

#endif // TEST_FILE_ACCESS_H
