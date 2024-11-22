#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <cstdlib>
#include "cimage.h"
#include "bmplib.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2) {
        cout << "Usage ./compedit in_img_filename <debug-level debug-options>" << endl;
        return 1;
    }

    CImage img1(argv[1]);

    img1.findComponents();

    if(argc >= 3) {
        int debug = 0;
        debug = atoi(argv[2]);
        if(debug == 1) {
            img1.printComponents();
        }
        else if(debug == 2 || debug == 3) {
            if(argc < 4) {
                cout << "Please provide an extra argument of the filename to save the image." << endl;
            }
            else if(debug == 2){
                img1.labelToRGB(argv[3]);
            }
            else if(debug == 3){
                img1.drawBoundingBoxesAndSave(argv[3]);
            }
        }
        return 0;
    }

    char option;
    int cid;
    bool again = true;
    do {
        img1.printComponents();
        cout << "\nEnter a command [t,f,b,s,q]: " << endl;
        cin >> option;
        if(option == 't') {
            int nr, nc;
            cin >> cid >> nr >> nc;
            if(cin.fail()) {
                break;
            }
            img1.translate(cid, nr, nc);
        }
        else if(option == 'f' || option == 'b') {
            int delta;
            cin >> cid >> delta;
            if(cin.fail()) {
                break;
            }
            if(option == 'f') {
                img1.forward(cid, delta);
            }
            else {
                img1.backward(cid, delta);
            }
        }
        else if(option == 's') {
            string filename;
            cin >> filename;
            img1.save(filename.c_str());
        }
        else {
            again = false;
        }
    } while(again);

    return 0;
}

