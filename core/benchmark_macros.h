/**************************************************************************/
/*  benchmark_macros.h                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef BENCHMARK_MACROS_H
#define BENCHMARK_MACROS_H

#include "core/os/time.h"

/**
 * Creates a micro benchmarking block with a name. The name must be a valid
 * identifier and must not be surrounded by quotes. Spaces in the name are not
 * allowed. Example:
 *
 *     BENCHMARK_BEGIN(process_text);
 *     // Do stuff here. The time between those two lines is measured
 *     // (excluding the expanded code from the begin/end macros).
 *     BENCHMARK_END(process_text);
 */
#define BENCHMARK_BEGIN(m_name)                          \
	static uint64_t benchmark_count_##m_name = 0;        \
	static uint64_t benchmark_total_##m_name = 0;        \
	static uint64_t benchmark_min_##m_name = UINT64_MAX; \
	static uint64_t benchmark_max_##m_name = 0;          \
	const uint64_t benchmark_begin_##m_name = Time::get_singleton()->get_ticks_usec();

/**
 * Ends a micro benchmarking block with the specified name. Every time the end
 * block is reached, the time spent running the block (in microseconds) is
 * printed to standard output, along with the minimum, maximum and average time
 * spent across all iterations of the same name.
 */
#define BENCHMARK_END(m_name)                                                                                    \
	const uint64_t benchmark_diff_##m_name = Time::get_singleton()->get_ticks_usec() - benchmark_begin_##m_name; \
	String benchmark_color_##m_name;                                                                             \
	if (benchmark_diff_##m_name < 10) {                                                                          \
		benchmark_color_##m_name = "blue";                                                                       \
	} else if (benchmark_diff_##m_name < 100) {                                                                  \
		benchmark_color_##m_name = "cyan";                                                                       \
	} else if (benchmark_diff_##m_name < 1000) {                                                                 \
		benchmark_color_##m_name = "green";                                                                      \
	} else if (benchmark_diff_##m_name < 10000) {                                                                \
		benchmark_color_##m_name = "yellow";                                                                     \
	} else if (benchmark_diff_##m_name < 100000) {                                                               \
		benchmark_color_##m_name = "red";                                                                        \
	} else {                                                                                                     \
		benchmark_color_##m_name = "magenta";                                                                    \
	}                                                                                                            \
	benchmark_count_##m_name += 1;                                                                               \
	benchmark_total_##m_name += benchmark_diff_##m_name;                                                         \
	benchmark_min_##m_name = MIN(benchmark_min_##m_name, benchmark_diff_##m_name);                               \
	benchmark_max_##m_name = MAX(benchmark_max_##m_name, benchmark_diff_##m_name);                               \
	print_line_rich(vformat(String::utf8("[code]Benchmark:[/code] [b]%s[/b] #%d took [color=%s]%d µs[/color] [code](min: %d µs, max: %d µs, avg: %d µs)[/code]"), #m_name, benchmark_count_##m_name, benchmark_color_##m_name, benchmark_diff_##m_name, benchmark_min_##m_name, benchmark_max_##m_name, benchmark_total_##m_name / benchmark_count_##m_name));

#endif // BENCHMARK_MACROS_H
