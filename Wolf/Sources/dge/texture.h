namespace DGE {
    class Texture{
    public:
        DGE::xTextureID textureId{};
        int mWidth, mHeight, mChannels;
        Texture(char* texturePath){
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            //Wrapping Parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            //Filtering Parametes
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            auto pData = stbi_load(texturePath, &mWidth, &mHeight, &mChannels, 0);
            if (!pData){
                spdlog::critical("{:s}:{:d} -- Texture::Texture() stbi_load failed", __FILE__, __LINE__);
                std::exit(-1);
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(pData);

        }
    };
}