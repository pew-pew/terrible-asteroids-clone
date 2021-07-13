#include "display.h"
#include <string>
#include <iostream>

namespace display {

std::string normalize(std::string s) {
  std::string normalized;
  for (char c : s) {
    switch (c) {
      case ' ': 
        break;
      case '\n':
        if (!normalized.empty() && normalized.back() != '\n')
          normalized += '\n';
        break;
      default:
        normalized += c;
        break;
    }
  }
  if (!normalized.empty() && normalized[-1] != '\n') normalized += '\n';
  return normalized;
}


Sprite string2sprite(std::string s, std::vector<Color> pallete) {
  Color bg{0, 0, 0, 1};

  std::vector<std::vector<Color>> lines(1);
  for (char c : s) {
    if (c == '\n') {
      if (!lines.back().empty())
        lines.emplace_back();
    } else if (c == '.')
      lines.back().push_back(bg);
    else if ('0' <= c && c <= '9')
      lines.back().push_back(pallete.at(c - '0'));
    else {
      std::cout << '<' << c << '>' << std::endl;
      assert(false);
    }
  }
  if (lines.back().empty())
    lines.pop_back();

  assert(!lines.empty());
  size_t sz = lines.front().size();
  for (auto &line : lines) assert(line.size() == sz);

  return Sprite{std::move(lines)};
}


std::string s = R"(
.00...00.
0000.0000
000000000
.0000000.
..00000..
...000...
....0....
)";

Sprite hearth = string2sprite(s, {{0, 0, 255}});


std::string s_ast = R"(
................
.....0000000....
...11000110001..
..110000110000..
..0000000000000.
.10000000011000.
.11000110011000.
.10000111010000.
.00000010000000.
.00100000000000.
.01110000000000.
.01111000001000.
..0010000011101.
...0000000110...
.....0000001....
................
)";

Sprite asteroid = string2sprite(s_ast, {{0, 100, 200}, {0, 50, 100}});

}