#include <array>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <glad/glad.h>
#include <wolf/maputils.hpp>
#include <wolf/import/_3dformats/objfileparser.hpp>


void fuhrerCube(std::vector<unsigned int>& vaovector, std::vector<unsigned int>&vbovector, std::vector<unsigned int>& vertexcount){

    Wolf::_3DFormats::OBJFileParser objdata("fuhrercube.obj");
    objdata.Serialize();
    
    auto geometryData = objdata.DumpCodeVectorMap("Cube");    
    auto geometrytDataKeys = Wolf::MapUtils::keys(geometryData);
    std::for_each(geometrytDataKeys.begin(), geometrytDataKeys.end(),
    [&](auto&p){
        std::cout << "Generating OpenGl vao's and vbo's for shader:" << p << std::endl;
        auto dataVector = objdata.DumpCodeVector("Cube", p);
        auto dataVectorPtr = dataVector.data();
        unsigned int vbo; 
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //-- Grab Vao --/
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //-- VBO Header --/
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dataVector.size(), dataVectorPtr, GL_STATIC_DRAW);
        //-- Vertices --/
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	    //-- Normals --/
	    glEnableVertexAttribArray(1);
    	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    	//-- Texture Coordinates
    	glEnableVertexAttribArray(2);
    	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    	//-- Release Vao --/
    	glBindVertexArray(0);

    	//Number of vertices
    	auto nvertices = dataVector.size() / 8;

		vaovector.push_back(vao);
        vbovector.push_back(vbo);
        vertexcount.push_back(nvertices);
    } 
    );

}