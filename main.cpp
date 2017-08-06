#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <cmath>

using namespace std;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int winx = 640;
int winy = 640;

GLFWwindow * window = nullptr;

int init_gl_window()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(winx,winy,"Diffusion-Limited Aggregation",nullptr,nullptr);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewInit();

    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    return 0;
}


int main(int argc,char ** argv)
{
    srand(time(nullptr));
    if(init_gl_window()<0) return -1;

    shader show;
    show.load_file(GL_VERTEX_SHADER,"vert.glsl");
    show.load_file(GL_FRAGMENT_SHADER,"show.glsl");
    show.create();

    int hx = 1024;
    int hy = 1024;
    float * P_ = new float [hx*hy];
    float * Q_ = new float [hx*hy];
    float * R_ = new float [hx*hy];
    for(int i=0;i<hx;i++)
    {
        for(int j=0;j<hy;j++)
        {
            P_[hx*j+i] = 0.0;
            Q_[hx*j+i] = 0.0;
            R_[hx*j+i] = 0.0;
        }
    }
    for(int k=0;k<360;k++)
    {
        int x_ = (int)(0.5*hx+0.25*hx*cos(2.0*M_PI*k/360.0));
        int y_ = (int)(0.5*hy+0.25*hy*sin(2.0*M_PI*k/360.0));
        P_[hx*y_+x_] = 1.0;
    }
    for(int t=0;t<hy;t++)
    {
        P_[hx*t] = P_[hx*t+hx-1] = 1.0;
    }
    for(int t=0;t<hx;t++)
    {
        P_[t] = P_[hx*(hy-1)+t] = 1.0;
    }
    tex<GL_TEXTURE_2D> H;
    H.bind_();
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,hx,hy,0,GL_RED,GL_FLOAT,R_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    H.unbind_();

    double dt = 1e-3;
    double  g =  9.8;
    screen screen_;

    int N = 100000;
    int M = 100000;
    vector<int> x(N,-1);
    vector<int> y(N,-1);
    for(int n=0;n<N;n++)
    {
        x[n] = rand()%hx;
        y[n] = rand()%hy;
    }
    int px[8] = {-1, 0, 1 , 0, 1,-1, 1,-1};
    int py[8] = { 0,-1, 0 , 1, 1, 1,-1,-1};

    double max_P = 0.0;
    int m = 0;
    int T = 1;
    while (!glfwWindowShouldClose(window))
    {
        for(int n=0;n<N;n++)
        {
            bool b_ = false;
            for(int k=0;k<8;k++)
            {
                int x_ = x[n]+px[k];
                int y_ = y[n]+py[k];
                if(x_ >= 0 && x_ < hx && y_ >= 0 && y_ < hy)
                {
                    if(P_[hx*y_+x_] > 0.0)
                    {
                        P_[hx*y_+x_] += 1e-1;
                        b_ = true;
                        break;
                    }
                }
            }
            if(b_)
            {
                P_[hx*y[n]+x[n]] += 1.0;
                x[n] = rand()%hx;
                y[n] = rand()%hy;
                m++;
            }
            else
            {
                int k = rand()%8;
                x[n] = (hx+x[n]+px[k])%hx;
                y[n] = (hy+y[n]+py[k])%hy;
            }
        }
        for(int i=0;i<hx;i++)
            for(int j=0;j<hy;j++)
                max_P = max_P < P_[hx*j+i] ? P_[hx*j+i] : max_P;
        for(int i=0;i<hx;i++)
            for(int j=0;j<hy;j++)
                Q_[hx*j+i] = 0.005*R_[hx*j+i]+0.995*P_[hx*j+i]/(max_P+1e-1);
        for(int n=0;n<N;n++)
            Q_[hx*y[n]+x[n]] += 1e-1;
        cout << m << endl;
        if(m >= M)
        {
            m = 0;
            T++;
            for(int i=0;i<hx;i++)
                for(int j=0;j<hy;j++)
                    R_[hx*j+i] = 0.005*R_[hx*j+i]+0.995*P_[hx*j+i]/max_P;
            for(int i=0;i<hx;i++)
                for(int j=0;j<hy;j++)
                    P_[hx*j+i] = 0.0;

            for(int k=0;k<360;k++)
            {
                int x_ = (int)(0.5*hx+0.25*hx*cos(2.0*M_PI*k/360.0));
                int y_ = (int)(0.5*hy+0.25*hy*sin(2.0*M_PI*k/360.0));
                P_[hx*y_+x_] = 1.0;
            }

            for(int t=0;t<hy;t++)
                P_[hx*t] = P_[hx*t+hx-1] = 1.0;
            for(int t=0;t<hx;t++)
                P_[t] = P_[hx*(hy-1)+t] = 1.0;
            max_P = 0.0;
            for(int n=0;n<N;n++)
            {
                x[n] = rand()%hx;
                y[n] = rand()%hy;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        H.bind_();
        glTexImage2D(GL_TEXTURE_2D,0,GL_RED,hx,hy,0,GL_RED,GL_FLOAT,Q_);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        H.unbind_();

        show.begin();
            glActiveTexture(GL_TEXTURE0);
            H.bind_();
            screen_.draw();
        show.end();

        glfwSwapBuffers(window);
        glfwPollEvents();
        //max_P += 1.0;
    }

    delete [] P_;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
