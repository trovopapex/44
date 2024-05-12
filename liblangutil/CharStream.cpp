/*
 * This file is part of solidity.
 *
 * solidity is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * solidity is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with solidity.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file is derived from the file "scanner.cc", which was part of the
 * V8 project. The original copyright header follows:
 *
 * Copyright 2006-2012, the V8 project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 * * Neither the name of Google Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**
 * Character stream / input file.
 */

#include <liblangutil/CharStream.h>
#include <liblangutil/Exceptions.h>
#include <algorithm>
#include <optional>

using namespace solidity;
using namespace solidity::langutil;

char CharStream::advanceAndGet(size_t _chars) {
    if (isPastEndOfInput())
        return 0;
    m_position += _chars;
    return isPastEndOfInput() ? 0 : m_source[m_position];
}

char CharStream::rollback(size_t _amount) {
    solAssert(m_position >= _amount, "");
    m_position -= _amount;
    return get();
}

char CharStream::setPosition(size_t _location) {
    solAssert(_location <= m_source.size(), "Attempting to set position past end of source.");
    m_position = _location;
    return get();
}

std::string CharStream::lineAtPosition(int _position) const {
    auto searchStart = static_cast<size_t>(std::min(std::max(_position, 0), static_cast<int>(m_source.size())));
    if (searchStart > 0)
        searchStart--;
    auto lineStart = m_source.rfind('\n', searchStart);
    lineStart = (lineStart == std::string::npos) ? 0 : lineStart + 1;
    auto lineEnd = m_source.find('\n', lineStart);
    lineEnd = (lineEnd == std::string::npos) ? m_source.size() : lineEnd;
    std::string line = m_source.substr(lineStart, lineEnd - lineStart);
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return line;
}

LineColumn CharStream::translatePositionToLineColumn(int _position) const {
    auto searchPosition = static_cast<size_t>(std::min(std::max(_position, 0), static_cast<int>(m_source.size())));
    auto lineNumber = std::count(m_source.begin(), m_source.begin() + searchPosition, '\n');
    auto lineStart = (searchPosition == 0) ? 0 : m_source.rfind('\n', searchPosition - 1);
    lineStart = (lineStart == std::string::npos) ? 0 : lineStart + 1;
    return LineColumn{static_cast<int>(lineNumber), static_cast<int>(searchPosition - lineStart)};
}

std::string_view CharStream::text(SourceLocation const& _location) const {
    if (!_location.hasText() || !_location.sourceName || *_location.sourceName != m_name ||
        static_cast<size_t>(_location.end) > m_source.size())
        return {};
    return std::string_view{m_source}.substr(static_cast<size_t>(_location.start),
                                              static_cast<size_t>(_location.end - _location.start));
}

std::string CharStream::singleLineSnippet(std::string const& _sourceCode, SourceLocation const& _location) {
    if (!_location.hasText() || static_cast<size_t>(_location.start) >= _sourceCode.size())
        return {};

    auto cut = _sourceCode.substr(static_cast<size_t>(_location.start), static_cast<size_t>(_location.end - _location.start));
    auto newLinePos = cut.find_first_of("\n\r");
    if (newLinePos != std::string::npos)
        cut = cut.substr(0, newLinePos) + "...";

    return cut;
}

std::optional<int> CharStream::translateLineColumnToPosition(LineColumn const& _lineColumn) const {
    return translateLineColumnToPosition(m_source, _lineColumn);
}

std::optional<int> CharStream::translateLineColumnToPosition(std::string const& _text, LineColumn const& _input) {
    if (_input.line < 0)
        return std::nullopt;

    size_t offset = 0;
    for (int i = 0; i < _input.line; i++) {
        offset = _text.find('\n', offset);
        if (offset == std::string::npos)
            return std::nullopt;
        offset++; // Skip linefeed.
    }

    size_t endOfLine = _text.find('\n', offset);
    endOfLine = (endOfLine == std::string::npos) ? _text.size() : endOfLine;

    if (offset + static_cast<size_t>(_input.column) > endOfLine)
        return std::nullopt;
    return static_cast<int>(offset + _input.column);
}
