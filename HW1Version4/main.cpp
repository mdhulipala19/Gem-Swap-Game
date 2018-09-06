/*
 Mahesh Dhulipala HW1
 Implemented:
 
 1.Stellar
 2.Lovely
 3.Heartbeat
 4.Gyro
 5.Swap
 6.Legal
 7.Bomb
 8. Three-in-a-line
 9.Dramatic exit
 10. Turn the tables
 11. Sticky
 12. Quake
 
 Features have Comments with above names to identify where they are implemented
 //Reset Quake and Turn the Tables with 'R'
 
 Not yet Implemented:
 
 2. Skyfall
 3. Feather fall
 4. Downshift
 
 
 */

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>        // must be downloaded
#include <GL/freeglut.h>    // must be downloaded unless you have an Apple
#endif

#include <ctime>
const unsigned int windowWidth = 512, windowHeight = 512;
double lastTime;
double dt;



bool keyboardState[256];

// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;

void getErrorInfo(unsigned int handle)
{
    int logLen;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0)
    {
        char * log = new char[logLen];
        int written;
        glGetShaderInfoLog(handle, logLen, &written, log);
        printf("Shader log:\n%s", log);
        delete log;
    }
}

// check if shader could be compiled
void checkShader(unsigned int shader, char * message)
{
    int OK;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
    if (!OK)
    {
        printf("%s!\n", message);
        getErrorInfo(shader);
    }
}

// check if shader could be linked
void checkLinking(unsigned int program)
{
    int OK;
    glGetProgramiv(program, GL_LINK_STATUS, &OK);
    if (!OK)
    {
        printf("Failed to link shader program!\n");
        getErrorInfo(program);
    }
}

// row-major matrix 4x4
struct mat4
{
    float m[4][4];
public:
    mat4() {}
    mat4(float m00, float m01, float m02, float m03,
         float m10, float m11, float m12, float m13,
         float m20, float m21, float m22, float m23,
         float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }
    
    mat4 operator*(const mat4& right)
    {
        mat4 result;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) result.m[i][j] += m[i][k] * right.m[k][j];
            }
        }
        return result;
    }
    mat4 Inverse()
    {
        float m2[16];
        
        int k=0;
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                m2[k]=m[i][j];
                k++;
            }
        }
        
        double inv[16], det;
        int i;
        
        inv[0] = m2[5]  * m2[10] * m2[15] -
        m2[5]  * m2[11] * m2[14] -
        m2[9]  * m2[6]  * m2[15] +
        m2[9]  * m2[7]  * m2[14] +
        m2[13] * m2[6]  * m2[11] -
        m2[13] * m2[7]  * m2[10];
        
        inv[4] = -m2[4]  * m2[10] * m2[15] +
        m2[4]  * m2[11] * m2[14] +
        m2[8]  * m2[6]  * m2[15] -
        m2[8]  * m2[7]  * m2[14] -
        m2[12] * m2[6]  * m2[11] +
        m2[12] * m2[7]  * m2[10];
        
        inv[8] = m2[4]  * m2[9] * m2[15] -
        m2[4]  * m2[11] * m2[13] -
        m2[8]  * m2[5] * m2[15] +
        m2[8]  * m2[7] * m2[13] +
        m2[12] * m2[5] * m2[11] -
        m2[12] * m2[7] * m2[9];
        
        inv[12] = -m2[4]  * m2[9] * m2[14] +
        m2[4]  * m2[10] * m2[13] +
        m2[8]  * m2[5] * m2[14] -
        m2[8]  * m2[6] * m2[13] -
        m2[12] * m2[5] * m2[10] +
        m2[12] * m2[6] * m2[9];
        
        inv[1] = -m2[1]  * m2[10] * m2[15] +
        m2[1]  * m2[11] * m2[14] +
        m2[9]  * m2[2] * m2[15] -
        m2[9]  * m2[3] * m2[14] -
        m2[13] * m2[2] * m2[11] +
        m2[13] * m2[3] * m2[10];
        
        inv[5] = m2[0]  * m2[10] * m2[15] -
        m2[0]  * m2[11] * m2[14] -
        m2[8]  * m2[2] * m2[15] +
        m2[8]  * m2[3] * m2[14] +
        m2[12] * m2[2] * m2[11] -
        m2[12] * m2[3] * m2[10];
        
        inv[9] = -m2[0]  * m2[9] * m2[15] +
        m2[0]  * m2[11] * m2[13] +
        m2[8]  * m2[1] * m2[15] -
        m2[8]  * m2[3] * m2[13] -
        m2[12] * m2[1] * m2[11] +
        m2[12] * m2[3] * m2[9];
        
        inv[13] = m2[0]  * m2[9] * m2[14] -
        m2[0]  * m2[10] * m2[13] -
        m2[8]  * m2[1] * m2[14] +
        m2[8]  * m2[2] * m2[13] +
        m2[12] * m2[1] * m2[10] -
        m2[12] * m2[2] * m2[9];
        
        inv[2] = m2[1]  * m2[6] * m2[15] -
        m2[1]  * m2[7] * m2[14] -
        m2[5]  * m2[2] * m2[15] +
        m2[5]  * m2[3] * m2[14] +
        m2[13] * m2[2] * m2[7] -
        m2[13] * m2[3] * m2[6];
        
        inv[6] = -m2[0]  * m2[6] * m2[15] +
        m2[0]  * m2[7] * m2[14] +
        m2[4]  * m2[2] * m2[15] -
        m2[4]  * m2[3] * m2[14] -
        m2[12] * m2[2] * m2[7] +
        m2[12] * m2[3] * m2[6];
        
        inv[10] = m2[0]  * m2[5] * m2[15] -
        m2[0]  * m2[7] * m2[13] -
        m2[4]  * m2[1] * m2[15] +
        m2[4]  * m2[3] * m2[13] +
        m2[12] * m2[1] * m2[7] -
        m2[12] * m2[3] * m2[5];
        
        inv[14] = -m2[0]  * m2[5] * m2[14] +
        m2[0]  * m2[6] * m2[13] +
        m2[4]  * m2[1] * m2[14] -
        m2[4]  * m2[2] * m2[13] -
        m2[12] * m2[1] * m2[6] +
        m2[12] * m2[2] * m2[5];
        
        inv[3] = -m2[1] * m2[6] * m2[11] +
        m2[1] * m2[7] * m2[10] +
        m2[5] * m2[2] * m2[11] -
        m2[5] * m2[3] * m2[10] -
        m2[9] * m2[2] * m2[7] +
        m2[9] * m2[3] * m2[6];
        
        inv[7] = m2[0] * m2[6] * m2[11] -
        m2[0] * m2[7] * m2[10] -
        m2[4] * m2[2] * m2[11] +
        m2[4] * m2[3] * m2[10] +
        m2[8] * m2[2] * m2[7] -
        m2[8] * m2[3] * m2[6];
        
        inv[11] = -m2[0] * m2[5] * m2[11] +
        m2[0] * m2[7] * m2[9] +
        m2[4] * m2[1] * m2[11] -
        m2[4] * m2[3] * m2[9] -
        m2[8] * m2[1] * m2[7] +
        m2[8] * m2[3] * m2[5];
        
        inv[15] = m2[0] * m2[5] * m2[10] -
        m2[0] * m2[6] * m2[9] -
        m2[4] * m2[1] * m2[10] +
        m2[4] * m2[2] * m2[9] +
        m2[8] * m2[1] * m2[6] -
        m2[8] * m2[2] * m2[5];
        
        det = m2[0] * inv[0] + m2[1] * inv[4] + m2[2] * inv[8] + m2[3] * inv[12];
        
        
        det = 1.0 / det;
        double invOut[16];
        for (i = 0; i < 16; i++)
            invOut[i] = inv[i] * det;
        
        mat4 answer2=
        mat4((float)invOut[0],(float)invOut[1],(float)invOut[2],(float)invOut[3],
             (float)invOut[4],(float)invOut[5],(float)invOut[6],(float)invOut[7],
             (float)invOut[8],(float)invOut[9],(float)invOut[10],(float)invOut[11],
             (float)invOut[12],(float)invOut[13],(float)invOut[14],(float)invOut[15]);
        
        
        return answer2;
    }
    operator float*() { return &m[0][0]; }
};


// 3D point in homogeneous coordinates
struct vec4
{
    float v[4];
    
    vec4(float x = 0, float y = 0, float z = 0, float w = 1)
    {
        v[0] = x; v[1] = y; v[2] = z; v[3] = w;
    }
    
    vec4 operator*(const mat4& mat)
    {
        vec4 result;
        for (int j = 0; j < 4; j++)
        {
            result.v[j] = 0;
            for (int i = 0; i < 4; i++) result.v[j] += v[i] * mat.m[i][j];
        }
        return result;
    }
    
    vec4 operator+(const vec4& vec)
    {
        vec4 result(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2], v[3] + vec.v[3]);
        return result;
    }
};

// 2D point in Cartesian coordinates
struct vec2
{
    float x, y;
    
    vec2(float x = 0.0, float y = 0.0) : x(x), y(y) {}
    
    vec2 operator+(const vec2& v)
    {
        return vec2(x + v.x, y + v.y);
    }
    
    vec2 operator*(float s)
    {
        return vec2(x * s, y * s);
    }
};
class Shader
{
protected:
    unsigned int shaderProgram;
    
public:
    void CompileProgram(const char *vertexSource, const char *fragmentSource)
    {
        // create vertex shader from string
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if (!vertexShader) { printf("Error in vertex shader creation\n"); exit(1); }
        
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);
        checkShader(vertexShader, "Vertex shader error");
        
        // create fragment shader from string
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (!fragmentShader) { printf("Error in fragment shader creation\n"); exit(1); }
        
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);
        checkShader(fragmentShader, "Fragment shader error");
        
        // attach shaders to a single program
        shaderProgram = glCreateProgram();
        if (!shaderProgram) { printf("Error in shader program creation\n"); exit(1); }
        
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        
    }
    void LinkProgram()
    {
        // program packaging
        glLinkProgram(shaderProgram);
        checkLinking(shaderProgram);
    }
    void Run()
    {
        glUseProgram(shaderProgram);
    }
    ~Shader()
    {
        glDeleteProgram(shaderProgram);
    }
    
    virtual void UploadColor(vec4& color)
    {
        
    }
    void UploadM(mat4& M)
    {
        int location = glGetUniformLocation(shaderProgram, "M");
        if (location >= 0) glUniformMatrix4fv(location, 1, GL_TRUE, M);
        else
        {
            printf("uniform M cannot be set\n");
            printf("Failure at Shader\n");
        }
    }
    virtual void UploadTime(){};
    
};



class Geometry
{
protected: unsigned int vao;
public:
    Geometry()
    {
        glGenVertexArrays(1, &vao);    // create a vertex array object
    }
    
    virtual void Draw() = 0;
    
    
};

class Material
{
    Shader* shader;
    vec4 color;
    
public:
    Material(Shader *s, vec4 c)
    {
        shader=s;
        color=c;
    }
    
    void UploadAttributes()
    {
        shader->UploadColor(color);
        shader->UploadTime();
        
    }
    Shader* GetShader()
    {
        return shader;
    }
    
    
    
};


class Mesh
{
    Material* material;
    Geometry *geometry;
public:
    Mesh(Material* m, Geometry *g)
    {
        material=m;
        geometry=g;
    }
    void Draw()
    {
        material->UploadAttributes();
        geometry->Draw();
    }
    
    
    Shader* GetShader()
    {
        return material->GetShader();
    }
};
class Camera
{
    vec2 center;
    vec2 halfSize;
    
public:
    float angle=0;
    Camera()
    {

        center = vec2(0.45, 0.45);
        halfSize =  vec2(0.5, 0.5);
    }
    
    mat4 GetViewTransformationMatrix()
    {
        mat4 T = mat4(
                      1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      -center.x, -center.y, 0.0, 1.0);
        
        mat4 S = mat4(
                      1.0 / halfSize.x, 0.0, 0.0, 0.0,
                      0.0, 1.0 / halfSize.y, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
        
        mat4 R=mat4(
                    cos(angle), sin(angle),0,0,
                    -sin(angle),cos(angle),0,0,
                    0,0,1,0,
                    0,0,0,1);
        
        return T * S*R;
    }
    mat4 GetInverseViewTransformationMatrix()
    {
        mat4 T = mat4(
                      1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      -center.x, -center.y, 0.0, 1.0);
        
        mat4 S = mat4(
                      1.0 / halfSize.x, 0.0, 0.0, 0.0,
                      0.0, 1.0 / halfSize.y, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
        mat4 R=mat4(
                    cos(angle), sin(angle),0,0,
                    -sin(angle),cos(angle),0,0,
                    0,0,1,0,
                    0,0,0,1);
        mat4 temp=T * S*R;
        
        
        return temp.Inverse();
        
    }
    
    void SetAspectRatio(int width, int height)
    {
        halfSize = vec2((float)width / height,1.0)*0.5;
    }
    
    void Move(float timeT)
    {
        
        
        if(keyboardState['w']) center = center + vec2(0.0, 1.0) * timeT;
        if(keyboardState['s']) center = center + vec2(0.0, -1.0) * timeT;
        
    }
    void Quake(float timeT)
    {
        if(keyboardState['q'])
        {
            

            angle=angle+sin(timeT)*100;
            
            
        }
        
        //Reset Angle of Camera
        if(keyboardState['r'])
        {
            angle=0;
        }
    }
    //Turn the tables
    void TurnTables(float timeT)
    {
        if(keyboardState['d']) angle = angle=angle+timeT*5;
        if(keyboardState['a']) angle = angle=angle-timeT*5;
    }
    
};

Camera camera;
class Object
{
    
    Shader *shader;
    Mesh *mesh;
    
    
    
    float orientation;
    
public:
    int gemTypeId;
    vec2 position;
    vec2 targetPosition;
    vec2 scaling;
    vec2 targetScale;
    bool removing=false;
public:
    Object(Mesh* m, vec2 p=vec2(0.0,0.0),vec2 s=vec2(1.0,1.0),int g=1,float o =0.0)
    {
        
        mesh =m;
        position=p;
        targetPosition=p;
        scaling=s;
        orientation=o;
        gemTypeId=g;
        targetScale=s;
        
        shader=mesh->GetShader();
    }
    void UploadAttributes()
    {
        // calculate T, S, R from position, scaling, and orientation
        mat4 T = mat4(
                      1.0, 0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      position.x, position.y, 0.0, 1.0);
        
        mat4 S = mat4(
                      scaling.x, 0.0, 0.0, 0.0,
                      0.0, scaling.y, 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
        
        float alpha = orientation / 180.0 * M_PI;
        
        mat4 R = mat4(
                      cos(alpha), sin(alpha), 0.0, 0.0,
                      -sin(alpha), cos(alpha), 0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);
        
        mat4 V = camera.GetViewTransformationMatrix();
        
        mat4 M = S * R * T * V;
        
        shader->UploadM(M);
    }
    void Draw()
    {
        shader->Run();
        UploadAttributes();
        mesh->Draw();
        
        
    }
    void Move(float dt)
    {
        if(removing)
        {
            //Dramatic Exit
            //Shrink and Rotate if removing
            orientation =orientation  + 100 * dt;
            scaling.x=scaling.x/1.05;
            scaling.y=scaling.y/1.05;
       
            gemTypeId=100;
        }
        else
        {
            //Otherwise Move to targey position
            MoveTarget(dt);
        }
        
        
    }
    void PrintPosition()
    {
        //std::cout<<"This is the new position: ("<<position.x<<","<<position.y<<")"<<std::endl;
    }
    
    //Moves Object to target
    void MoveTarget(float dt)
    {
        
        if(position.x!=targetPosition.x)
        {
            if(abs(position.x-targetPosition.x)<0.01)
            {
                position.x=targetPosition.x;
            }
            else
            {
                position.x=position.x+(targetPosition.x-position.x)*10*dt;
            }
            
        }
        if(position.y!=targetPosition.y)
        {
            if(abs(position.y-targetPosition.y)<0.01)
            {
                position.y=targetPosition.y;
            }
            else
            {
                position.y=position.y+(targetPosition.y-position.y)*10*dt;
            }
        }
        
    }

    void remove()
    {
        removing=true;
    }
    bool isActive()
    {
        
        return ((position.x==targetPosition.x)&&(position.y==targetPosition.y));
    }
    bool isDead()
    {
        return scaling.x<0.0001;
    }
    void rotate()
    {
        orientation=orientation+5;
    }
    
    
};

class Quad : public Geometry
{
    unsigned int vbo;
    
public:
    Quad()
    {
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        static float vertexCoords[] = { 0, 0, 1, 0, 0, 1, 1, 1};
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    
    void Draw()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
};

class Triangle : public Geometry
{
    unsigned int vbo;
    
public:
    Triangle()
    {
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        static float vertexCoords[] = { 0, 0, 1, 0, 0, 1 };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords),
                     vertexCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    
    void Draw()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    
};

//Stellar
class Star : public Geometry
{
    unsigned int vbo;
    
public:
    Star()
    {
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        int r=1;
        int R=2;
        static float vertexCoords[] = {
            (float)(R*cos(18/ 180.0 * M_PI)),(float)(R*sin(18/ 180.0 * M_PI)),
            (float)(r*cos(342/ 180.0 * M_PI)),(float)(r*sin(324/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(18/ 180.0 * M_PI)),(float)(R*sin(18/ 180.0 * M_PI)),
            (float)(r*cos(54/ 180.0 * M_PI)),(float)(r*sin(54/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(90/ 180.0 * M_PI)),(float)(R*sin(90/ 180.0 * M_PI)),
            (float)(r*cos(54/ 180.0 * M_PI)),(float)(r*sin(54/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(90/ 180.0 * M_PI)),(float)(R*sin(90/ 180.0 * M_PI)),
            (float)(r*cos(126/ 180.0 * M_PI)),(float)(r*sin(126/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(162/ 180.0 * M_PI)),(float)(R*sin(162/ 180.0 * M_PI)),
            (float)(r*cos(126/ 180.0 * M_PI)),(float)(r*sin(126/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(162/ 180.0 * M_PI)),(float)(R*sin(162/ 180.0 * M_PI)),
            (float)(r*cos(198/ 180.0 * M_PI)),(float)(r*sin(198/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(234/ 180.0 * M_PI)),(float)(R*sin(234/ 180.0 * M_PI)),
            (float)(r*cos(198/ 180.0 * M_PI)),(float)(r*sin(198/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(234/ 180.0 * M_PI)),(float)(R*sin(234/ 180.0 * M_PI)),
            (float)(r*cos(270/ 180.0 * M_PI)),(float)(r*sin(270/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(306/ 180.0 * M_PI)),(float)(R*sin(306/ 180.0 * M_PI)),
            (float)(r*cos(270/ 180.0 * M_PI)),(float)(r*sin(270/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            (float)(R*cos(306/ 180.0 * M_PI)),(float)(R*sin(306/ 180.0 * M_PI)),
            (float)(r*cos(342/ 180.0 * M_PI)),(float)(r*sin(342/ 180.0 * M_PI)),
            (float)0,(float)0,
            
            
            
            
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords),
                     vertexCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    
    void Draw()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 30);
    }
    
    
};
class Hexagon : public Geometry
{
    unsigned int vbo;
    
public:
    Hexagon()
    {
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        static float vertexCoords[] = {
            (float)cos(60/ 180.0 * M_PI),(float)sin(60/ 180.0 * M_PI),
            (float)cos(120/ 180.0 * M_PI),(float)sin(120/ 180.0 * M_PI),
            (float)cos(180/ 180.0 * M_PI),(float)sin(180/ 180.0 * M_PI),
            (float)cos(240/ 180.0 * M_PI),(float)sin(240/ 180.0 * M_PI),
            (float)cos(300/ 180.0 * M_PI),(float)sin(300/ 180.0 * M_PI),
            (float)cos(360/ 180.0 * M_PI),(float)sin(360/ 180.0 * M_PI)
            
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords),
                     vertexCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    
    void Draw()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_LOOP, 0, 6 );
    }
    
};

//Lovely
class Lovely : public Geometry
{
    unsigned int vbo;
    
public:
    Lovely()
    {
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        static float vertexCoords[720];
        float t=0;
        for(int i=0;i<720;i+=2)
        {
            vertexCoords[i]=16*sin(t)*sin(t)*sin(t);
            vertexCoords[i+1]=13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t);
            t=t+(1/ 180.0 * M_PI);
            
            
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords),
                     vertexCoords, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    
    void Draw()
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 360 );
    }
    void Move()
    {
        
    }
};

class StandardShader : public Shader
{
    
    
public:
    StandardShader()
    {
        // vertex shader in GLSL
        const char *vertexSource = "\n\
        #version 410 \n\
        precision highp float; \n\
        in vec2 vertexPosition;    \n\
        out vec3 color; \n\
        uniform mat4 M; \n\
        uniform vec3 vertexColor; \n\
        void main() \n\
        { \n\
        color = vertexColor; \n\
        gl_Position = vec4(vertexPosition.x, \n\
        vertexPosition.y, 0, 1) * M; \n\
        } \n\
        ";
        
        // fragment shader in GLSL
        const char *fragmentSource = "\n\
        #version 410 \n\
        precision highp float; \n\
        \n\
        in vec3 color;            // variable input: interpolated from the vertex colors \n\
        out vec4 fragmentColor;        // output that goes to the raster memory as told by glBindFragDataLocation \n\
        \n\
        void main() \n\
        { \n\
        fragmentColor = vec4(color, 1); // extend RGB to RGBA \n\
        } \n\
        ";
        CompileProgram(vertexSource, fragmentSource);
        
        
        // connect Attrib Array to input variables of the vertex shader
        glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0
        
        // connect the fragmentColor to the frame buffer memory
        glBindFragDataLocation(shaderProgram, 0, "fragmentColor"); // fragmentColor goes to the frame buffer memory
        LinkProgram();
        
        
    }
    
    void UploadColor(vec4& color)
    {
        int location = glGetUniformLocation(shaderProgram, "vertexColor");
        if (location >= 0) glUniform3fv(location, 1, &color.v[0]);
        else{
            printf("uniform vertexColor cannot be set\n");
            printf("Failure at StandardShader\n");
        }
        
    }

    
    
};

//Heartbeat
class PulseShader : public Shader
{
    
    
public:
    PulseShader()
    {
        // vertex shader in GLSL
        const char *vertexSource = "\n\
        #version 410 \n\
        precision highp float; \n\
        in vec2 vertexPosition;    \n\
        out vec3 color; \n\
        uniform mat4 M; \n\
        uniform vec3 vertexColor; \n\
        uniform float time; \n\
        void main() \n\
        { \n\
        color = vertexColor*(1-abs(0.8*sin(time))); \n\
        gl_Position = vec4(vertexPosition.x, \n\
        vertexPosition.y, 0, 1) * M; \n\
        } \n\
        ";
        
        // fragment shader in GLSL
        const char *fragmentSource = "\n\
        #version 410 \n\
        precision highp float; \n\
        \n\
        in vec3 color;            // variable input: interpolated from the vertex colors \n\
        out vec4 fragmentColor;        // output that goes to the raster memory as told by glBindFragDataLocation \n\
        \n\
        void main() \n\
        { \n\
        fragmentColor = vec4(color, 1); // extend RGB to RGBA \n\
        } \n\
        ";
        CompileProgram(vertexSource, fragmentSource);
        
        
        // connect Attrib Array to input variables of the vertex shader
        glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0
        
        // connect the fragmentColor to the frame buffer memory
        glBindFragDataLocation(shaderProgram, 0, "fragmentColor"); // fragmentColor goes to the frame buffer memory
        
        LinkProgram();
        
        
    }
    
    void UploadColor(vec4& color)
    {
        int location = glGetUniformLocation(shaderProgram, "vertexColor");
        if (location >= 0) glUniform3fv(location, 1, &color.v[0]);
        else
        {
            printf("uniform vertexColor cannot be set\n");
            printf("Failure at PulseShader\n");
        }
        
    }
    
    

    void UploadTime()
    {
        int location = glGetUniformLocation(shaderProgram, "time");
        if (location >= 0) glUniform1f(location,lastTime);
        else
        {
            printf("uniform vertexColor cannot be set\n");
            printf("Failure at Time\n");
        }
    }
    
    
};
int numObjectsInRow=10;
int numObjectsInColumn=10;

class Scene {
    Shader* shader;
    Shader* pulseShader;
    std::vector<Material*> materials;
    std::vector<Geometry*> geometries;
    std::vector<Mesh*> meshes;
    std::vector<Object*> objectsToClear;
    
public:
    
    std::vector<std::vector<Object*>> twoDObjects;
    
    int xSelected=-1;
    int ySelected;
    float xMouse;
    float yMouse;
public:
    Scene() { shader = 0; pulseShader=0;}
    void Initialize() {
        shader = new StandardShader();
        pulseShader=new PulseShader();
        // build the scene here
        
        
        materials.push_back(new Material(pulseShader, vec4(1, 0, 0,1)));
        materials.push_back(new Material(shader, vec4(0, 1, 0,1)));
        materials.push_back(new Material(shader, vec4(1, 1, 0,1)));
        materials.push_back(new Material(shader, vec4(0, 0, 1,1)));
        materials.push_back(new Material(shader, vec4(1, 0, 1,1)));
        materials.push_back(new Material(shader, vec4(0, 1, 1,1)));
        materials.push_back(new Material(shader, vec4(1, 1, 1,1)));
        
        geometries.push_back(new Lovely());
        geometries.push_back(new Lovely());
        geometries.push_back(new Star());
        geometries.push_back(new Star());
        geometries.push_back(new Hexagon());
        geometries.push_back(new Hexagon());
        geometries.push_back(new Hexagon());
        
        for(int i=0;i<7;i++)
        {
            meshes.push_back(new Mesh(materials[i],geometries[i]));
        }
        
        // ID's:
        //0: Red Heart
        //1: Green Heart
        //2: Yellow Star
        //3: Blue Star
        //4: Pink Hexagon
        //5: Blue Hexagon
        //6: White Hexagon
        srand(time(NULL));
        float positionX=0;
        float positionY=0;
        
        
        for(int i=0;i<numObjectsInRow;i++)
        {
            positionY=0;
            twoDObjects.push_back(std::vector<Object*>());
            for(int j=0;j<numObjectsInColumn;j++)
            {
                
                int id= (rand() % 7) ;
                //                int id=0;
                if(id==0)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.0025, 0.0025),id));
                }
                else if(id==1)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.0025, 0.0025),id));
                }
                else if(id==2)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.025, 0.025),id));
                }
                else if(id==3)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.025, 0.025),id));
                }
                else if(id==4)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.03, 0.03),id));
                }
                else if(id==5)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.03, 0.03),id));
                }
                else if(id==6)
                {
                    twoDObjects[i].push_back(new Object(meshes[id], vec2(positionX, positionY), vec2(0.03, 0.03),id));
                }
                positionY=positionY+0.1;
                
                
            }
            positionX=positionX+0.1;
        }
        
        
        
        
        
        //        shader->Run();
    }
    ~Scene() {

        for (int i=0;i<materials.size() ; i++)
        {
            delete materials[i];
            delete geometries[i];
            delete meshes[i];
            
        }
        for(int i=0;i<numObjectsInColumn;i++)
        {
            for(int j=0;j<numObjectsInRow;j++)
            {
                
                delete twoDObjects[i][j];
                
                
            }
        }
        if(shader) delete shader ;
        if(pulseShader) delete pulseShader;
        
    }
    void Bomb(int i,int j)
    {
        
        // twoDObjects[i].at(j)->remove();
        objectsToClear.push_back(twoDObjects[i][j]);
    }
    void Move()
    {
        // time elapsed since program started, in seconds
        double t = glutGet(GLUT_ELAPSED_TIME) * 0.001;
        // variable to remember last time idle was called
        lastTime = 0.0;
        static double cameraLastTime = 0.0;
        // time difference between calls: time step
        dt = t - lastTime;
        double cameraDT=t - cameraLastTime;
        
        // store time
        lastTime = t;
        cameraLastTime=t;
        camera.Move(cameraDT);
        camera.Quake(dt);
        camera.TurnTables(cameraDT);
        
        //Gyro
        //Rotate Stars
        for(int i=0;i<numObjectsInRow;i++)
        {
            for(int j=0;j<numObjectsInColumn;j++)
            {
                if(twoDObjects[i][j])
                {
                    if(twoDObjects[i][j]->gemTypeId==2)
                    {
                        twoDObjects[i][j]->rotate();
                    }
                    
                }
                
                
            }
        }
        
        //Quake
        if(keyboardState['q'])
        {
            std::srand(std::time(0));
            //Quake 0.1% of disappearing
            for(int i=0;i<numObjectsInRow;i++)
            {
                for(int j=0;j<numObjectsInColumn;j++)
                {
                    // needed once per program run
                    int r = std::rand() % 1000 + 1;
                    
                    //                    std::cout<<r<<std::endl;
                    if(r==2)
                    {
                        objectsToClear.push_back(twoDObjects[i][j]);
                    }
                    
                }
            }
        }
        
        //Move towards target position
        for(int i=0;i<numObjectsInRow;i++)
        {
            for(int j=0;j<numObjectsInColumn;j++)
            {
                if(twoDObjects[i][j])
                {
                    twoDObjects[i].at(j)->Move(cameraDT);
                    
                    if(!twoDObjects[i][j]->isActive())
                    {
                        //std::cout<<"test2"<<std::endl;
                    }
                    
                    //                checkThree(i,j);
                }
                
                
            }
        }
        //Check 3 in a row
        for(int i=0;i<numObjectsInRow;i++)
        {
            for(int j=0;j<numObjectsInColumn;j++)
            {
                if(twoDObjects[i][j])
                {
                    checkThree(i,j);
                }
            }
        }
        //Clear Objects if needed
        for(int i=0;i<objectsToClear.size();i++)
        {
            if(objectsToClear[i]->isActive())
            {
                
                objectsToClear[i]->remove();
            }
            
            
        }
        //Drag Object with mouse
        if(xSelected>=0)
        {
            if(twoDObjects[xSelected][ySelected])
            {
                twoDObjects[xSelected][ySelected]->position=vec2(xMouse,yMouse);
            }
        }
        //Delete Removed Objects
        for(int i=0;i<numObjectsInRow;i++)
        {
            for(int j=0;j<numObjectsInColumn;j++)
            {
                if(twoDObjects[i][j])
                {
                    if(twoDObjects[i][j]->isDead())
                    {
                        //delete twoDObjects[i][j];
                        // twoDObjects[i][j]=nullptr;
                        
                        
                        //                        int k=0;
                        //
                        //                        while(twoDObjects[i][j+k]->isDead())
                        //                        {
                        //                            k++;
                        //                            if(j+k>9)
                        //                            {
                        //                                break;
                        //                            }
                        //
                        //
                        //                        }
                        //                        //First non-dead gem above is at i,j+k
                        //                        int q=0;
                        //
                        //                            while(twoDObjects[i][j-q]->isDead())
                        //                            {
                        //                                q++;
                        //                                if(j-q<0)
                        //                                {
                        //                                    break;
                        //                                }
                        //
                        //                            }
                        //
                        //                        //Last Dead gem is at i,j-(q-1)
                        //
                        //
                        //
                        //                        //Swap pointers As well
                        //                        for(int h=0;h+j+k<10;h++)
                        //                        {
                        //                           // twoDObjects[i][j+k+h]->targetPosition=twoDObjects[i][j-(q-1)+h]->position;
                        //                         //   std::swap(twoDObjects[i][j+k+h],twoDObjects[i][j-(q-1)+h]);
                        //                        }
                        //
                        
                    }
                }
                
            }
        }
        
        //Skyfall
        //        for(int i=0;i<numObjectsInRow;i++)
        //        {
        //            for(int j=0;j<numObjectsInColumn;j++)
        //            {
        //                if(!twoDObjects[i][j])
        //                {
        //                    for(int k=j+1;k<=9;k++)
        //                    {
        //                        Object* temp=new Object(meshes[0], vec2(0.1, 0.1), vec2(0.003, 0.003),0);
        //                        twoDObjects[i][k]->targetPosition=twoDObjects[i][k-1]->targetPosition;
        //                    }
        //                }
        //
        //            }
        //        }
        
        
        
        
        //    std::cout<<clearObjectsHorizontal.size()<<std::endl;
    }
    void Draw()
    {
        //for(int i = 0; i < objects.size(); i++) objects[i]->Draw();
        for (int i=0;i<numObjectsInRow ; i++)
        {
            for(int j=0;j<numObjectsInColumn;j++)
            {
                if(twoDObjects[i][j])
                {
                    twoDObjects[i].at(j)->Draw();
                }
            }
            
        }
        

    }
    void Select(int u, int v)
    {
        xSelected=u;
        ySelected=v;
        
    }
    //Swap
    void Swap(int u, int v)
    {
        bool allowed=checkSwap(u,v);
        if(allowed&&twoDObjects[u][v]&&twoDObjects[xSelected][ySelected])
            
        {
            std::cout<<"Selected: ("<<xSelected<<","<<ySelected<<")"<<std::endl;
            
            std::swap(twoDObjects[u][v],twoDObjects[xSelected][ySelected]);
            vec2 temp=twoDObjects[u][v]->targetPosition;
            
            twoDObjects[u][v]->targetPosition=twoDObjects[xSelected][ySelected]->targetPosition;
            
            twoDObjects[xSelected][ySelected]->targetPosition=temp;
            
            
            if(!(checkThree(xSelected,ySelected)||checkThree(u,v)))
            {
                std::swap(twoDObjects[u][v],twoDObjects[xSelected][ySelected]);
                vec2 temp=twoDObjects[u][v]->targetPosition;
                
                
                
                twoDObjects[u][v]->targetPosition=twoDObjects[xSelected][ySelected]->targetPosition;
                
                twoDObjects[xSelected][ySelected]->targetPosition=temp;
                
            }
            
            
        }
        //Keeps track of whether something is held
        xSelected=-1;
    }

    
    //Legal and Three-in-a-line
    bool checkThree(int x1, int y1)
    {
        std::vector<Object*> clearObjectsHorizontal;
        std::vector<Object*> clearObjectsVertical;
        bool success=false;
        
        int k=1;

        while(x1+k<=9)
        {
            if(!twoDObjects[x1+k][y1])
            {
                break;
            }
            
            if(! (twoDObjects[x1].at(y1)->gemTypeId==twoDObjects[x1+k].at(y1)->gemTypeId))
            {
                break;
            }
            
            clearObjectsHorizontal.push_back(twoDObjects[x1+k].at(y1));
            
            
            k++;
            
        }
        
        k=1;
        //Checking left
        while(x1-k>=0)
        {
            if(!twoDObjects[x1-k][y1])
            {
                break;
            }
            if(!(twoDObjects[x1].at(y1)->gemTypeId==twoDObjects[x1-k].at(y1)->gemTypeId))
            {
                break;
            }
            
            clearObjectsHorizontal.push_back(twoDObjects[x1-k].at(y1));
            
            
            k++;
            
            
        }
        
        k=1;
        //Checking Up
        while(y1+k<=9)
        {
            if(!twoDObjects[x1][y1+k])
            {
                break;
            }
            if(!(twoDObjects[x1].at(y1)->gemTypeId==twoDObjects[x1].at(y1+k)->gemTypeId))
            {
                break;
            }
            clearObjectsVertical.push_back(twoDObjects[x1].at(y1+k));
            
            
            k++;
            
        }
        
        k=1;
        //Checking Down
        while(y1-k>=0)
        {
            if(!twoDObjects[x1][y1-k])
            {
                break;
            }
            if( !(twoDObjects[x1].at(y1)->gemTypeId==twoDObjects[x1].at(y1-k)->gemTypeId))
            {
                break;
            }
          
            clearObjectsVertical.push_back(twoDObjects[x1].at(y1-k));
            
            
            k++;
            
        }
        
        if(clearObjectsHorizontal.size()>=2)
        {
            if(twoDObjects[x1][y1]->isActive())
            {
                
                clearObjectsHorizontal.push_back(twoDObjects[x1].at(y1));
                for(int i=0;i<clearObjectsHorizontal.size();i++)
                {
                    objectsToClear.push_back(clearObjectsHorizontal[i]);
                }
           
            }
     
            success=true;
        }
        if(clearObjectsVertical.size()>=2)
        {
            if(twoDObjects[x1][y1]->isActive())
            {
              
                clearObjectsVertical.push_back(twoDObjects[x1].at(y1));
                for(int i=0;i<clearObjectsVertical.size();i++)
                {
                    objectsToClear.push_back(clearObjectsVertical[i]);
                }
            }

            success=true;
            
        }
        
        
        return success;

    }

    bool checkSwap(int u, int v )
    {
        
        bool allowed=checkDistance(u,v);
        
        return allowed;
        
    }
    bool checkDistance(int u, int v)
    {
        
        bool allowed=false;
        if((abs(u-xSelected)==1&&abs(v-ySelected)==0)||(abs(u-xSelected)==0&&abs(v-ySelected)==1))
        {
            allowed=true;
        }
        return allowed;
    }
    

    
};
Scene scene;




void onInitialization()
{
    for(int i = 0; i < 256; i++) keyboardState[i] = false;
    
    glViewport(0, 0, windowWidth, windowHeight);
    scene.Initialize();
    
}



void onExit()
{
    //glDeleteProgram(shaderProgram);
    
    printf("exit");
}

// window has become invalid: redraw
void onDisplay()
{
    
    glClearColor(0, 0, 0, 0); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
    scene.Draw();
    glutSwapBuffers(); // exchange the two buffers
    
}

void onKeyboard(unsigned char key, int x, int y)
{
    keyboardState[key] = true;
}

void onKeyboardUp(unsigned char key, int x, int y)
{
    keyboardState[key] = false;
}
void onMouse(int button, int state, int i, int j)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    float x=((float)i /viewport[2])*2.0 -1.0;
    float y=1.0-((float)j /viewport[3])*2.0;

    mat4 invV=camera.GetInverseViewTransformationMatrix();
    
    vec4 p=vec4(x,y,0,1)*invV;
    
    
    int u=(int)floor((p.v[0]+0.5)*10 -4.5);
    int v=(int)floor((p.v[1]+0.5)*10 -4.5);
    
    
    if((u<0)||(v>9)||(v<0)||(v>9)) return;
    if(button==GLUT_LEFT_BUTTON)
    {
        if(state==GLUT_DOWN)
        {
            scene.Select(u,v);
            
        }
        if(state==GLUT_UP)
        {
            scene.Swap(u,v);
            
        }
        
    }
    else if(button==GLUT_RIGHT_BUTTON)
    {
        if(state==GLUT_DOWN)
        {
            scene.Bomb(u,v);
            
            
        }
    }
    
    
}

//Sticky
void onMouseMove(int i, int j)
{
    
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    scene.xMouse=((float)i /viewport[2])*1 -0.05;
    scene.yMouse=1-((float)j /viewport[3])*1.0 -0.05;
    
}

void onReshape(int winWidth0, int winHeight0)
{
    camera.SetAspectRatio(winWidth0, winHeight0);
    glViewport(0, 0, winWidth0, winHeight0);
}

void onIdle( )
{

    scene.Move();

    glutPostRedisplay();
}

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);
#if !defined(__APPLE__)
    glutInitContextVersion(majorVersion, minorVersion);
#endif
    glutInitWindowSize(windowWidth, windowHeight);     // application window is initially of resolution 512x512
    glutInitWindowPosition(50, 50);            // relative location of the application window
#if defined(__APPLE__)
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);  // 8 bit R,G,B,A + double buffer + depth buffer
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutCreateWindow("Triangle Rendering");
    
#if !defined(__APPLE__)
    glewExperimental = true;
    glewInit();
#endif
    printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
    printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
    printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
    printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    onInitialization();
    
    glutDisplayFunc(onDisplay); // register event handlers
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutReshapeFunc(onReshape);
    glutMouseFunc(onMouse);
    glutPassiveMotionFunc(onMouseMove);
    glutMotionFunc(onMouseMove);
    
    glutMainLoop();
    onExit();
    return 1;
}





