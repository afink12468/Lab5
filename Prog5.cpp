#include <iostream>
#include <cstdlib>
#include <cstring>

using std::cerr;
using std::endl;

#include "Support.h"

int main(int argc, char *argv[]) // optional command line arguments
{
    int block_size = 256;
    int K = 1;
    float rho = 1.0;

    bool do_gray = false;
    bool do_flip = false;
    bool do_draw = false;
    bool do_lens = false;

    char *filename = NULL;
    char *end;

    const char *usage =
        "Usage: ./Prog5 [-gray] [-flip] [-draw K] "
        "[-lens z] [-bs nbytes] image.ppm";

    for (int i = 1; i < argc; i++) { // parsing of arguments
        if (strcmp(argv[i], "-gray") == 0) {
            do_gray = true;
        }
        else if (strcmp(argv[i], "-flip") == 0) {
            do_flip = true;
        }
        else if (strcmp(argv[i], "-draw") == 0) {
            if (i + 1 >= argc) {
                cerr << "Error: -draw needs a value for K" << endl
                     << usage << endl;
                return 1;
            }

            long k = strtol(argv[++i], &end, 10);

            if (*end != '\0' || k < 1) {
                cerr << "Error: K must be an integer, at least 1"
                     << endl << usage << endl;
                return 1;
            }

            K = (int)k;
            do_draw = true;
        }
        else if (strcmp(argv[i], "-lens") == 0) {
            if (i + 1 >= argc) {
                cerr << "Error: -lens needs a value for rho" << endl
                     << usage << endl;
                return 1;
            }

            double z = strtod(argv[++i], &end);

            if (*end != '\0' || z <= 1.0 || z > 2.0) {
                cerr << "Error: rho must be greater than 1.0 "
                     << "and at most 2.0" << endl
                     << usage << endl;
                return 1;
            }

            rho = (float)z;
            do_lens = true;
        }
        else if (strcmp(argv[i], "-bs") == 0) {
            if (i + 1 >= argc) {
                cerr << "Error: -bs needs a block size" << endl
                     << usage << endl;
                return 1;
            }

            long n = strtol(argv[++i], &end, 10);

            if (*end != '\0' || n < 8) {
                cerr << "Error: block size must be an integer, "
                     << "at least 8" << endl << usage << endl;
                return 1;
            }

            block_size = (int)n;
        }
        else if (argv[i][0] == '-') {
            cerr << "Error: unknown option " << argv[i] << endl
                 << usage << endl;
            return 1;
        }
        else {
            if (filename != NULL || i != argc - 1) {
                cerr << "Error: provide exactly one image filename "
                     << "at the end" << endl << usage << endl;
                return 1;
            }

            filename = argv[i];
        }
    }

    if (filename == NULL) { /
        cerr << usage << endl;
        return 1;
    }

    ppm img(block_size); // This creates the image object and reads the input file.

    img.read(filename);

    if (do_gray) { // Applies the argument based on user selection.
        gray(img);
    }

    if (do_flip) {
        flip(img);
    }

    if (do_draw) {
        draw(img, K);
    }

    if (do_lens) {
        lens(img, rho);
    }

    img.write(filename);

    return 0;
}