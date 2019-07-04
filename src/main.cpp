/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"

#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

struct F{
    float t;
    float c;
    float h;
    float bb;
    F(float a, float b, float d, float p)
    {
        t = a;
        c = b;
        h = d;
        bb = p;
    }
};
int ind = 0;
int ind2 = 2;

int ind3 = 9;
int ind4 = 11;

vector<F> keyframes{
    F(-0.6,0,0.4,0),    //1
    F(-1.4,0,1.4,0),    //2
    F(-1.6,0,1.4,0),    //3
    F(-1.8,0,0.8,0),    //4
    F(-2,0,0.5,0),      //5
    F(-2,-1.2,0.4,0),  //6
    F(-2,-1.6,0,0),  //7
    F(-1.6,-1.8,0.3,0), //8
    F(-1.2,-1.8,0.3,0), //9
    F(-1,-1.6,0.3,0),   //10
    F(-0.7,-1.4,0.5,0), //11
    F(-0.4,-1.2,0.5,0), //12
    F(-0.2,-0.9,0.5,0), //13
    F(-0.2,-0.6,0.9,0), //14
    F(-0.3,-0.4,0.9,0), //15
    F(-0.7,0,1.3,0),    //16
    F(-1,0,1.3,0)       //17
    
};

vector<F> keyframes2{
    F(-0.6,-1.5,2,-0.5),    //1
    F(-0.6,-1.2,1.9,-0.5),  //2
    F(-0.6,-1,1.7,-0.5),    //3
    F(-0.6,-7,1.3,-0.4),    //4
    F(-0.7,-0.4,1,-0.2),    //5
    F(-0.8,-0.4,0.7,-0.2),  //6
    F(-0.9,-0.4,0.8,0.2),   //7
    F(-1,-0.5,0.9,0.9),     //8
    F(-1.1,-0.7,0.7,0.9),   //9
    F(-1.3,-0.9,0.6,0.2),   //10
    F(-1.4,-1,0.5,-0.4),    //11
    F(-1.4,-1.2,0.6,-0.4),  //12
    F(-1.3,-1.5,0.8,-0.4),  //13
    F(-1.3,-1.6,1.3,-0.4),  //14
    F(-1.2,-1.9,1.8,-0.4),  //15
    F(-1,-1.8,1.9,-0.4),    //16
    F(-0.8,-1.7,1.9,-0.4)   //17
   
    
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
    std::shared_ptr<Program> prog2;
    std::shared_ptr<Program> cubeProg;

	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> mesh;
    vector<shared_ptr<Shape>> allShapes;
    
    shared_ptr<Texture> texture0;
    shared_ptr<Texture> texture1;
    shared_ptr<Texture> texture2;
    shared_ptr<Texture> texture3;
    shared_ptr<Texture> texture4;
    
    
    vector<std::string> faces {
        "/cracks/midnight-silence_rt.tga",
        "/cracks/midnight-silence_lf.tga",
        "/cracks/midnight-silence_up.tga",
        "/cracks/midnight-silence_dn.tga",
        "/cracks/midnight-silence_ft.tga",
        "/cracks/midnight-silence_bk.tga"
    };
    
	// Contains vertex information for OpenGL
	GLuint VertexArrayID;
//


    //keyframes.push_back(kf2);
	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;
    vec3 gMax;
    float dxmin;
    float dxmax;
    float dymin;
    float hxmin;
    float hxmax;
    float hymin;
    int sky;
    
    bool run = false;
    int renderCtr = 0;
    

	//animation data
	float sTheta = 0;
    float gTheta = 0;
    float changeMat = 0;
	float gTrans = 0;
    float sTrans = 0;
    float zTrans = 0;
    float rot;
    float lTrans = 0;
    
    bool init1 = false;
    
    float phi = 0;
    float theta = 0;
    
    double oldx = 0;
    double oldy = 0;
    
    vec3 horseO = glm::vec3(0,1,0);
    
    vec3 horsePos = glm::vec3(0,0,0);
    float radius =20;
    
    float rotH = 0;
    
    bool jumpSequence = false;
    float jumpHeight = 0.0;
    
    float cTrans = 0;
    //vec3 eye = glm::vec3(5,10,25);
    vec3 eye = glm::vec3(-055,20,25);
    
    vec3 velocity = glm::vec3(0,0,0);
    
    vec3 lookAt = glm::vec3(5,0,1);
    vec3 up = glm::vec3(0,1,0);
    
    vec3 gaze = lookAt - eye;
    vec3 w = -(glm::normalize(gaze));
    vec3 u = glm::normalize(glm::cross(up,w));
    vec3 v = glm::normalize(glm::cross(w,u));
    float speed = 1;
    
    
    void scrollCallback(GLFWwindow * window, double in_deltaX, double in_deltaY)
    {
        theta += in_deltaX * 0.2;
        phi += in_deltaY * 0.2;
        
        if(phi > 1.396)//80 degrees
        {
            phi = 1.396;
        }
        if(phi < -1.396)//80 degrees
        {
            phi = -1.396;
        }
        
//        if(theta > 1.396)//80 degrees
//        {
//            phi = 1.396;
//        }
//        if(theta < -1.396)//80 degrees
//        {
//            phi = -1.396;
//        }
//        cout << "phi " << phi <<  " theta " << theta <<endl;
//        cout << "dx " << in_deltaX <<  " dy " << in_deltaY <<endl;
        
        lookAt.x = (cos(phi) * cos(theta)) + eye.x;
        lookAt.y = sin(phi) + eye.y;
        lookAt.z = (cos(phi) * cos(1.57079632679 - theta)) + eye.z;
        //cout << "LAx " << lookAt.x <<  " LAy " << lookAt.y << "LAz " << lookAt.z <<endl;
        
   
        
        
        
    }
    
    unsigned int createSky(string dir, vector<string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false);
        for(GLuint i = 0; i < faces.size(); i++) {
            unsigned char *data =
            stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else {
                cout << "failed to load: " << (dir+faces[i]).c_str() << endl;
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        cout << " creating cube map any errors : " << glGetError() << endl;
        return textureID;
    }

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
        //cout << "rot: " << rotH << endl;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        
        
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            rotH +=0.5;

        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            rotH -=0.5;
        }
        
        if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
            run = true;
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotH, horseO);
            velocity = rot * glm::vec4(vec3(1,0,0), 0);
            horsePos.z = horsePos.z +1 * velocity.z;
            horsePos.x = horsePos.x +1 * velocity.x;

            
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
            run = false;
        }
        if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
            run = true;
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotH, horseO);
            velocity = rot * glm::vec4(vec3(1,0,0), 0);
            
            horsePos.z = horsePos.z -1 * velocity.z;
            horsePos.x = horsePos.x -1 * velocity.x;

        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
            //velocity.x = 0;
            run = false;
        }
        if (key == GLFW_KEY_M && action == GLFW_PRESS)
        {
            changeMat += 1;
            if(changeMat > 5)
            {
                changeMat = 0;
            }
        }
        if (key == GLFW_KEY_V && action == GLFW_PRESS)
        {
            if(ind3 == 16)
            {
                ind3 = 0;
            }
            ind3++;
            cout<< ind3 << endl;
        }
        if (key == GLFW_KEY_C && action == GLFW_PRESS)
        {
            if(ind3 == 0)
            {
                ind3 = 16;
            }
            ind3--;
            cout<< ind3  << endl;
        }
        if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            run = true;
        }
        if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        {
            run = false;
        }
        
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
            
            jumpSequence = true;
            
        }
        
	}
    
    void initTex(const std::string& resourceDirectory){
        texture0 = make_shared<Texture>();
        texture0->setFilename(resourceDirectory + "/cactusmat.jpg");
        texture0->init();
        texture0->setUnit(0);
        texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture1 = make_shared<Texture>();
        texture1->setFilename(resourceDirectory + "/grass.jpg");
        texture1->init();
        texture1->setUnit(2);
        texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture2 = make_shared<Texture>();
        texture2->setFilename(resourceDirectory + "/maple_bark.jpg");
        texture2->init();
        texture2->setUnit(2);
        texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture3 = make_shared<Texture>();
        texture3->setFilename(resourceDirectory + "/grass1.jpg");
        texture3->init();
        texture3->setUnit(2);
        texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        sky = createSky(resourceDirectory, faces);
//
    }
    
    void SetMaterial(int i) {
        switch (i) {
            case 0: //white rubber
                glUniform3f(prog->getUniform("matAmb"), 0.05, 0.05, 0.05);
                glUniform3f(prog->getUniform("matDif"), 0.5, 0.5, 0.5);
                glUniform3f(prog->getUniform("matSpec"), 0.7, 0.7, 0.7);
                glUniform1f(prog->getUniform("shine"), 0.78125);
                break;
            case 1: // flat grey
                glUniform3f(prog->getUniform("matAmb"), 0.13, 0.13, 0.14);
                glUniform3f(prog->getUniform("matDif"), 0.3, 0.3, 0.4);
                glUniform3f(prog->getUniform("matSpec"), 0.3, 0.3, 0.4);
                glUniform1f(prog->getUniform("shine"), 4.0);
                break;
            case 2: //brass
                glUniform3f(prog->getUniform("matAmb"), 0.3294, 0.2235, 0.02745);
                glUniform3f(prog->getUniform("matDif"), 0.7804, 0.5686, 0.11373);
                glUniform3f(prog->getUniform("matSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;
            case 3: //grass
                glUniform3f(prog->getUniform("matAmb"), 0, 0.05, 0);
                glUniform3f(prog->getUniform("matDif"), 0.4, 0.5, 0.4);
                glUniform3f(prog->getUniform("matSpec"), 0.04, 0.7, 0.04);
                glUniform1f(prog->getUniform("shine"), 0.078);
                break;
            case 4: //gold
                glUniform3f(prog->getUniform("matAmb"), 0.24725, 0.1995, 0.0745);
                glUniform3f(prog->getUniform("matDif"), 0.75164, 0.60648, 0.22648);
                glUniform3f(prog->getUniform("matSpec"), 0.628281, 0.555802, 0.366065);
                glUniform1f(prog->getUniform("shine"), 0.4);
                break;
            case 5: //jade
                glUniform3f(prog->getUniform("matAmb"), 0.135, 0.2225, 0.1575);
                glUniform3f(prog->getUniform("matDif"), 0.54, 0.89, 0.63);
                glUniform3f(prog->getUniform("matSpec"), 0.116228, 0.116228, 0.116228);
                glUniform1f(prog->getUniform("shine"), 0.4);
                break;
                
        }
    }

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
        
        
        
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
        
        prog->addUniform("matDif");
        prog->addUniform("matAmb");
        prog->addUniform("matSpec");
        prog->addUniform("shine");
        prog->addUniform("LightPos");
        //prog->addUniform("Texture0");
        
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
        prog->addAttribute("vertTex");
        
        
        
        
        prog2 = make_shared<Program>();
        prog2->setVerbose(true);
        prog2->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
        
        
        prog2->init();
        prog2->addUniform("P");
        prog2->addUniform("V");
        prog2->addUniform("M");
        
        prog2->addUniform("Texture0");
        
        prog2->addAttribute("vertPos");
        prog2->addAttribute("vertNor");
        prog2->addAttribute("vertTex");
        
        
        cubeProg = make_shared<Program>();
        cubeProg->setVerbose(true);
        cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
        
        
        cubeProg->init();
        cubeProg->addUniform("P");
        cubeProg->addUniform("V");
        cubeProg->addUniform("M");
        
        //prog2->addUniform("Texture0");
        
        cubeProg->addAttribute("vertPos");
        cubeProg->addAttribute("vertNor");
//        prog2->addAttribute("vertTex");
	}

	void initGeom(const std::string& resourceDirectory)
	{

		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
        //std::vector<shared_ptr<Shapes>> allShapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		//load in the mesh and make the shape(s)
//        int minx = 1000;
//        int miny = 1000;
//        int minz = 1000;
//        int maxx = -1000;
//        int maxy = -1000;
//        int maxz = -1000;
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/pony.obj").c_str());
       
		if (!rc) {
			cerr << errStr << endl;
		}
        else {
            int len = TOshapes.size();
        
            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);

            }
			
		}
        
        
        bool rc2 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
        
        if (!rc2) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();

            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        bool rc3 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
        
        if (!rc3) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();

            int minx = 1000;
            int miny = 1000;
            int minz = 1000;
            int maxx = -1000;
            int maxy = -1000;
            int maxz = -1000;
            
            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
                
                
                if(i==22)
                {
                    hxmin = mesh->min.x;
                    hxmax = mesh->max.x;
                    hymin = mesh->min.y;
                }
                if(i==23)
                {
                    dxmin = mesh->min.x;
                    dxmax = mesh->max.x;
                    dymin = mesh->min.y;
                }
                
                if(mesh->min.x < minx)
                {
                    minx =mesh->min.x;
                }
                if(mesh->min.y < miny)
                {
                    miny =mesh->min.y;
                }
                if(mesh->min.z < minz)
                {
                    minz =mesh->min.z;
                }
                
                if(mesh->max.x < maxx)
                {
                    maxx =mesh->max.x;
                }
                if(mesh->max.y < maxy)
                {
                    maxy =mesh->max.y;
                }
                if(mesh->max.z < maxz)
                {
                    maxz =mesh->max.z;
                }

                
            }
            
                gMin.x = minx;
                gMin.y = miny;
                gMin.z = minz;
                gMax.x = maxx;
                gMax.y = maxy;
                gMax.z = maxz;
            
        }
        bool rc4 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/tree.obj").c_str());

        if (!rc4) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();

            //for len of toshape


            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }

        }
        
        bool rc5 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cactus.obj").c_str()); //33
        
        if (!rc5) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();
        
            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        bool rc6 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/bunnyN.obj").c_str()); //34
        
        if (!rc6) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();
            
            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        bool rc7 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/MapleTreeStem.obj").c_str()); //35
        
        if (!rc7) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();
            
            //for len of toshape
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        bool rc8 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/MapleLeaves.obj").c_str()); //36->?
        
        if (!rc8) {
            cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();
            
            //for len of toshape
//            cout<<"len: ";
//            cout<<len;
//
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        bool rc9 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/SmoothSphere.obj").c_str()); //36->?
        
        if (!rc9)
        {
            cerr << errStr << endl;
        }
        else
        {
            int len = TOshapes.size();
            
           
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }

        
	
    bool rc10 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/grass.obj").c_str()); //36->?
    
    if (!rc10) {
        cerr << errStr << endl;
        }
        else {
            int len = TOshapes.size();
            
            
            for(int i = 0; i < len; i ++)
            {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
            
        }
        
        
    }

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   }
    
    void drawHorse(std::shared_ptr<MatrixStack> Model, float scale, float rotx, float rotz, float tx, float ty, float tz) //make_shared<MatrixStack>();
    {
        Model->pushMatrix();
        Model->loadIdentity();
        //Model->translate(vec3(gTrans, sTrans, zTrans));
        /* draw top cube - aka head */
        Model->pushMatrix();
        Model->rotate(rotx, vec3(1,0,0));
        Model->rotate(rotz, vec3(0,0,1));
        Model->translate(vec3(tx, ty, tz));
        Model->scale(vec3(scale, scale, scale));
        setModel(prog, Model);
        allShapes[0]->draw(prog);
        Model->popMatrix();
        Model->popMatrix();
    }
    
     void drawCact(std::shared_ptr<MatrixStack> Model, float scale, float rotx, float roty, float tx, float ty, float tz)
    {
        Model->pushMatrix();
        Model->loadIdentity();
        //Model->translate(vec3(gTrans, sTrans, zTrans));
        /* draw top cube - aka head */
        Model->pushMatrix();
        //Model->rotate(-1.5, vec3(1,0,0));
    
        Model->translate(vec3(tx, ty, tz));
        Model->rotate(roty, vec3(0,1,0));
        Model->rotate(rotx, vec3(1,0,0));
        Model->scale(vec3(scale, scale-0.05, scale));
        setModel(prog2, Model);
        allShapes[33]->draw(prog2);
    
        Model->popMatrix();
        Model->popMatrix();
    }
    
    void drawTree(std::shared_ptr<MatrixStack> Model, float scalex, float scaley, float scalez, float tx, float ty, float tz)
    {
        Model->pushMatrix();
        Model->loadIdentity();
        //Model->translate(vec3(gTrans, sTrans, zTrans));
        /* draw top cube - aka head */
        Model->pushMatrix();
        //Model->rotate(-1.5, vec3(1,0,0));
        //Model->rotate(0.5, vec3(0,0,1));
        Model->translate(vec3(tx, ty, tz));
        Model->scale(vec3(scalex, scaley, scalez));
        setModel(prog, Model);
        allShapes[31]->draw(prog);
        allShapes[32]->draw(prog);
        Model->popMatrix();
        Model->popMatrix();
        
    }
    void drawMaple(std::shared_ptr<MatrixStack> Model, float scalex, float scaley, float scalez, float tx, float ty, float tz)
    {
        texture2->bind(prog2->getUniform("Texture0"));
        Model->pushMatrix();
        Model->loadIdentity();
        //Model->translate(vec3(gTrans, sTrans, zTrans));
        /* draw top cube - aka head */
        Model->pushMatrix();
        Model->translate(vec3(tx, ty, tz));
        // Model->rotate(0, vec3(1,0,0));
        //Model->translate(vec3(25, -1, 7));
        Model->scale(vec3(scalex, scaley, scalez));
        setModel(prog, Model);
        allShapes[35]->draw(prog);
        Model->popMatrix();
        Model->popMatrix();
        
        texture1->bind(prog2->getUniform("Texture0"));
        Model->pushMatrix();
        Model->loadIdentity();
        //Model->translate(vec3(gTrans, sTrans, zTrans));
        /* draw top cube - aka head */
        Model->pushMatrix();
        Model->translate(vec3(tx, ty, tz));
        // Model->rotate(0, vec3(1,0,0));
        //Model->translate(vec3(25, -1, 7));
        Model->scale(vec3(scalex, scaley, scalez));
        setModel(prog, Model);
        allShapes[36]->draw(prog);
        allShapes[37]->draw(prog);
        Model->popMatrix();
        Model->popMatrix();
    }
    

	void render() {
		// Get current frame buffer size.
		int width, height;
        renderCtr++;
        double posX, posY;
        
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
        
        //cout << "velocity: " << velocity.x << " " << velocity.z << endl;
        if(!init1)
        {
            int x,y;
            glfwGetWindowPos(windowManager->getHandle(),&x, &y);
            glfwSetWindowPos(windowManager->getHandle(),x+1,y);
            oldx = 0;
            oldy = 0;
            
            init1 = true;
        }
        //glfwGetCursorPos(windowManager->getHandle(), &posX, &posY);
        //mat4 view = glm::lookAt(eye, lookAt, up);
        lookAt.x = (cos(phi) * cos(theta)) *radius;
        lookAt.y = sin(phi) *radius;
        lookAt.z = (cos(phi) * cos(1.57079632679 - theta)) *radius;
        
        eye = lookAt + horsePos;
        
        
        
        if(run)
        {
            if(renderCtr % 10 ==0)
            {
                if(ind == 16)
                {
                    ind = 0;
                }
                ind++;
                if(ind2 == 16)
                {
                    ind2 = 0;
                }
                ind2++;
                if(ind3 == 16)
                {
                    ind3 = 0;
                }
                ind3++;
                if(ind4 == 16)
                {
                    ind4 = 0;
                }
                ind4++;
                
                
            }
            
            
            
        }

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 450.0f); //change

		// View is global translation along negative z for now
//        View->pushMatrix();
//            View->loadIdentity();
//        View->rotate(rot, vec3(0,1,0));
//            View->translate(vec3(0, 0, -75));

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(view));
        glUniform3f(prog->getUniform("LightPos"), -2+lTrans,2,2);


        
        
        //CATS
        prog->unbind();
        prog2->bind();
        glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
        glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, value_ptr(view));
        texture0->bind(prog2->getUniform("Texture0"));
        
        drawCact(Model, 0.2, -1.5, -0.9, 5, -5, 45);
//        texture1->bind(prog2->getUniform("Texture0"));
//        drawCat(Model, 0.2, -1.5, 0.2, -5, -5, 45);
//
        
        //MAPLE TREE

//        drawMaple(Model, 0.5, 0.5, 0.5, -36, -5, 60);
//        drawMaple(Model, 0.7, 0.7, 0.7, -65, -7, 80);
//        drawMaple(Model, 0.3, 0.5, 0.5, -70, -7, 30);
//        drawMaple(Model, 0.3, 0.5, 0.5, -70, -7, 120);
//        drawMaple(Model, 0.5, 0.5, 0.5, -30, -7, 120);
//        drawMaple(Model, 1, 1, 1, 40, -14, 50);
        
        drawMaple(Model, 0.5, 0.5, 0.5, -36, -9, 60);
        drawMaple(Model, 0.7, 0.7, 0.7, -65, -9, -80);
        drawMaple(Model, 0.3, 0.6, 0.5, -70, -9, 30);
        drawMaple(Model, 0.3, 0.5, 0.5, -50, -9, 120);
        drawMaple(Model, 0.5, 0.5, 0.5, -30, -9, 100);
        drawMaple(Model, 1, 1, 1, 40, -14, 50);
        drawMaple(Model, 0.7, 0.7, 0.7, 25, -9, -10);
        drawMaple(Model, 0.7, 0.7, 0.7, 30, -9, -60);
        drawMaple(Model, 0.7, 0.9, 0.7, -30, -9, -70);
        drawMaple(Model, 0.7, 0.7, 0.7, -75, -9, -90);
        
        
        drawMaple(Model, 0.7, 0.7, 0.7, -25, -9, 14);
        drawMaple(Model, 0.7, 0.9, 0.7, -39, -9, 90);
        drawMaple(Model, 0.7, 0.7, 0.7, 30, -9, 73);
        drawMaple(Model, 0.7, 0.7, 0.7, 75, -9, 81);
        
        drawMaple(Model, 0.7, 0.7, 0.7, 65, -9, 5);
        drawMaple(Model, 0.7, 0.9, 0.7, 40, -9, 12);
        drawMaple(Model, 0.7, 0.7, 0.7, -10, -9, 50);
        
        drawMaple(Model, 0.7, 0.7, 0.7, 100, -9, 54);
        drawMaple(Model, 0.7, 0.9, 0.7, 88, -9, 42);
        drawMaple(Model, 0.5, 0.5, 0.5, -20, -9, 81);
        
        drawMaple(Model, 0.7, 0.9, 0.7, -150, -13, -150);
        drawMaple(Model, 0.7, 0.9, 0.7, -140, -13, -120);
        drawMaple(Model, 0.7, 0.9, 0.7, -120, -13, -90);
        
        drawMaple(Model, 0.7, 0.9, 0.7, 150, -9, 150);
        drawMaple(Model, 0.7, 0.9, 0.7, 140, -9, 120);
        drawMaple(Model, 0.7, 0.9, 0.7, 120, -8, 90);
        
        drawMaple(Model, 0.7, 0.9, 0.7, -150, -7, 150);
        drawMaple(Model, 0.7, 0.9, 0.7, -140, -10, 120);
        drawMaple(Model, 0.7, 0.9, 0.7, -120, -8, 90);
        
        drawMaple(Model, 0.7, 0.9, 0.7, 158, -10, -138);
        drawMaple(Model, 0.7, 0.9, 0.7, 140, -7, -120);
        drawMaple(Model, 0.7, 0.9, 0.7, 120, -8, -90);
        
        
        drawMaple(Model, 0.7, 0.9, 0.7, 102, -10, -152);
        drawMaple(Model, 0.7, 0.9, 0.7, 80, -11, -140);
        drawMaple(Model, 0.7, 0.9, 0.7, 50, -8, -90);
        
        drawMaple(Model, 0.7, 0.9, 0.7, -100, -10, -150);
        drawMaple(Model, 0.7, 0.9, 0.7, -80, -13, -130);
        drawMaple(Model, 0.7, 0.9, 0.7, -52, -8, -90);
        
        drawMaple(Model, 0.7, 0.9, 0.7, -102, -10, 134);
        drawMaple(Model, 0.7, 0.9, 0.7, -63, -7, 160);
        drawMaple(Model, 0.7, 0.9, 0.7, -23, -8, 87);
        
        drawMaple(Model, 0.7, 0.9, 0.7, 100, -10, 150);
        drawMaple(Model, 0.7, 0.9, 0.7, 83, -7, 160);
        drawMaple(Model, 0.7, 0.9, 0.7, 51, -8, 93);
        
        
        //        //GROUND
        //
        //SetMaterial(3);
        texture1->bind(prog2->getUniform("Texture0"));
        Model->pushMatrix();
            Model->loadIdentity();
            Model->translate(vec3(gTrans, sTrans, 0));
            /* draw top cube - aka head */
            Model->pushMatrix();
            Model->translate(vec3(0,-13.8,0));
            Model->rotate(-1.6, vec3(1,0,0));
            //Model->translate(vec3(25, -1, 7));
            // Model->scale(vec3(400, 1, 400));
            Model->scale(vec3(1, 1, 1));
            setModel(prog, Model);
            allShapes[39]->draw(prog);
        Model->popMatrix();
        Model->popMatrix();
        
        //        //END GROUND
        
        
        
        prog2->unbind();
        prog->bind();
        
       
        vec3 toOrigin = glm::vec3(0,0,0);
        //my take on horse
        SetMaterial(changeMat);
        Model->pushMatrix();
            Model->loadIdentity();
            if(jumpSequence)
            {
                run = false;
                Model->translate(vec3(0, sTheta*2.5, 0));
                if(sTheta <= 0.003)
                {
                    jumpSequence = false;
                    run = true;
                }
            }
            Model->translate(vec3(horsePos.x, 0, horsePos.z));
            Model->rotate(rotH,vec3(0,1,0));
            /* draw tummy*/
            Model->pushMatrix();//tummy
        
        
            Model->pushMatrix();//shoulder
        
                Model->translate(vec3(2,0.4,0));
                toOrigin.x = -2;
                toOrigin.x = -0.4;
        
        
                Model->pushMatrix();//neck
                    Model->translate(vec3(1,0.7,0));
                    Model->rotate(0.7,vec3(0,0,1));
                    //Model->rotate(sTheta,vec3(0,0,1));
                    Model->translate(vec3(toOrigin.x+1.8, toOrigin.y+0.3, toOrigin.z));
                    toOrigin.x += 1.8;
                    toOrigin.x += 0.3;
        
                    Model->pushMatrix();//head1
        
                        Model->translate(vec3(1.6,-0.3,0));
        
                        Model->pushMatrix();//head2
                            Model->translate(vec3(0.7,-0.2,0));
                            //Model->rotate(sTheta,vec3(0,0,1));
                            Model->rotate(-1.6,vec3(0,0,1));
                            Model->translate(vec3(toOrigin.x-1.2,toOrigin.y,toOrigin.z));
                            toOrigin.x += -0.2;
                            toOrigin.x += -0.6;
                            Model->scale(vec3(1.3,0.7,0.9));
                            setModel(prog, Model);
                            allShapes[38]->draw(prog);
                        Model->popMatrix();//head2
                        Model->pushMatrix();//ear1
                            Model->translate(vec3(0,0.2,-0.4));
                            Model->rotate(0.5,vec3(0,0,1));
                            Model->translate(toOrigin);
                            toOrigin.x += 0;
                            toOrigin.x += 0;
                            Model->scale(vec3(0.5,0.2,0.2));
                            setModel(prog, Model);
                            allShapes[38]->draw(prog);
                        Model->popMatrix();//ear1
                        Model->pushMatrix();//ear2
                            Model->translate(vec3(0,0.2,0.4));
                            Model->rotate(0.5,vec3(0,0,1));
                            Model->translate(toOrigin);
                            toOrigin.x += 0;
                            toOrigin.x += 0;
                            Model->scale(vec3(0.5,0.2,0.2));
                            setModel(prog, Model);
                            allShapes[38]->draw(prog);
                        Model->popMatrix();//ear2
        
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//head1
        
        
        
                Model->scale(vec3(2,1.2,1.2));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
        
        
                Model->popMatrix();//neck
        
        toOrigin.x = 0.9;
        toOrigin.y = -0.1;
        toOrigin.z = 0;
        
                Model->pushMatrix();//leftThighFRONT
                    Model->translate(vec3(0.4,-1.6,0.9));
                    Model->rotate(keyframes[ind].t,vec3(0,0,1));
                    //Model->rotate(sTheta,vec3(0,0,1));
                    Model->translate(toOrigin);
                    toOrigin.x += 0;
                    toOrigin.y += 0;
        
                    Model->pushMatrix();//leftLegFRONT
                        Model->translate(vec3(1.8,0,0));
                        Model->rotate(keyframes[ind].c,vec3(0,0,1));
                        //Model->rotate(-sTheta,vec3(0,0,1));
                        Model->translate(toOrigin);
                        toOrigin.x += -0.6;
                        toOrigin.y += 0.1;
        
                        Model->pushMatrix();//leftAnkleFRONT
                            Model->translate(vec3(0.9,0,0));
                            Model->rotate(keyframes[ind].h,vec3(0,0,1));
                          //Model->rotate(sTheta-0.4,vec3(0,0,1));
                            Model->rotate(-1.2,vec3(0,0,1));
                            Model->translate(toOrigin);
                            toOrigin.x += 0;
                            toOrigin.x += 0;
                            Model->scale(vec3(0.3,0.2,0.2));
                            setModel(prog, Model);
                            allShapes[38]->draw(prog);
                        Model->popMatrix();//leftAnkleFRONT
        
                        Model->scale(vec3(1.2,0.3,0.3));
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//leftLegFRONT
        
                    Model->scale(vec3(1.9,0.6,0.6));
                    setModel(prog, Model);
                    allShapes[38]->draw(prog);
                Model->popMatrix();//leftThighFRONT
        
        
        toOrigin.x = 0.9;
        toOrigin.y = -0.1;
        toOrigin.z = 0;
        
            Model->pushMatrix();//leftThighFRONT
                Model->translate(vec3(0.4,-1.6,-0.9));
                Model->rotate(keyframes[ind2].t,vec3(0,0,1));
                //Model->rotate(sTheta,vec3(0,0,1));
                Model->translate(toOrigin);
                toOrigin.x += 0;
                toOrigin.y += 0;
        
                Model->pushMatrix();//leftLegFRONT
                    Model->translate(vec3(1.8,0,0));
                    Model->rotate(keyframes[ind2].c,vec3(0,0,1));
                    //Model->rotate(-sTheta,vec3(0,0,1));
                    Model->translate(toOrigin);
                    toOrigin.x += -0.6;
                    toOrigin.y += 0.1;
        
                    Model->pushMatrix();//leftAnkleFRONT
                        Model->translate(vec3(0.9,0,0));
                        Model->rotate(keyframes[ind2].h,vec3(0,0,1));
                        //Model->rotate(sTheta-0.4,vec3(0,0,1));
                        Model->rotate(-1.2,vec3(0,0,1));
                        Model->translate(toOrigin);
                        toOrigin.x += 0;
                        toOrigin.x += 0;
                        Model->scale(vec3(0.3,0.2,0.2));
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//leftAnkleFRONT
        
                    Model->scale(vec3(1.2,0.3,0.3));
                    setModel(prog, Model);
                    allShapes[38]->draw(prog);
                Model->popMatrix();//leftLegFRONT
        
                Model->scale(vec3(1.9,0.6,0.6));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
            Model->popMatrix();//leftThighFRONT
        
        
        
        
                Model->scale(vec3(1.8,2,1.8));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
        
            Model->popMatrix();//shoulder
        
        
            Model->pushMatrix();//hindquarters
        
                Model->translate(vec3(-2.5,0,0));
                toOrigin.x = -2;
                toOrigin.x = -0.4;
                Model->rotate(-0.2,vec3(0,0,1));
        
        toOrigin.x = 1.3;
        toOrigin.y = -0.1;
        toOrigin.z = 0;
        
        Model->pushMatrix(); //rightUpperThighBack
            Model->translate(vec3(-0.3,-0.6,0.9));
            Model->rotate(keyframes2[ind3].t,vec3(0,0,1));
            //Model->rotate(sTheta,vec3(0,0,1));
            Model->translate(toOrigin);
            toOrigin.x += -0.5;
            toOrigin.y += 0.1;
            Model->pushMatrix();//rightLowerThighBACK
                Model->translate(vec3(1.4,-0.1,0));
                Model->rotate(keyframes2[ind3].c,vec3(0,0,1));
                //Model->rotate(sTheta,vec3(0,0,1));
                Model->translate(toOrigin);
                toOrigin.x += -0.2;
                toOrigin.y += 0.1;
        
                Model->pushMatrix();//rightLegBACK
                    Model->translate(vec3(1.1,0.1,0));
                    Model->rotate(keyframes2[ind3].h,vec3(0,0,1));
                    //Model->rotate(-sTheta,vec3(0,0,1));
                    Model->translate(toOrigin);
                    toOrigin.x += -0.4;
                    toOrigin.y += -0.1;
        
                    Model->pushMatrix();//rightAnkleBACK
                        Model->translate(vec3(0.7,-0.1,0));
                        Model->rotate(keyframes2[ind3].bb,vec3(0,0,1));
                        //Model->rotate(-sTheta-0.4,vec3(0,0,1));
        
                        Model->translate(toOrigin);
                        toOrigin.x += 0;
                        toOrigin.x += 0;
                        Model->scale(vec3(0.3,0.2,0.2));
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//rightAnkleBACK
        
                    Model->scale(vec3(0.9,0.3,0.3));
                    setModel(prog, Model);
                    allShapes[38]->draw(prog);
                Model->popMatrix();//rightLegBACK
        
                Model->scale(vec3(1.1,0.4,0.4));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
            Model->popMatrix();//rightLowerThighBACK
        
            Model->scale(vec3(1.6,0.6,0.6));
            setModel(prog, Model);
            allShapes[38]->draw(prog);
        Model->popMatrix();//rightUpperThighBACK
        
        toOrigin.x = 1.3;
        toOrigin.y = -0.1;
        toOrigin.z = 0;
        
        Model->pushMatrix(); //leftUpperThighBack
            Model->translate(vec3(-0.3,-0.6,-0.9));
            Model->rotate(keyframes2[ind4].t,vec3(0,0,1));
            //Model->rotate(sTheta,vec3(0,0,1));
            Model->translate(toOrigin);
            toOrigin.x += -0.5;
            toOrigin.y += 0.1;
            Model->pushMatrix();//leftLowerThighBACK
                Model->translate(vec3(1.4,-0.1,0));
                Model->rotate(keyframes2[ind4].c,vec3(0,0,1));
                //Model->rotate(sTheta,vec3(0,0,1));
                Model->translate(toOrigin);
                toOrigin.x += -0.2;
                toOrigin.y += 0.1;
        
                Model->pushMatrix();//leftLegBACK
                    Model->translate(vec3(1.1,0.1,0));
                    Model->rotate(keyframes2[ind4].h,vec3(0,0,1));
                    //Model->rotate(-sTheta,vec3(0,0,1));
                    Model->translate(toOrigin);
                    toOrigin.x += -0.4;
                    toOrigin.y += -0.1;
        
                    Model->pushMatrix();//leftAnkleBACK
                        Model->translate(vec3(0.7,-0.1,0));
                        Model->rotate(keyframes2[ind4].bb,vec3(0,0,1));
                        //Model->rotate(-sTheta-0.4,vec3(0,0,1));
        
                        Model->translate(toOrigin);
                        toOrigin.x += 0;
                        toOrigin.x += 0;
                        Model->scale(vec3(0.3,0.2,0.2));
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//leftAnkleBACK
        
                    Model->scale(vec3(0.9,0.3,0.3));
                    setModel(prog, Model);
                    allShapes[38]->draw(prog);
                Model->popMatrix();//leftLegBACK
        
                Model->scale(vec3(1.1,0.4,0.4));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
            Model->popMatrix();//leftLowerThighBACK
        
            Model->scale(vec3(1.6,0.6,0.6));
            setModel(prog, Model);
            allShapes[38]->draw(prog);
        Model->popMatrix();//leftUpperThighBACK
        
        
        
        
    
        

        
        
                Model->pushMatrix();//tail
                    Model->translate(vec3(-1.2,0.7,0));
                    Model->rotate(-2.5,vec3(0,0,1));
                    //Model->rotate(sTheta-0.4,vec3(0,0,1));
        
                    Model->translate(toOrigin);
                    toOrigin.x += 0;
                    toOrigin.x += 0;
                    Model->pushMatrix();//tail1
                        Model->translate(vec3(0.7,0.1,0));
                        Model->rotate(1.5,vec3(0,0,1));
                        //Model->rotate(sTheta,vec3(0,0,1));
                        Model->rotate(-0.8,vec3(0,0,1));
        
                        Model->translate(toOrigin);
                        toOrigin.x += 0;
                        toOrigin.x += 0;
                        Model->scale(vec3(0.9,0.4,0.4));
                        setModel(prog, Model);
                        allShapes[38]->draw(prog);
                    Model->popMatrix();//tail1
        
                    Model->scale(vec3(0.9,0.4,0.4));
                    setModel(prog, Model);
                    allShapes[38]->draw(prog);
                Model->popMatrix();//tail
        
        
        
                Model->scale(vec3(1.6,1.8,1.6));
                setModel(prog, Model);
                allShapes[38]->draw(prog);
        
            Model->popMatrix();//hindquarters
        
        
        
        
        
            Model->scale(vec3(3.3,2,2));
            setModel(prog, Model);
            allShapes[38]->draw(prog);
            Model->popMatrix();//tummy
        Model->popMatrix();
        //end my horse
        
        
        
        //BUNNY (no normals- need to compute)
        SetMaterial(2);
        Model->pushMatrix();
            Model->loadIdentity();
            //Model->translate(vec3(gTrans, sTrans, zTrans));
            /* draw top cube - aka head */
            Model->pushMatrix();
            Model->translate(vec3(-15,-5.5,45));
           // Model->rotate(0, vec3(1,0,0));
            //Model->translate(vec3(25, -1, 7));
            Model->scale(vec3(30, 30, 30));
            setModel(prog, Model);
            allShapes[34]->draw(prog);
            Model->popMatrix();
        Model->popMatrix();
        
    

        
        SetMaterial(4);
        
        
       
        
        
        
        
        
//        //rect
//        Model->pushMatrix();
//        Model->loadIdentity();
//        Model->translate(vec3(gTrans, sTrans, 0));
//        /* draw top cube - aka head */
//        Model->pushMatrix();
//        //Model->rotate(0.1, vec3(1,0,0));
//        Model->translate(vec3(10,0,5));
//        Model->scale(vec3(30, 8, 3));
//        setModel(prog, Model);
//        allShapes[1]->draw(prog);
//        Model->popMatrix();
//        Model->popMatrix();
        //end rect
        
        //SKY BOX
        //to draw the sky box bind the right shader
        cubeProg->bind();
        //set the projection matrix - can use the same one
        glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE,
                           value_ptr(Projection->topMatrix()));
        //set the depth function to always draw the box!
        glDepthFunc(GL_LEQUAL);
        //set up view matrix to include your view transforms
        //(your code likely will be different depending
        glUniformMatrix4fv(cubeProg->getUniform("V"), 1,
                           GL_FALSE,value_ptr(view) );
        //set and send model transforms - likely want a bigger cube
        glUniformMatrix4fv(cubeProg->getUniform("M"), 1,
                           GL_FALSE,value_ptr(Model->topMatrix()));
        //bind the cube map texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky);
        //draw the actual cube
        Model->scale(vec3(350,350,350));
        setModel(cubeProg, Model);
        allShapes[1]->draw(cubeProg);
        //set the depth test back to normal!
        glDepthFunc(GL_LESS);
        //unbind the shader for the skybox
        cubeProg->unbind();
        
        

		prog->unbind();

		//animation update example
		sTheta = abs(sin(glfwGetTime()));
        gTheta =cos(glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		//View->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();
    

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);
    application->initTex(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
