#include <...>
using std::xxx;

#include "support.h"

ppm constructor(int)
ppm constructor(const ppm &)
ppm destructor()

ppm operator[]()

ppm read()
  open file -- error check

  read header
  skip blanks

  magicid -- error check
  maxvalue -- error check

  read image (block_size) -- error check

  close file
}

ppm write()
  update file name
  open file -- error check

  write header
  write image (block size)

  close file
}

void gray(ppm &img) {
  change RBG to (R+G+B)/3
}

void flip(ppm &img) {
  flip image left-to-right
}

void draw(ppm &img, int K) {
  extract drawing of image
}

void lens(ppm &img, float rho) {
  apply spherical transformation
}