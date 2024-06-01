#include "component.h"
#include "cimage.h"
#include "bmplib.h"
#include <deque>
#include <iomanip>
#include <iostream>
#include <cmath>
using namespace std;

// TO DO: Complete this function
CImage::CImage(const char* bmp_filename)
{
    //  Note: readRGBBMP dynamically allocates a 3D array
    //    (i.e. array of pointers to pointers (1 per row/height) where each
    //    point to an array of pointers (1 per col/width) where each
    //    point to an array of 3 unsigned char (uint8_t) pixels [R,G,B values])

    // ================================================
    // TO DO: call readRGBBMP to initialize img_, h_, and w_;
    img_ = readRGBBMP(bmp_filename, h_, w_);


    // Leave this check
    if(img_ == NULL) {
        throw std::logic_error("Could not read input file");
    }

    // Set the background RGB color using the upper-left pixel
    for(int i=0; i < 3; i++) {
        bgColor_[i] = img_[0][0][i];
    }

    // ======== This value should work - do not alter it =======
    // RGB "distance" threshold to continue a BFS from neighboring pixels
    bfsBgrdThresh_ = 60;

    // ================================================
    // TO DO: Initialize the vector of vectors of labels to -1
    labels_ = vector<vector<int>>(h_, vector<int>(w_, -1));
}

// TO DO: Complete this function
CImage::~CImage()
{
    deallocateImage(img_);
}

// Complete - Do not alter
bool CImage::isCloseToBground(uint8_t p1[3], double within) {
    // Computes "RGB" (3D Cartesian distance)
    double dist = sqrt( pow(p1[0]-bgColor_[0],2) +
                        pow(p1[1]-bgColor_[1],2) +
                        pow(p1[2]-bgColor_[2],2) );
    return dist <= within;
}

// TO DO: Complete this function
size_t CImage::findComponents()
{
  size_t componentCount = 0;
  
  // Loop through each pixel in image 
  // from top row to bottom row left to right
  for (int i = 0; i < h_; i++) {
    for (int j = 0; j < w_; j++) {
      // if the pixel is not labeled and not part of background
      if (labels_[i][j] == -1 && !isCloseToBground(img_[i][j], bfsBgrdThresh_)) {
        // find compoent using BFS
        Component newComponent = bfsComponent(i, j, componentCount);
        // add component to vector of components
        components_.push_back(newComponent);
        // update component label
        componentCount++;
      }
    }
  }
  return componentCount;
}

// Complete - Do not alter
void CImage::printComponents() const
{
    cout << "Height and width of image: " << h_ << "," << w_ << endl;
    cout << setw(4) << "Ord" << setw(4) << "Lbl" << setw(6) << "ULRow" << setw(6) << "ULCol" << setw(4) << "Ht." << setw(4) << "Wi." << endl;
    for(size_t i = 0; i < components_.size(); i++) {
        const Component& c = components_[i];
        cout << setw(4) << i << setw(4) << c.label << setw(6) << c.ulNew.row << setw(6) << c.ulNew.col
             << setw(4) << c.height << setw(4) << c.width << endl;
    }

}


// TODO: Complete this function
int CImage::getComponentIndex(int mylabel) const
{
  for (size_t i = 0; i < components_.size(); i++) {
    // if component label matches my label
    if (components_[i].label == mylabel) {
      return static_cast<int>(i); // component found
    }
  }
  return -1; // component not found
}


// Nearly complete - TO DO:
//   Add checks to ensure the new location still keeps
//   the entire component in the legal image boundaries
void CImage::translate(int mylabel, int nr, int nc)
{
    // Get the index of specified component
    int cid = getComponentIndex(mylabel);
    if(cid < 0) {
        return;
    }
    int h = components_[cid].height;
    int w = components_[cid].width;

    // ==========================================================
    // ADD CODE TO CHECK IF THE COMPONENT WILL STILL BE IN BOUNDS
    // IF NOT:  JUST RETURN.
    // if translated location is out of bounds
    if (nr < 0 || nc < 0 || nr + h > h_ || nc + w > w_) {
      return; // translated location is out of bounds
    }
    // ==========================================================

    // If we reach here we assume the component will still be in bounds
    // so we update its location.
    Location nl(nr, nc);
    components_[cid].ulNew = nl;
}

// TO DO: Complete this function
void CImage::forward(int mylabel, int delta)
{
    int cid = getComponentIndex(mylabel);
    if(cid < 0 || delta <= 0) {
        return;
    }

    // if moving component delta times forward is out of bounds
    if (cid + delta >= static_cast<int>(components_.size())) {
        // adjust delta to move cid as far forward as possible
        delta = components_.size() - cid - 1;
    }
    
    // move component delta times forward by swapping
    for (int i = 0; i < delta; i++) {
        Component temp = components_[cid + i];
        components_[cid + i] = components_[cid + i + 1];
        components_[cid + i + 1] = temp;
    }
}

// TO DO: Complete this function
void CImage::backward(int mylabel, int delta)
{
    int cid = getComponentIndex(mylabel);
    if(cid < 0 || delta <= 0) {
        return;
    }

    // if component moving delta times backward is out of bounds
    if (delta > cid) {
        // adjust delta so cid moves as far back as possible
        delta = cid;
    }

    // move component delta times backward by swapping
    for (int i = 0; i < delta; i++) {
        Component temp = components_[cid - i];
        components_[cid - i] = components_[cid - i - 1];
        components_[cid - i - 1] = temp;
    }
}

// TODO: complete this function
void CImage::save(const char* filename)
{
    // Create another image filled in with the background color
    uint8_t*** out = newImage(bgColor_);
    
    for (size_t i = 0; i < components_.size(); i++) {
       // initialize current component
       Component currComponent = components_[i];
       // initialize new/translated upper left row of component
       int ulNewRow = currComponent.ulNew.row;
       // initialize new/translated upper left column of component
       int ulNewCol = currComponent.ulNew.col;
       // initialize original upper left row of component
       int ulOldRow = currComponent.ulOrig.row;
       // initialize original upper left column of component
       int ulOldCol = currComponent.ulOrig.col;

       // place foreground pixels given new bounding box location
       for (int row = 0; row < currComponent.height; row++) {
           for (int col = 0; col < currComponent.width; col++) {
               // if old pixel is part of component
               if (labels_[ulOldRow + row][ulOldCol + col] == currComponent.label) {
                   // copy pixel to new image at new location
                   // from old image at old location
                   for (int k = 0; k < 3; k++) {
                    out[ulNewRow + row][ulNewCol + col][k] = img_[ulOldRow + row][ulOldCol + col][k];
                   }
               }
           }
       }
   }
    writeRGBBMP(filename, out, h_, w_); // save image to file
    deallocateImage(out); // deallocate image
}

// Complete - Do not alter - Creates a blank image with the background color
uint8_t*** CImage::newImage(uint8_t bground[3]) const
{
    uint8_t*** img = new uint8_t**[h_];
    for(int r=0; r < h_; r++) {
        img[r] = new uint8_t*[w_];
        for(int c = 0; c < w_; c++) {
            img[r][c] = new uint8_t[3];
            img[r][c][0] = bground[0];
            img[r][c][1] = bground[1];
            img[r][c][2] = bground[2];
        }
    }
    return img;
}

// To be completed
void CImage::deallocateImage(uint8_t*** img) const
{
    if (img != nullptr) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
            delete[] img[i][j];
        }
        delete[] img[i];
        }
        delete[] img;
    }
    img = nullptr;
}

// TODO: Complete the following function
Component CImage::bfsComponent(int pr, int pc, int mylabel)
{
    // Arrays to help produce neighbors easily in a loop
    // by encoding the **change** to the current location.
    // Goes in order N, NW, W, SW, S, SE, E, NE
    int neighbor_row[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int neighbor_col[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // initialize upper left location
    Location upperLeft(pr, pc);
    // initialize component for bounding box
    Component boundingBox(upperLeft, 0, 0, mylabel);
    
    deque<Location> bfsQueue; // create queue for BFS
    bfsQueue.push_back(upperLeft); // add starting pixel location to queue
    labels_[pr][pc] = mylabel; // mark starting pixel location with label
    int maxRow = pr, maxCol = pc;

    while (!bfsQueue.empty()) { // while queue isn't empty
      Location currLoc = bfsQueue.front(); // start with starting location
      bfsQueue.pop_front(); // empty queue
      
      // find minimum row of any pixel in component and
      // assign it to new and old upper left row of bounding box
      if (currLoc.row < boundingBox.ulNew.row) {
        boundingBox.ulOrig.row = currLoc.row;
        boundingBox.ulNew.row = currLoc.row;
      }
      // find minimum column of any pixel in component and
      // assign it to new and old upper left column of bounding box
      if (currLoc.col < boundingBox.ulNew.col) {
        boundingBox.ulOrig.col = currLoc.col;
        boundingBox.ulNew.col = currLoc.col;
      }
      // find maximum row of any pixel in component
      if (currLoc.row > maxRow) {
        maxRow = currLoc.row;
      }
      // find maximum column of any pixel in component
      if (currLoc.col > maxCol) {
        maxCol = currLoc.col;
      }

      // BFS search
      for (int i = 0; i < 8; i++) { // explore 8 connected neighbors
        // initialize current neighbor's row and column
        int currNeighborRow = currLoc.row + neighbor_row[i];
        int currNeighborCol = currLoc.col + neighbor_col[i];

        // if neighbor is not out of bounds, not labeled,
        // and not part of background
        if (currNeighborRow >= 0 && currNeighborRow < h_ && 
        currNeighborCol >= 0 && currNeighborCol < w_ &&
        labels_[currNeighborRow][currNeighborCol] == -1 &&
        !isCloseToBground(img_[currNeighborRow][currNeighborCol], bfsBgrdThresh_)) {
          // label neighbor
          labels_[currNeighborRow][currNeighborCol] = mylabel;
          // add neighbor to queue
          bfsQueue.push_back(Location(currNeighborRow, currNeighborCol));
        }
      }
    }
    // find bounding box height using old upper left row
    boundingBox.height = maxRow - boundingBox.ulOrig.row + 1;
    // find bounding box width using old upper left column
    boundingBox.width = maxCol - boundingBox.ulOrig.col + 1;
    
    return boundingBox;
}

// Complete - Debugging function to save a new image
void CImage::labelToRGB(const char* filename)
{
    //multiple ways to do this -- this is one way
    vector<uint8_t[3]> colors(components_.size());
    for(unsigned int i=0; i<components_.size(); i++) {
        colors[i][0] = rand() % 256;
        colors[i][1] = rand() % 256;
        colors[i][2] = rand() % 256;
    }

    for(int i=0; i<h_; i++) {
        for(int j=0; j<w_; j++) {
            int mylabel = labels_[i][j];
            if(mylabel >= 0) {
                img_[i][j][0] =  colors[mylabel][0];
                img_[i][j][1] =  colors[mylabel][1];
                img_[i][j][2] =  colors[mylabel][2];
            } else {
                img_[i][j][0] = 0;
                img_[i][j][1] = 0;
                img_[i][j][2] = 0;
            }
        }
    }
    writeRGBBMP(filename, img_, h_, w_);
}

// Complete - Do not alter
const Component& CImage::getComponent(size_t i) const
{
    if(i >= components_.size()) {
        throw std::out_of_range("Index to getComponent is out of range");
    }
    return components_[i];
}

// Complete - Do not alter
size_t CImage::numComponents() const
{
    return components_.size();
}

// Complete - Do not alter
void CImage::drawBoundingBoxesAndSave(const char* filename)
{
    for(size_t i=0; i < components_.size(); i++){
        Location ul = components_[i].ulOrig;
        int h = components_[i].height;
        int w = components_[i].width;
        for(int i = ul.row; i < ul.row + h; i++){
            for(int k = 0; k < 3; k++){
                img_[i][ul.col][k] = 255-bgColor_[k];
                img_[i][ul.col+w-1][k] = 255-bgColor_[k];

            }
            // cout << "bb " << i << " " << ul.col << " and " << i << " " << ul.col+w-1 << endl; 
        }
        for(int j = ul.col; j < ul.col + w ; j++){
            for(int k = 0; k < 3; k++){
                img_[ul.row][j][k] = 255-bgColor_[k];
                img_[ul.row+h-1][j][k] = 255-bgColor_[k];

            }
            // cout << "bb2 " << ul.row << " " << j << " and " << ul.row+h-1 << " " << j << endl; 
        }
    }
    writeRGBBMP(filename, img_, h_, w_);
}
