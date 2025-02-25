// This file is made available under Elastic License 2.0.
// This file is based on code available under the Apache license here:
//   https://github.com/apache/incubator-doris/blob/master/be/src/olap/rowset/segment_v2/binary_plain_page.h

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// Simplistic page encoding for strings.
//
// The page consists of:
// Strings:
//   raw strings that were written
// Trailer
//  Offsets:
//    offsets pointing to the beginning of each string
//  num_elems (32-bit fixed)
//

#pragma once

#include <cstdint>

#include "common/logging.h"
#include "runtime/mem_pool.h"
#include "storage/olap_common.h"
#include "storage/rowset/segment_v2/options.h"
#include "storage/rowset/segment_v2/page_builder.h"
#include "storage/rowset/segment_v2/page_decoder.h"
#include "storage/types.h"
#include "storage/vectorized/range.h"
#include "util/coding.h"
#include "util/faststring.h"

namespace starrocks::vectorized {
class Column;
}

namespace starrocks {
namespace segment_v2 {

class BinaryPlainPageBuilder final : public PageBuilder {
public:
    explicit BinaryPlainPageBuilder(const PageBuilderOptions& options)
            : _reserved_head_size(0), _size_estimate(0), _next_offset(0), _options(options), _finished(false) {
        reset();
    }

    void reserve_head(uint8_t head_size) override {
        CHECK_EQ(0, _reserved_head_size);
        _reserved_head_size = head_size;
        _buffer.resize(_reserved_head_size);
    }

    bool is_page_full() override {
        // data_page_size is 0, do not limit the page size
        return (_options.data_page_size != 0) & (_size_estimate > _options.data_page_size);
    }

    size_t add(const uint8_t* vals, size_t count) override {
        DCHECK(!_finished);
        const Slice* slices = reinterpret_cast<const Slice*>(vals);
        for (size_t i = 0; i < count; i++) {
            if (!add_slice(slices[i])) {
                return i;
            }
        }
        return count;
    }

    bool add_slice(const Slice& s) {
        if (is_page_full()) {
            return false;
        }
        DCHECK_EQ(_buffer.size(), _reserved_head_size + _next_offset);
        _offsets.push_back(_next_offset);
        _buffer.append(s.data, s.size);

        _next_offset += s.size;
        _size_estimate += s.size;
        _size_estimate += sizeof(uint32_t);
        return true;
    }

    faststring* finish() override {
        DCHECK(!_finished);
        DCHECK_EQ(_next_offset + _reserved_head_size, _buffer.size());
        _buffer.reserve(_size_estimate);
        // Set up trailer
        for (uint32_t _offset : _offsets) {
            put_fixed32_le(&_buffer, _offset);
        }
        put_fixed32_le(&_buffer, _offsets.size());
        if (!_offsets.empty()) {
            _copy_value_at(0, &_first_value);
            _copy_value_at(_offsets.size() - 1, &_last_value);
        }
        _finished = true;
        return &_buffer;
    }

    void reset() override {
        _offsets.clear();
        _buffer.reserve(_options.data_page_size == 0 ? 65536 : _options.data_page_size);
        _buffer.resize(_reserved_head_size);
        _next_offset = 0;
        _size_estimate = sizeof(uint32_t);
        _finished = false;
    }

    size_t count() const override { return _offsets.size(); }

    uint64_t size() const override { return _size_estimate; }

    Status get_first_value(void* value) const override {
        DCHECK(_finished);
        if (_offsets.empty()) {
            return Status::NotFound("page is empty");
        }
        *reinterpret_cast<Slice*>(value) = Slice(_first_value);
        return Status::OK();
    }

    Status get_last_value(void* value) const override {
        DCHECK(_finished);
        if (_offsets.empty()) {
            return Status::NotFound("page is empty");
        }
        *reinterpret_cast<Slice*>(value) = Slice(_last_value);
        return Status::OK();
    }

    Slice get_value(size_t idx) const {
        DCHECK(!_finished);
        DCHECK_LT(idx, _offsets.size());
        size_t end = (idx + 1) < _offsets.size() ? _offsets[idx + 1] : _next_offset;
        size_t off = _offsets[idx];
        return Slice(&_buffer[_reserved_head_size + off], end - off);
    }

private:
    void _copy_value_at(size_t idx, faststring* value) const {
        Slice s = get_value(idx);
        value->assign_copy((const uint8_t*)s.data, s.size);
    }

    uint8_t _reserved_head_size;
    size_t _size_estimate;
    size_t _next_offset;
    faststring _buffer;
    // Offsets of each entry, relative to the start of the page
    std::vector<uint32_t> _offsets;
    PageBuilderOptions _options;
    faststring _first_value;
    faststring _last_value;
    bool _finished;
};

template <FieldType Type>
class BinaryPlainPageDecoder final : public PageDecoder {
public:
    explicit BinaryPlainPageDecoder(Slice data) : BinaryPlainPageDecoder(data, PageDecoderOptions()) {}

    BinaryPlainPageDecoder(Slice data, const PageDecoderOptions& options)
            : _data(data), _options(options), _parsed(false), _num_elems(0), _offsets_pos(0), _cur_idx(0) {}

    Status init() override {
        RETURN_IF(_parsed, Status::OK());

        if (_data.size < sizeof(uint32_t)) {
            std::stringstream ss;
            ss << "file corrupton: not enough bytes for trailer in BinaryPlainPageDecoder ."
                  "invalid data size:"
               << _data.size << ", trailer size:" << sizeof(uint32_t);
            return Status::Corruption(ss.str());
        }

        // Decode trailer
        _num_elems = decode_fixed32_le((const uint8_t*)&_data[_data.get_size() - sizeof(uint32_t)]);
        _offsets_pos = _data.get_size() - (_num_elems + 1) * sizeof(uint32_t);

        _parsed = true;

        return Status::OK();
    }

    Status seek_to_position_in_page(size_t pos) override {
        DCHECK_LE(pos, _num_elems);
        _cur_idx = pos;
        return Status::OK();
    }

    Status next_batch(size_t* n, ColumnBlockView* dst) override {
        DCHECK(_parsed);
        if (PREDICT_FALSE(*n == 0 || _cur_idx >= _num_elems)) {
            *n = 0;
            return Status::OK();
        }
        size_t max_fetch = std::min(*n, static_cast<size_t>(_num_elems - _cur_idx));

        Slice* out = reinterpret_cast<Slice*>(dst->data());

        for (size_t i = 0; i < max_fetch; i++, out++, _cur_idx++) {
            Slice elem(string_at_index(_cur_idx));
            out->size = elem.size;
            if (elem.size != 0) {
                out->data = reinterpret_cast<char*>(dst->pool()->allocate(elem.size * sizeof(uint8_t)));
                RETURN_IF_UNLIKELY_NULL(out->data, Status::MemoryAllocFailed("alloc mem for binary plain page failed"));
                memcpy(out->data, elem.data, elem.size);
            }
        }

        *n = max_fetch;
        return Status::OK();
    }

    Status next_batch(size_t* count, vectorized::Column* dst) override;

    Status next_batch(const vectorized::SparseRange& range, vectorized::Column* dst) override;

    size_t count() const override {
        DCHECK(_parsed);
        return _num_elems;
    }

    size_t current_index() const override {
        DCHECK(_parsed);
        return _cur_idx;
    }

    EncodingTypePB encoding_type() const override { return PLAIN_ENCODING; }

    Slice string_at_index(size_t idx) const {
        const uint32_t start_offset = offset(idx);
        uint32_t len = offset(static_cast<int>(idx) + 1) - start_offset;
        return Slice(&_data[start_offset], len);
    }

    int find(const Slice& word) const {
        DCHECK(_parsed);
        for (uint32_t i = 0; i < _num_elems; i++) {
            const uint32_t off1 = offset_uncheck(i);
            const uint32_t off2 = offset(i + 1);
            Slice s(&_data[off1], off2 - off1);
            if (s == word) {
                return i;
            }
        }
        return -1;
    }

    uint32_t max_value_length() const {
        uint32_t max_length = 0;
        for (int i = 0; i < _num_elems; ++i) {
            uint32_t length = offset(i + 1) - offset_uncheck(i);
            if (length > max_length) {
                max_length = length;
            }
        }
        return max_length;
    }

    uint32_t dict_size() { return _num_elems; }

private:
    // Return the offset within '_data' where the string value with index 'idx' can be found.
    uint32_t offset(int idx) const {
        if (idx >= _num_elems) {
            return _offsets_pos;
        }
        return offset_uncheck(idx);
    }

    uint32_t offset_uncheck(int idx) const {
        const uint32_t pos = _offsets_pos + idx * sizeof(uint32_t);
        const uint8_t* const p = reinterpret_cast<const uint8_t*>(&_data[pos]);
        return decode_fixed32_le(p);
    }

    Slice _data;
    PageDecoderOptions _options;
    bool _parsed;

    uint32_t _num_elems;
    uint32_t _offsets_pos;

    // Index of the currently seeked element in the page.
    uint32_t _cur_idx;
};

} // namespace segment_v2
} // namespace starrocks
