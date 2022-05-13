#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 10) Finish this function to create an empty texture with the given size and format
    texture->bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, format, size.x, size.y);
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 4) Finish this function to fill the texture with the data found in "pixels" and generate the mipmaps if requested
    // Bind texture 
    texture->bind();
    //To send image data to the image we will use glTexImage2D function
    // This function sends texture data from the RAM to the VRAM.
    // Parameters:
    // target (GLenum): The texture to which we should send the data.
    // level (GLint): The mip level in which this data should be stored.
    // internalformat (GLint): The format in which the texture data will be stored in the VRAM. Since we have 4 channels (8 bits each) , we will store it as GL_RGBA8.
    // width, height (GLsizei): the width and height of the texture in pixels. Here we picked 2x2 so the data will be stored as 2 rows where each row contains 2 pixels.
    // border (GLint): This does nothing and it must be 0. In old OpenGL, this was used to indicate whether the texture would have a border or not.
    // format (GLenum): this is the format of the data as it is stored in the array "data" on the RAM. Since we have 4 components, we use GL_RGBA.
    // type (GLenum): this is the data type of each component in the array "data" on the RAM. We stored it as uint8 so we pick GL_UNSIGNED_BYTE.
    // data (const void*): this is the pointer to the data on the RAM. The function will read data from this location and send it to the GPU.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
    // generate the mipmaps if requested
    // This function will generate the mip map for the texture. It will automatically generate all the mip level till we reach a mip level whose size is 1x1 pixel.
    // The mip levels are generate by averaging each 2x2 pixels into 1 pixel in the higher level.
    if(generate_mipmap == true)
        glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(pixels); //Free image data after uploading to GPU
    return texture;
}