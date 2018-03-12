//
//  main.cpp
//  OpenGL_test
//
//  Created by 吴宇飞 on 2018/2/7.
//  Copyright © 2018年 gyd. All rights reserved.
//

#include <stdio.h>
#include "GLTools.h"
#include <glut/glut.h>
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//相机frame
GLFrame viewFrame;

//透视投影矩阵
GLFrustum viewFrustum;

//模型试图矩阵
GLMatrixStack modelViewStack;

//投影视图矩阵
GLMatrixStack projectionViewStack;

//定义一个着色管理器
GLShaderManager shaderManager;

//定义一个图形批次容器
GLTriangleBatch torusBatch;

GLGeometryTransform transformPipeline;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

void changeSize(int w , int h)
{
    glViewport(0, 0, w, h);
    
    //初始化透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    //把透视矩阵加载到透视矩阵堆栈中
    projectionViewStack.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewStack, projectionViewStack);
}

void specialKeyClick(int key , int x , int y)
{
    if (key == GLUT_KEY_UP) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (iCull) {
        
        //开启背面剔除
        glEnable(GL_CULL_FACE);
        //指定剔除模式
        glFrontFace(GL_CCW);
        //执行剔除
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    
    
    modelViewStack.PushMatrix(viewFrame);
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    //默认光源着色器
    //参数1:GLT_SHADER_DEFAULT_LIGHT
    //参数2:模型视图矩阵
    //参数3:投影矩阵
    //参数4:颜色
    
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    //绘制
    torusBatch.Draw();
    //出栈
    modelViewStack.PopMatrix();
    glutSwapBuffers();
}

void processMenu(int value)
{
    switch (value) {
        case 1:
            if (iCull) {
                iCull = 0;
            }
            else
            {
                iCull = 1;
            }
            break;
        case 2:
            if (iDepth) {
                iDepth = 0;
            }
            else
            {
                iDepth = 1;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void setUp()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1);
    
    shaderManager.InitializeStockShaders();
    
    //观察者，参数表示Z轴的值
    viewFrame.MoveForward(8.0f);
    
    //创建一个游泳圈
    /*
     void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
     参数1:GLTriangleBatch;
     参数2:外圆半径,从圆心到外圆的距离
     参数3:内边圆半径,从圆心到内圆的距离
     参数4\5:三角形带的数量 numMajor = 2 * numMinor
     */
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    glPointSize(4.0f);
}

int main(int argc,char *argv[])
{
    //设置当前的工作目录
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    glutInit(&argc, argv);
    
    //初始化缓存区，GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指双缓存区、颜色缓存区、深度缓存区、模版缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    //设置窗口大小
    glutInitWindowSize(800, 600);
    glutCreateWindow("🍩");
    
    //注册回调函数
    
    //重塑函数
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutSpecialFunc(specialKeyClick);
    glutCreateMenu(processMenu);
    
    //设置右击菜单选项
    //添加右击菜单栏选项
    //参数1: 菜单栏中选项的内容
    //参数2: 1
    glutAddMenuEntry("背面剔除", 1);
    glutAddMenuEntry("深度测试", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    /*
     初始化一个GLEW库,确保OpenGL API对程序完全可用。
     在试图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题
     */
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;
        
    }
    
    //设置渲染环境
    setUp();
    
    //程序运行死循环
    glutMainLoop();
    return 0;
    
}


