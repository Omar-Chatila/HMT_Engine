#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "util.h"

/*
# Motion file saved by ICSPACE MotionTool
# File format version: 0.1
#
# First row: list of joints separated by ' ' used in this animation. See the H-Anim standard (http://www.web3d.org/documents/specifications/19774-1/V2.0/index.html) for hierarchy information.
# Other rows: Motion data with metadata. One row per frame: 
#   timestamp (ms, optional, 0 if not set.) followed by ','
#   root translation (x y z followed by ',')
#   joint rotations (w1 x1 y1 z1, w2 x2 y2 z2, ..., quaternions for each joint, local w.r.t hierarchy), followed by ','
#   joint translations (absolute w.r.t. to root of world, coordinates separated by ' ', data for each joint separated by ','. Finally followed by ';')
*/

class Input_parser {
private:
    std::vector<Frame> trajectories;
    const char* file;

    std::vector<std::string> readAllLines();
    Frame line_to_frame(std::string &line, int time_frame);
public:
    Input_parser(const char* path_to_file);
    ~Input_parser();
    std::vector<Frame> get_trajectories();
};

#endif