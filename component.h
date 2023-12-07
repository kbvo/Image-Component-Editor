#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstdint> // for uint8_t

/**
 * @brief Models a row/column location and provides an
 *        initializing constructor and comparison ability
 *
 *  COMPLETE - DO NOT ALTER
 */
struct Location {
    int row;
    int col;
    // Default constructor
    Location() {
        row = -1;
        col = -1;
    }
    // Initializing constructor
    Location(int r, int c) {
        row = r;
        col = c;
    }

};


/**
 * @brief Models the area of pixels representing a component in the image
 *
 */
struct Component {
    /**
     * @brief Construct a Component object
     *
     */
    Component();

    /**
     * @brief Construct a Component object using
     *        the bounding box info and label provided
     *
     */
    Component(Location origUL, int h, int w, int mylabel);

    /**
     * @brief Destroy the Component object
     *
     */
    ~Component();

    /**
     * @brief No accessors necessary, since members are public
     *
     */


    //===========================================================
    // Data members

    // original upper-left location
    Location ulOrig;
    // translated upper-left location
    Location ulNew;
    // height and width
    int height;
    int width;
    // label
    int label;
};
#endif