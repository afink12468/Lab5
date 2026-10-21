#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
using std::cerr;
using std::endl;
using std::string;

#include "Support.h"

ppm::ppm(int n) // constructor that sets expected id and max value, as well as the file handling block size.
{
magicid = "P6";
maxvalue = 255;
block_size = n;
Nrow = 0;
Ncol = 0;
img = NULL;
}

ppm::ppm(const ppm &other) // copy constructor that copies every rgb value from another image.
{
magicid = other.magicid;
Nrow = other.Nrow;
Ncol = other.Ncol;
maxvalue = other.maxvalue;
block_size = other.block_size;
img = NULL;

    if (Nrow > 0 && Ncol > 0) {
img = new RGB[Nrow * Ncol];

for (int i = 0; i < Nrow * Ncol; i++) {
    img[i] = other.img[i];
        }
    }
}

ppm::~ppm() // destructor the frees pixel memory
{
delete[] img;
}

RGB *ppm::operator[](int i)
{
return img + i * Ncol;
}

void ppm::read(const string &filename) // this function opens the ppm file, checks for errors, and then reads the data in block_size values.
{
FILE *fp = fopen(filename.c_str(), "rb");

    if (fp == NULL) {
  cerr << "Error: cannot open input file "
   << filename << endl;
    exit(1);
    }

    char id[3];

    if (fscanf(fp, "%2s", id) != 1 || magicid != id) {
        cerr << "Error: invalid PPM magic identifier" << endl;
        fclose(fp);
        exit(1);
    }

    int vals[3];

    for (int k = 0; k < 3; k++) {
        int c = fgetc(fp);

        while (c == '#' || isspace(c)) {
        if (c == '#') {
        while (c != '\n' && c != EOF) {
        c = fgetc(fp);
                }
            } else {
                c = fgetc(fp);
            }
        }

        if (c != EOF) {
            ungetc(c, fp);
        }

        if (fscanf(fp, "%d", &vals[k]) != 1) {
            cerr << "Error: invalid PPM header" << endl;
            fclose(fp);
            exit(1);
        }
    }

    Ncol = vals[0];
    Nrow = vals[1];
    maxvalue = vals[2];

    if (maxvalue != 255 || Nrow <= 0 || Ncol <= 0) {
        cerr << "Error: invalid PPM dimensions or max value"
             << endl;
        fclose(fp);
        exit(1);
    }

    int ch;

    do {
        ch = fgetc(fp);
    } while (ch != '\n' && ch != EOF);

    if (ch == EOF) {
        cerr << "Error: missing image data" << endl;
        fclose(fp);
        exit(1);
    }

    delete[] img;
    img = new RGB[Nrow * Ncol];

    int total = Nrow * Ncol * sizeof(RGB);
    int bytes_read = 0;
    char *data = (char *)img;

    while (bytes_read < total) {
    int amount = block_size;

    if (amount > total - bytes_read) {
            amount = total - bytes_read;
        }

    int count = fread(data + bytes_read, 1, amount, fp);

    if (count != amount) {
            cerr << "Error: not enough RGB image data"
                 << endl;
            fclose(fp);
            exit(1);
        }

        bytes_read += count;
    }

    fclose(fp);
}

void ppm::write(const string &filename) // This function writes the image to a new file. The rgb data is written in block_size chunks.
{
    string output = filename;
    string::size_type pos = output.rfind('.');
    string::size_type slash = output.rfind('/');

    if (pos == string::npos ||
        (slash != string::npos && pos < slash)) {
        output += "_mod.ppm";
    } else {
        output.insert(pos, "_mod");
    }

    FILE *fp = fopen(output.c_str(), "wb");

    if (fp == NULL) {
        cerr << "Error: cannot open output file "
             << output << endl;
        exit(1);
    }

    fprintf(fp, "%s\n%d %d\n%d\n",
     magicid.c_str(), Ncol, Nrow, maxvalue);

    int total = Nrow * Ncol * sizeof(RGB);
    int bytes_written = 0;
    char *data = (char *)img;

    while (bytes_written < total) {
        int amount = block_size;

        if (amount > total - bytes_written) {
            amount = total - bytes_written;
        }

        int count = fwrite(data + bytes_written, 1, amount, fp);

        if (count != amount) {
            cerr << "Error: could not write all image data"
                 << endl;
            fclose(fp);
            exit(1);
        }

        bytes_written += count;
    }

    fclose(fp);
}

void gray(ppm &img) // Converts the image to gray by setting the pixel of R,G, and B to R+G+B / 3.
{
    for (int i = 0; i < img.get_Nrow(); i++) {
        for (int j = 0; j < img.get_Ncol(); j++) {
      int R = img[i][j].R;
      int G = img[i][j].G;
      int B = img[i][j].B;

      byte Y = (R + G + B) / 3;

     img[i][j].R = Y;
     img[i][j].G = Y;
      img[i][j].B = Y;
        }
    }
}

void flip(ppm &img) // This function flips the image from left to right by swapping each pixel with its matching pixel in the right half.
{
    int cols = img.get_Ncol();

    for (int i = 0; i < img.get_Nrow(); i++) {
        for (int j = 0; j < cols / 2; j++) {
            RGB temp = img[i][j];

            img[i][j] = img[i][cols - 1 - j];
            img[i][cols - 1 - j] = temp;
        }
    }
}

void draw(ppm &img, int K) // this function creates a drawing of the image by finding the largest gray value for each pixel and then setting the pixel to 255-(ymax-y)
{
    ppm img_copy = img;

    int rows = img.get_Nrow();
    int cols = img.get_Ncol();

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int R = img_copy[i][j].R;
            int G = img_copy[i][j].G;
            int B = img_copy[i][j].B;

            int Y = (R + G + B) / 3;
            int Ymax = 0;

            for (int di = -K; di <= K; di++) {
            for (int dj = -K; dj <= K; dj++) {
            int ni = i + di;
            int nj = j + dj;

            if (ni >= 0 && ni < rows &&
            nj >= 0 && nj < cols) {
            int nr = img_copy[ni][nj].R;
            int ng = img_copy[ni][nj].G;
            int nb = img_copy[ni][nj].B;

            int neighborY = (nr + ng + nb) / 3;

            if (neighborY > Ymax) {
            Ymax = neighborY;
                        }
                    }
                }
            }

            byte value = 255 - (Ymax - Y);

            img[i][j].R = value;
            img[i][j].G = value;
            img[i][j].B = value;
        }
    }
}

void lens(ppm &img, float rho) // This function introduces a lens that is centered in the image.
{
    ppm img_copy = img;

    int rows = img.get_Nrow();
    int cols = img.get_Ncol();

    int ci = rows / 2;
    int cj = cols / 2;

    int rmax = rows / 2;

    if (cols / 2 < rmax) {
        rmax = cols / 2;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float di = i - ci;
            float dj = j - cj;

            float r = sqrtf(di * di + dj * dj);

            if (r < rmax) {
            float z = sqrtf((float)rmax * rmax - r * r);

            float dzi = di / sqrtf(di * di + z * z);
            float dzj = dj / sqrtf(dj * dj + z * z);

            float bi = (1.0f - 1.0f / rho) * asinf(dzi);
            float bj = (1.0f - 1.0f / rho) * asinf(dzj);

            int ip = (int)(i - z * tanf(bi) + 0.5f);
            int jp = (int)(j - z * tanf(bj) + 0.5f);

            if (ip >= 0 && ip < rows &&
                    jp >= 0 && jp < cols) {
                    img[i][j] = img_copy[ip][jp];
                }
            }
        }
    }
}