#include <array>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <wolf/maputils.hpp>
#include <wolf/import/_3dformats/objfileparser.hpp>

void fuhrerCube(std::vector<unsigned int>& vaovector, std::vector<unsigned int>&vbovector){

    Wolf::_3DFormats::OBJFileParser objdata("fuhrercube.obj");
    objdata.Serialize();
    
    auto geometryData = objdata.DumpCodeVectorMap("Cube");    
    auto geometytDataKeys = Wolf::MapUtils::keys(geometryData);
    std::for_each(geometytDataKeys.begin(), geometytDataKeys.end(),
    [&](auto&p){
        std::cout << "Generating OpenGl vao's and vbo's for " << p << std::endl;
    } 
    );

}