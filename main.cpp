#define GLFW_VERSION_MAJOR 4
#define GLFW_VERSION_MINOR 6
#define GLFW_VERSION_REVISION 0
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3

#include <glm/gtc/type_ptr.hpp>
#include  <cstdlib>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <vector>
#include <iterator>


#include <xercesc/sax2/DefaultHandler.hpp>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>




using namespace std;
using namespace xercesc;


class GexfglSax : public DefaultHandler {
public:
    void startElement(
        const   XMLCh* const    uri,
        const   XMLCh* const    localname,
        const   XMLCh* const    qname,
        const   Attributes& attrs
    ) {
        char* message = XMLString::transcode(qname);
        XMLSize_t len = attrs.getLength();
       
        if(strcmp(message,"node") == 0) {
        cout << XMLString::transcode(attrs.getValue(XMLString::transcode("label"))) << endl;
        }
       // cout << "I saw element: " << message << endl;
        XMLString::release(&message);
    };
    void fatalError(const SAXParseException& exception) {
        char* message = XMLString::transcode(exception.getMessage());
        cout << "Fatal Error: " << message
            << " at line: " << exception.getLineNumber()
            << endl;
        XMLString::release(&message);
    };
};


double previous_sec = 0;
int frame_count;
char filename[] = "C:\\Users\\totetmatt\\project\\viz-engine\\modules\\demo-lwjgl\\samples\\comic-hero-network.gexf";

void update_fps(GLFWwindow* window) {
    double current;
    double elapsed;

    current = glfwGetTime();
    elapsed = current - previous_sec;

    if (elapsed > 0.25) {
        previous_sec = current;
        char tmp[128];
        double fps = (double)frame_count / elapsed;
        sprintf_s(tmp, "Opengl @ fps : %.2f", fps);
        glfwSetWindowTitle(window, tmp);
        frame_count = 0;
    }
    frame_count++;

}


int createNodeIdIndexMap(std::map<std::string, int>* map, std::vector<int>* edges, std::vector<float>* positionNodes, std::vector<float>* colorsNodes) {
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Error during initialization! :\n" << message << "\n";
        XMLString::release(&message);
        return 1;
    }
    XMLCh* GEXF_NODE = XMLString::transcode("node");
    XMLCh* GEXF_EDGE = XMLString::transcode("edge");

    XMLCh* GEXF_EDGE_SOURCE = XMLString::transcode("source");
    XMLCh* GEXF_EDGE_TARGET = XMLString::transcode("target");

    XMLCh* GEXF_NODE_VIZ_POSITION = XMLString::transcode("viz:position");
    XMLCh* GEXF_NODE_X = XMLString::transcode("x");
    XMLCh* GEXF_NODE_Y = XMLString::transcode("y");

    XMLCh* GEXF_NODE_VIZ_COLOR = XMLString::transcode("viz:color");
    XMLCh* GEXF_NODE_R = XMLString::transcode("r");
    XMLCh* GEXF_NODE_G = XMLString::transcode("g");
    XMLCh* GEXF_NODE_B = XMLString::transcode("b");

    XMLCh* GEXF_NODE_ID = XMLString::transcode("id");

   
    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true); // optional
    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    const char* xmlFile = filename;

    try {
        parser->parse(xmlFile);
        DOMDocument* dom = parser->getDocument();
        /* Node */
        DOMNodeList* xmlnodeList = dom->getElementsByTagName(GEXF_NODE);
        DOMNodeList* positions = dom
            ->getElementsByTagName(GEXF_NODE_VIZ_POSITION);
        DOMNodeList* colors = dom
            ->getElementsByTagName(GEXF_NODE_VIZ_COLOR);
        int len = xmlnodeList->getLength();
        for (len = 0; len < xmlnodeList->getLength(); len++) {
            /* Id Index */
            DOMNode* xmlNode = xmlnodeList->item(len);
            string id = XMLString::transcode(xmlNode->getAttributes()->getNamedItem(GEXF_NODE_ID)->getNodeValue());
            map->insert(std::make_pair(id, len));

         
            /*for (int j = 0; j < xmlNode->getChildNodes()->getLength(); j++) {
                 
                if( strcmp(xmlNode->getChildNodes()->item(j)->getNodeName(), GEXF_NODE_VIZ_POSITION)) {
                    cout << GEXF_NODE_VIZ_POSITION << endl;
                }

            }*/
            /* Position & Color parse*/
            DOMNamedNodeMap *position = positions->item(len)
                ->getAttributes();

            positionNodes->push_back(stof(XMLString::transcode(
                position->getNamedItem(GEXF_NODE_X)
                ->getNodeValue())) );

            positionNodes->push_back(stof(XMLString::transcode(
                position->getNamedItem(GEXF_NODE_Y)
                ->getNodeValue())) );
            positionNodes->push_back(0.0f); // Z
        
            DOMNamedNodeMap* color = colors
                ->item(len)
                ->getAttributes();

            colorsNodes->push_back(stof(XMLString::transcode(
                color->getNamedItem(GEXF_NODE_R)
                ->getNodeValue())) / 255.0f);

            colorsNodes->push_back(stof(XMLString::transcode(
                color->getNamedItem(GEXF_NODE_G)
                ->getNodeValue())) / 255.0f);

            colorsNodes->push_back(stof(XMLString::transcode(
                color->getNamedItem(GEXF_NODE_B)
                ->getNodeValue()))/255.0f);

        }
        /* Edge */
        DOMNodeList* xmledgeList = dom->getElementsByTagName(GEXF_EDGE);

        for (len = 0; len < xmledgeList->getLength(); len++) {
            DOMNode* xmlNode = xmledgeList->item(len);
            string source = XMLString::transcode(xmlNode->getAttributes()->getNamedItem(GEXF_EDGE_SOURCE)->getNodeValue());
            string target = XMLString::transcode(xmlNode->getAttributes()->getNamedItem(GEXF_EDGE_TARGET)->getNodeValue());
            edges->push_back(map->at(source));
            edges->push_back(map->at(target));
        }
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n" << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n";
        return -1;
    }
    delete parser;
    delete errHandler;

}
double *glZoom ;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cout << xoffset << ":" << yoffset << endl;


    (*glZoom) += yoffset;
    if (*glZoom <0.1) {
        *glZoom = 0.1;
    }
}


int main() {
    /*try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Error during initialization! :\n" << message << "\n";
        XMLString::release(&message);
        return 1;
    }
    SAX2XMLReader* parserSax = XMLReaderFactory::createXMLReader();
    parserSax->setFeature(XMLUni::fgSAX2CoreValidation, false);
    parserSax->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);   // optiona

    DefaultHandler* defaultHandler = new GexfglSax();
    parserSax->setContentHandler(defaultHandler);
    parserSax->setErrorHandler(defaultHandler);
    try {
        parserSax->parse(filename);
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
            << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const SAXParseException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
            << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n";
        return -1;
    }

    delete parserSax;
    delete defaultHandler;
    return 0;
    return 0;*/
    std::map<std::string, int> nodeIdIndexMap;
    std::vector<int> edgeIdIndexList;
    std::vector<float> nodePositionList;
    std::vector<float> nodeColorList;
    createNodeIdIndexMap(&nodeIdIndexMap, &edgeIdIndexList, &nodePositionList, &nodeColorList);
    /*for (std::list<int>::iterator it = edgeIdIndexList.begin(); it != edgeIdIndexList.end(); ++it)
        std::cout << ' ' << *it;
    
    for (std::list<float>::iterator it = nodeColorList.begin(); it != nodeColorList.end(); ++it)
        std::cout << ' ' << *it;*/
    cout << "Edge : " << edgeIdIndexList.size() /2. << endl;
    cout << "Position : " << nodePositionList .size() / 3. << endl;
    cout << "Color : " << nodeColorList.size() / 3. << endl;
   
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    int major, minor, revision;
    
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_CONTEXT_REVISION, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwGetVersion(&major, &minor, &revision);
   
    printf("Running against GLFW %i.%i.%i\n", major, minor, revision);

    // uncomment these lines if on Apple OS X
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    if (GLEW_ARB_uniform_buffer_object) {
        printf("GLEW_ARB ON\n");
    }
    else {
        printf("GLEW_ARB OFF\n");
    }
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    /* OTHER STUFF GOES HERE NEXT */
    float points[] = {
    0.0f,  0.5f,  -1.0f,
    0.5f, -0.5f,  1.0f,
   -0.5f, -0.5f,  0.0f
    };


    float colours[] = {
      1.0f, 0.0f,  0.0f,
      0.0f, 1.0f,  0.0f,
      0.0f, 0.0f,  1.0f
    };

    glm::mat4 matrix = glm::mat4(
        0.001f, 0.0f , 0.0f , 0.0f,
        0.0f, 0.001f, 0.0f , 0.0f,
        0.0f, 0.0f , 0.001f, 0.0f,
        0.0f, 0.0f , 0.0f , 1.0f
    );

    GLuint points_vbo = 0;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, nodePositionList.size() * sizeof(float), &nodePositionList[0], GL_STATIC_DRAW);

    GLuint colours_vbo = 0;
    glGenBuffers(1, &colours_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
    glBufferData(GL_ARRAY_BUFFER, nodeColorList.size() * sizeof(float), &nodeColorList[0], GL_STATIC_DRAW);



    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint index_vbo = 0;
    glGenBuffers(1, &index_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIdIndexList.size() * sizeof(int), &edgeIdIndexList[0], GL_STATIC_DRAW);


    GLuint shader_programme = glCreateProgram();

  
  
    const char* vertex_shader =
        "#version 400\n"
        "layout(location = 0) in vec3 vertex_position; \
        layout(location = 1) in vec3 vertex_colour;  \
        uniform mat4 matrix; \
        out vec3 colour;  \
            void main() {  \
            colour = vertex_colour;  \
            gl_Position = matrix * vec4(vertex_position, 1.0);  \
        }";
    const char* fragment_shader =
        "#version 400\n"
        "in vec3 colour; \
    out vec4 frag_colour; \
    void main() {  \
        frag_colour = vec4(colour, 1.0);  \
    }";
   
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
   
    
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);

    glBindAttribLocation(shader_programme, 0, "vertex_position");
    glBindAttribLocation(shader_programme, 1, "vertex_colour");

    glLinkProgram(shader_programme);
 
    // When linked, no need for shader object anymore
    glDeleteShader(fs);
    glDeleteShader(vs);
    int i = 0;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLuint matrix_location = glGetUniformLocation(shader_programme, "matrix");
    float speed = 1.0f;
    float last_pos = 0.0f;
    double zoom = 1.0;
    glZoom = &zoom;
    while (!glfwWindowShouldClose(window)) {
        /*if( int(speed)% 6 < 2) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
        else if(int(speed) % 6 < 4) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }*/
       /* speed = glfwGetTime();
        float c = cos(speed), s = sin(speed);
    
    
        glm::mat4 rot = glm::mat4(
        c, -s , 0.0f , 0.0f,
        s, c , 0.0f , 0.0f,
        0.0f, 0.0f , 1.0f , 0.0f,
        0.0f, 0.0f , 0.0f , 1.0f
    );*/
        float c = cos(*glZoom*.1), s = sin(*glZoom * .1);


        glm::mat4 rot = matrix;
        glUseProgram(shader_programme);
        cout << "Zoom :" << *glZoom << endl;
        glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(rot));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       
        // glBindVertexArray(vao);
        // glDrawArrays(GL_LINES, 0, edgeIdIndexList.size());
        glDrawElements(GL_LINES, edgeIdIndexList.size(), GL_UNSIGNED_INT, (void*)0);
        glfwPollEvents();
        glfwSwapBuffers(window);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
        glfwSetScrollCallback(window, scroll_callback);
        update_fps(window);
    }
    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}