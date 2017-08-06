#ifndef FRAME_HPP_INCLUDED
#define FRAME_HPP_INCLUDED

#include <GL/glew.h>
#include <GL/gl.h>

struct frame
{
    GLuint fbo;

    Gluint ctex;
    GLuint dtex;

    int winx;
    int winy;

    frame(int winx_,int winy_)
    {
        winx = winx_;
        winy = winy_;
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);

        glGenTextures(1,&ctex);
        glBindTexture(GL_TEXTURE_2D,ctex);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,winx,winy,0,GL_RGBA32F,GL_FLOAT,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,ctex,0);

        glGenTextures(1,&dtex);
        glBindTexture(GL_TEXTURE_2D,dtex);
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,winx,winy,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,dtex,0);

        GLenum * drawbuffers = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};
        glDrawBuffer(1,drawbuffers);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }
    ~frame()
    {
        glDeleteFramebuffers(1,&fbo);
        glDeleteTextures(1,&ctex);
        glDeleteTextures(1,&dtex);
    }
    void begin()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);
        glViewport(0,0,winx,winy);
    }
    void end()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }
};

#endif // FRAME_HPP_INCLUDED
