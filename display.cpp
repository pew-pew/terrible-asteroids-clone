#include "display.h"
#include <string>
#include <iostream>

namespace display {

Sprite Sprite::fromString(std::string s, std::vector<Color> pallete) {
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
    else
      assert(false);
  }
  if (lines.back().empty())
    lines.pop_back();

  assert(!lines.empty());
  size_t sz = lines.front().size();
  for (auto &line : lines)
    assert(line.size() == sz);

  return Sprite{std::move(lines)};
}


std::string string_hearth = R"(
.00...00.
0000.0000
000000000
.0000000.
..00000..
...000...
....0....
)";

Sprite sprites::hearth = Sprite::fromString(string_hearth, {colors::sexyRed});


std::string string_asteroid = R"(
................
.....0000000....
...11000110001..
..110000110000..
..0000000000000.
.10000000011000.
.11000110011000.
.10000111010000.
.00000010000000.
.00100000000001.
.01110000000011.
.01111000001011.
..001000001111..
...0000001111...
.....111111.....
................
)";

Sprite sprites::asteroids[3] = {
  Sprite::fromString(string_asteroid, {{0x41, 0x4c, 0x6d}, {0x23, 0x27, 0x3e}}),
  Sprite::fromString(string_asteroid, {{0x00, 0x69, 0x5c}, {0x00, 0x4d, 0x40}}),
  Sprite::fromString(string_asteroid, {{0x15, 0x43, 0xd8}, {0x0c, 0x36, 0xbf}}),
};

}