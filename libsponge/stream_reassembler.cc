#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {
    _unassembled.resize(capacity);
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    const auto first_unassembled = _output.bytes_written();
    const auto upper = _output.bytes_written() + _capacity;

    if (index > upper || index + data.size() < first_unassembled)  // out of scope
        return;

    const auto begin = max(index, first_unassembled);  // the begin pos of segment
    const auto end = min(index + data.size(), upper);  // the end pos of segment

    for (size_t i = begin; i < end; ++i) {
        if (!_unassembled[i % _capacity].second) {  // if [pos] empty(%capacity make vector a circle)->
            _unassembled[i % _capacity] = {data[i - index], true};
            _unassembled_size++;
        }
    }

    string assembled;
    size_t pos = first_unassembled % _capacity;
    size_t res = _output.remaining_capacity(); // avoid pushing too much
    while (assembled.size() <= res && _unassembled[pos].second) {
        assembled += _unassembled[pos].first;
        _unassembled[pos] = {};
        pos = (pos + 1) % _capacity;
    }

    if (!assembled.empty()) {
        _output.write(assembled);
        _unassembled_size -= assembled.size();
    }

    if (eof)
        _eof_pos = end;

    if (_eof_pos == _output.bytes_written())
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_size; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
