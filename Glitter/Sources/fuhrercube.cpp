#include <map>
#include <array>
#include <utility>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <glad/glad.h>
#include <wolf/maputils.hpp>
#include <wolf/import/_3dformats/objfileparser.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void fuhrerCube(
    
    std::vector<unsigned int>&vaovector, 
    std::vector<unsigned int>&vbovector, 
    std::vector<unsigned int>&vertexcount,
    std::vector<std::map<std::string, glm::vec3>>& matdictionaryvector,
    std::vector<std::string>& texturenamevector){    
    
    Wolf::_3DFormats::OBJFileParser objdata("fuhrercube.obj");
    objdata.Serialize();
    
    auto geometryData = objdata.DumpCodeVectorMap("Cube");    
    auto geometrytDataKeys = Wolf::MapUtils::keys(geometryData);
    std::for_each(geometrytDataKeys.begin(), geometrytDataKeys.end(),
    [&](auto&p){
        std::cout << "Generating OpenGl vao's and vbo's for shader:" << p << std::endl;
        auto dataVector = objdata.DumpCodeVector("Cube", p);
        auto dataVectorPtr = dataVector.data();
        auto matInfo = objdata.DumpMaterialInformation("fuhrercube.mtl",p);
        
        auto materialDictionary = std::map<std::string, glm::vec3>{
            std::make_pair("kd", matInfo.kd),
            std::make_pair("ka", matInfo.ka),
            std::make_pair("ke", matInfo.ke),
            std::make_pair("ks", matInfo.ks),
            std::make_pair("op_se_od", matInfo.op_se_od)
        };
        std::cout << "Mat : " << p << " " << matInfo.kd.x << " " << matInfo.kd.y << " " << matInfo.kd.z << std::endl;
        matdictionaryvector.push_back(materialDictionary);
        texturenamevector.push_back(matInfo.texturefile);
        
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