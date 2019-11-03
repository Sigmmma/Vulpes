/*
 *The MIT License (MIT) Copyright (c) 2016 Aadesh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "bit_buffer.hpp"

bit_buffer::bit_buffer(const size_t size) :
    pos_(0), bit_index_(0)
{
    this->buffer_.reserve(size);
}

bit_buffer::~bit_buffer() {}

void bit_buffer::write_byte(const ubyte_t& data) {
    this->write_<ubyte_t>(data);
}

void bit_buffer::write_char(const char& data) {
    this->write_<char>(data);
}

void bit_buffer::write_bool(const bool& data) {
    this->write_<bool>(data);
}

void bit_buffer::write_short(const short& data) {
    this->write_<short>(data);
}

void bit_buffer::write_int(const uint32_t& data) {
    this->write_<uint32_t>(data);
}

void bit_buffer::write_long(const uint64_t& data) {
    this->write_<uint64_t>(data);
}

uint8_t bit_buffer::read_byte(const size_t byte_index) {
    return this->read_bytes_(byte_index, 1);
}

uint32_t bit_buffer::read_bytes(const size_t byte_index, const size_t num_bytes) {
    return this->read_bytes_(byte_index, num_bytes);
}

uint8_t bit_buffer::read_bit(const size_t bit_index) {
    return this->read_bits_(bit_index, 1, 0);
}

uint32_t bit_buffer::read_bits(const size_t bit_index, const size_t num_bits) {
    return this->read_bits_(bit_index, num_bits, 0);
}

bit_iterator bit_buffer::create_iter() const {
    return bit_iterator(*this);
}

bit_buffer::iterator bit_buffer::begin() {
    return bit_iterator(*this);
}

bit_buffer::const_iterator bit_buffer::begin() const {
    return bit_iterator(*this);
}

bit_buffer::iterator bit_buffer::end() {
    return bit_iterator(*this, this->buffer_.size() * 8);
}

bit_buffer::const_iterator bit_buffer::end() const {
    return bit_iterator(*this, this->buffer_.size() * 8);
}
