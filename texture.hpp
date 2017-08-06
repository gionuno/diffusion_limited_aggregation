#ifndef TEXTURE_HPP_INCLUDED
#define TEXTURE_HPP_INCLUDED

#include <GL/glew.h>
#include <GL/gl.h>

template<GLuint type_>
struct tex
{
    GLuint  idx;
    tex()
    {
        glGenTextures(1,&idx);
    }
    ~tex()
    {
        if(idx)
            glDeleteTextures(1,&idx);
    }
    void bind_()
    {
        glBindTexture(type_,idx);
    }
    void unbind_()
    {
        glBindTexture(type_,0);
    }
};

#endif // TEXTURE_HPP_INCLUDED
