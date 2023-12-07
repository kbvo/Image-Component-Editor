#ifndef CIMAGE_H
#define CIMAGE_H

#include "bmplib.h"
#include "component.h"
#include <vector>
#include <string>


/**
 * @brief Reads, owns, and manages the 3D array representing an
 *        RGB color image and allows manipulations to create a
 *        new updated image.
 *
 */
class CImage {
public:
    /**
     * @brief Construct a new CImage object by reading an 8-bit
     *         color BMP image.
     *
     * @param bmp_filename C-string of the filename to read
     */
    CImage(const char* bmp_filename);

    /**
     * @brief Destructor
     *
     */
    ~CImage();

    /**
     * @brief Finds connected foreground pixels representing a component 
     *        and creates Component objects from them.
     *
     *        Post-condition:  Component objects should be stored
     *        in order of the top most pixel (and then left to right if
     *        multiple components top pixel are in the same row).
     *
     * @return count of how many components were identified
     *         (remember size_t is just an unsigned integer type)
     */
    size_t findComponents();

    /**
     * @brief Print bounding box information for all Components in
     *        a specific format used for debugging/grading.
     *
     * @param filename
     */
    void printComponents() const;

    /**
     * @brief Debugging methods that save new images with the components 
     *        filled with a randomly selected color (for labelToRGB) 
     *        or with bounding boxes drawn around each component.
     *
     * @param filename
     */
    void labelToRGB(const char* filename);
    void drawBoundingBoxesAndSave(const char* filename);

    /**
     * @brief translate a component to a new location
     *
     * @param mylabel Label of the component to translate
     * @param nr New upper-left corner row location 
     * @param nc New upper-left corner column location
     */
    void translate( int mylabel, int nr, int nc);

    /**
     * @brief move a component layer forward or backward
     *        (component/layer 0 is the back)
     *
     *
     * @param mylabel Label of the component to move forward/backward
     * @param delta positive amount to move the component/layer forward
     *              or backward. (No negative inputs.) 
     *              If the amount would place the layer out of bounds
     *              (less-than 0 or greater-than the max index), use 0
     *              or the max index (respectively),instead.
     */
    void forward( int mylabel, int delta);
    void backward( int mylabel, int delta);

    /**
     * @brief Renders and save the translated and ordered 
     *        component layers to a new image.
     *
     * filename - name for the new image
     */
    void save(const char* filename);

    /**
     * @brief Accessor to get a pointer to the i-th Component 
     *        for debugging purposes.  You should NOT need to use this.
     *
     * @param[in] i index of the Component to get
     * @return Component
     */
    const Component& getComponent(size_t i) const;

    /**
     * @brief Accessor to get how many Components were found. Used
     *        for debugging purposes.  You should NOT need to use this.
     *
     * @return size_t
     */
    size_t numComponents() const;

private:
    //===========================================================
    //  Private helper functions

    /**
     * @brief Returns true if the given pixel is within the given
     *        threshold distance of the background color
     *
     * @param pixel Pointer to an array of 3 uint8_t (i.e. an RGB value)
     * @param within distance threshold - Usually one of startBgrdThresh_
     *                                    or bfsBgrdThresh_
     * @return true
     * @return false
     */
    bool isCloseToBground(uint8_t pixel[3], double within);

    /**
     * @brief Create a Component object starting with one of its pixels and
     *         then using an 8-connected neighbor BFS to find all pixels that
     *         are part of this Component.
     *
     * @param pr Row of the first pixel of a new Component to explre
     * @param pc Col of the first pixel of a new Component to explore
     * @param mylabel Label (unique integer ID) to use for this component
     * @return Component corresponding to this newly found digit
     */
    Component bfsComponent(int pr, int pc, int mylabel);

    /**
     * @brief Get the index of the component with mylabel or
     *        -1 if no component with mylabel exists
     * 
     * @param mylabel Label of the component 
     * @return int index in the Component storage
     */
    int getComponentIndex(int mylabel) const;

    /**
     * @brief Creates an RGB image array with the same dimensions
     *        as the image read by the constructor.  Sets all pixels
     *        to the given background color
     *
     * @param bground - RGB value to initialize all pixels
     * @return uint8_t*** pointer to the new image array
     */
    uint8_t*** newImage(uint8_t bground[3]) const;

    /**
     * @brief Deallocates all the memory of the input 3D array
     *
     * @param img pointer to the 3D array to deallocate
     */
    void deallocateImage(uint8_t*** img) const;

    //===========================================================
    //  Add other private functions as necessary



    //===========================================================
    //  Data members

    /// Pointer to the 3D dynamically allocated array of pixels
    uint8_t*** img_;
    /// Height and width of input image (not always 256x256)
    int h_;
    int w_;
    // RGB value of the image background
    uint8_t bgColor_[3];
    double bfsBgrdThresh_;      // To start/continue the BFS (place neighbors
                                //  in the BFS queue), a pixel must be
                                //  more than this distance

    // height x width 2D vector of integers indicating what label each
    //  pixel in img_ belongs to (-1 is used to indicate background)
    std::vector<std::vector<int> > labels_;

    /// Vector to store the Components we find in the image
    std::vector<Component> components_;

    //===========================================================
    // Add any other data member(s)

};
#endif