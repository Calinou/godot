/*************************************************************************/
/*  test_image.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
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

#ifndef TEST_IMAGE_H
#define TEST_IMAGE_H

#include "core/io/file_access_pack.h"
#include "core/io/pck_packer.h"
#include "core/os/os.h"

#include "thirdparty/doctest/doctest.h"

namespace TestImage {

TEST_CASE("[Image] Stuff") {
}

TEST_CASE("[Image] Loading image formats from buffer and saving to files") {
	const String base_dir = OS::get_singleton()->get_executable_path().get_base_dir();
	const String input_bmp_path = OS::get_singleton()->get_cache_path().plus_file("tests/test_image/image.bmp");
	const String input_jpg_path = OS::get_singleton()->get_cache_path().plus_file("tests/test_image/image.jpg");
	const String input_tga_path = OS::get_singleton()->get_cache_path().plus_file("tests/test_image/image.tga");
	const String input_png_path = OS::get_singleton()->get_cache_path().plus_file("icong.png");
	const String input_webp_path = OS::get_singleton()->get_cache_path().plus_file("tests/test_image/image.webp");

	Ref<Image> image_bmp;
	image_bmp.instance();
	Error err;
	FileAccessRef f = FileAccess::open(input_bmp_path, FileAccess::READ, &err);
	PackedByteArray data;
	data.resize(f->get_len() + 1);
	f->get_buffer(data.ptrw(), f->get_len());
	CHECK_MESSAGE(
			image_bmp->load_bmp_from_buffer(data) == OK,
			"The BMP image should load successfully.");

	// Image image_jpg;
	// f = FileAccess::open(input_jpg_path, FileAccess::READ, &err);
	// data = Vector<uint8_t>();
	// data.resize(f->get_len() + 1);
	// f->get_buffer(data.ptrw(), f->get_len());
	// CHECK_MESSAGE(
	// 		image_jpg.load_jpg_from_buffer(data) == OK,
	// 		"The JPEG image should load successfully.");

	// Image image_tga;
	// f = FileAccess::open(input_tga_path, FileAccess::READ, &err);
	// data = Vector<uint8_t>();
	// data.resize(f->get_len() + 1);
	// f->get_buffer(data.ptrw(), f->get_len());
	// CHECK_MESSAGE(
	// 		image_tga.load_tga_from_buffer(data) == OK,
	// 		"The TGA image should load successfully.");

	// Image image_png;
	// f = FileAccess::open(input_png_path, FileAccess::READ, &err);
	// data = Vector<uint8_t>();
	// data.resize(f->get_len() + 1);
	// f->get_buffer(data.ptrw(), f->get_len());
	// CHECK_MESSAGE(
	// 		image_png.load_png_from_buffer(data) == OK,
	// 		"The PNG image should load successfully.");

	// Image image_webp;
	// f = FileAccess::open(input_webp_path, FileAccess::READ, &err);
	// data = Vector<uint8_t>();
	// data.resize(f->get_len() + 1);
	// f->get_buffer(data.ptrw(), f->get_len());
	// CHECK_MESSAGE(
	// 		image_webp.load_webp_from_buffer(data) == OK,
	// 		"The WebP image should load successfully.");
}
} // namespace TestImage

#endif // TEST_IMAGE_H
