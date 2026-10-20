#include <...>
using std::...;

#include "support.h"

int main(int argc, char *argv[]) {
  int block_size = 256;
  int K = 1;
  float rho = 1.0;

  read command line arguments
    -gray (optional)
    -flip (optional)
    -draw K (optional)
    -lens z (optional)
    -bs n (optional)
    file name (required)

  error check: 1 < K
  error check: 1.0 < z <= 2.0
  error check: 8 <= n

  instantiate ppm image
  with fileio block size

  read image

  if specified,
    apply gray conversion

  if specified
    flip image left-to-right

  if specified
    create image drawing

  if specified
    apply spherical transformation

  write image
}
