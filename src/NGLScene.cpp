#include <QMouseEvent>
#include <QGuiApplication>


#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/TransformStack.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include "NGLScene.h"
#include "SPHMelt.h"

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene(QWindow *_parent) : OpenGLWindow(_parent)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  setTitle("SPH Melt Simulation");

  m_timerLeft = new QTimer();
  m_timerRight = new QTimer();
  m_setForce = false;
  m_drawMesh = false;
  m_drawParticles = true;
  m_play = false;
  m_restart = false;
  m_toggleSim = false;
  m_forceRadius = 1.0;
  m_forceStrength = 0.0;
  m_externalForcePos = ngl::Vec3(0);
  m_forcePush = true;

}

NGLScene::~NGLScene()
{
  ngl::NGLInit *Init = ngl::NGLInit::instance();
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  Init->NGLQuit();

}

void NGLScene::resizeEvent(QResizeEvent *_event )
{
  //Store our window size
  m_windowWidth = _event->size().width();
  m_windowHeight = _event->size().height();
  if(isExposed())
  {
  int w=_event->size().width();
  int h=_event->size().height();
  // set the viewport for openGL
  glViewport(0,0,w,h);
  // now set the camera size values as the screen size has changed
  m_cam->setShape(45,(float)w/h,0.05,350);
  renderNow();
  }
}


void NGLScene::initialize()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.7f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,10,40);
  ngl::Vec3 to(0,10,0);
  ngl::Vec3 up(0,1,0);



  m_cam= new ngl::Camera(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam->setShape(45,(float)720.0/576.0,0.05,350);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // we are creating a shader called PerFragADS
  shader->createShaderProgram("PerFragADS");
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("PerFragADSVertex",ngl::VERTEX);
  shader->attachShader("PerFragADSFragment",ngl::FRAGMENT);
  // attach the source
  shader->loadShaderSource("PerFragADSVertex","shaders/PerFragASDVert.glsl");
  shader->loadShaderSource("PerFragADSFragment","shaders/PerFragASDFrag.glsl");
  // compile the shaders
  shader->compileShader("PerFragADSVertex");
  shader->compileShader("PerFragADSFragment");
  // add them to the program
  shader->attachShaderToProgram("PerFragADS","PerFragADSVertex");
  shader->attachShaderToProgram("PerFragADS","PerFragADSFragment");

  // now we have associated this data we can link the shader
  shader->linkProgramObject("PerFragADS");
  // and make it active ready to load values
  (*shader)["PerFragADS"]->use();
  // now we need to set the material and light values
  /*
   *struct MaterialInfo
   {
        // Ambient reflectivity
        vec3 Ka;
        // Diffuse reflectivity
        vec3 Kd;
        // Specular reflectivity
        vec3 Ks;
        // Specular shininess factor
        float shininess;
  };*/
  shader->setShaderParam3f("material.Ka",0.1,0.1,0.1);
  // red diffuse
  shader->setShaderParam3f("material.Kd",0.8,0.8,0.8);
  // white spec
  shader->setShaderParam3f("material.Ks",1.0,1.0,1.0);
  shader->setShaderParam1f("material.shininess",1000);
  // now for  the lights values (all set to white)
  /*struct LightInfo
  {
  // Light position in eye coords.
  vec4 position;
  // Ambient light intensity
  vec3 La;
  // Diffuse light intensity
  vec3 Ld;
  // Specular light intensity
  vec3 Ls;
  };*/
  shader->setUniform("light.position",ngl::Vec3(2,20,2));
  shader->setShaderParam3f("light.La",0.1,0.1,0.1);
  shader->setShaderParam3f("light.Ld",1.0,1.0,1.0);
  shader->setShaderParam3f("light.Ls",0.9,0.9,0.9);

  glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces

  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());
  m_text=new ngl::Text(QFont("Arial",16));
  m_text->setScreenSize(width(),height());

  (*shader)["nglToonShader"]->use();
  shader->setShaderParam4f("Colour",1,1,0,1);
  shader->setShaderParam3f("lightPos",1,1,1);
  shader->setShaderParam4f("lightDiffuse",1,1,1,1);

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createSphere("sphere",1.0,20);
  prim->createLineGrid("wall", 1, 1, 5);

  // first we create a mesh from an obj passing in the obj file and texture
  std::string modelLocation = "models/BruceReduced.obj";
  //std::string modelLocation = "models/pikatchu.obj";
  //std::string modelLocation = "models/pokeball.obj";


  //Load our mesh into our SPH Grid
  //m_SPHGrid = SPHMelt(mesh1,"models/BruceReduced.obj");
  m_SPHGrid = SPHMelt(modelLocation);
  //Initializes our arrays for our mesh
  m_SPHGrid.init();
  m_SPHGrid.setCam(m_cam);
  m_currentTime = m_currentTime.currentTime();
  startTimer(1);

}



void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["PerFragADS"]->use();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat4 M;
  M=m_transformStack.getCurrentTransform().getMatrix();
  MV= m_transformStack.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
  MVP=MV*m_cam->getProjectionMatrix() ;
  shader->setUniform("MVP",MVP);
  shader->setUniform("MV",MV);

}

void NGLScene::render()
{
  // clear the screen and depth buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   // Rotation based on the mouse position for our global transform
   ngl::Transformation trans;
   ngl::Mat4 rotX;
   ngl::Mat4 rotY;
   // create the rotation matrices
   rotX.rotateX(m_spinXFace);
   rotY.rotateY(m_spinYFace);
   // multiply the rotations
   ngl::Mat4 final=rotY*rotX;
   // add the translations
   final.m_m[3][0] = m_modelPos.m_x;
   final.m_m[3][1] = m_modelPos.m_y;
   final.m_m[3][2] = m_modelPos.m_z;
   // set this in the TX stack
   trans.setMatrix(final);
   m_transformStack.setGlobal(trans);

  loadMatricesToShader();

  // draw the mesh
  if(m_drawMesh){
    m_SPHGrid.drawMesh();
  }

  if(m_drawParticles){
      m_SPHGrid.drawParticles(m_transformStack);
  }
  m_SPHGrid.drawExternalForce(&m_transformStack,m_cam);
  m_SPHGrid.drawCollision(&m_transformStack,m_cam);

  // calculate the framerate
  QTime newTime = m_currentTime.currentTime();
  int msecsPassed = m_currentTime.msecsTo(newTime);
  m_currentTime = newTime;

  m_text->setColour(1.0f,1.0f,1.0f);
  QString text=QString("SPH simulation");
  m_text->renderText(10,18,text);
  text=QString("click '1' to toggle mesh");
  m_text->renderText(10,38,text);
  text=QString("click '2' to toggle particles");
  m_text->renderText(10,58,text);
  text=QString("Space to toggle pause");
  m_text->renderText(10,78,text);
  text=QString("Number of particles is %1").arg(m_SPHGrid.getNumParticles());
  m_text->renderText(10,98,text);
  text.sprintf("Press 3 to toggle between mesh and 2d sim");
  m_text->renderText(10,118,text);
  text.sprintf("Framerate is %d",(int)(1000.0/msecsPassed));
  m_text->renderText(10,138,text);
  text=QString("Force Radius: %1").arg(m_forceRadius);
  m_text->renderText(10,158,text);
  text=QString("Force Strength: %1").arg(m_forceStrength);
  m_text->renderText(10,178,text);
  text=QString("+/- to change force strength. 9/0 to change force radius.");
  m_text->renderText(10,198,text);
  text=QString("Ctrl and left-click to change the external force position");
  m_text->renderText(10,620,text);
  text=QString("E to toggle between pushing/sucking force");
  m_text->renderText(10,640,text);
  text=QString("Space to Play/Pause");
  m_text->renderText(10,660,text);
  text=QString("R to restart the simulation");
  m_text->renderText(10,680,text);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    renderNow();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    renderNow();

   }
  else if(m_setForce && _event->buttons() == Qt::LeftButton){
      // Convert from screen space to world space
      ngl::Mat4 MV;
      ngl::Mat4 MVP;
      MV= m_transformStack.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
      MVP=MV*m_cam->getProjectionMatrix() ;
      ngl::Mat4 invProjMat = MVP.inverse();
      ngl::Vec4 Pos((2.0*(_event->x()/(float)m_windowWidth))-1.0,
                    (2.0*((m_windowHeight - _event->y())/(float)m_windowHeight))-1.0,0.972,1.0);
      Pos =  Pos * invProjMat;
      Pos.m_x /= Pos.m_w;
      Pos.m_y /= Pos.m_w;
      Pos.m_z /= Pos.m_w;
      // set our external force postion
      m_externalForcePos = ngl::Vec3(Pos.m_x,Pos.m_y,Pos.m_z);
      m_SPHGrid.setExternalForce(ngl::Vec3(Pos.m_x,Pos.m_y,Pos.m_z),m_forceRadius, m_forceStrength,m_forcePush);
  }
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
    renderNow();
    if(m_play){
        m_SPHGrid.update();
    }
    if(m_restart){
        bool meshOn = false,partOn = false;
        if(m_drawMesh){
            meshOn = true;
        }
        if(m_drawParticles){
            partOn = true;
        }
        m_drawMesh = false;
        m_drawParticles = false;
        m_SPHGrid.restart();
        m_SPHGrid.setCam(m_cam);
        m_SPHGrid.resetClock();
        m_restart = false;
        if(meshOn){
            m_drawMesh = true;
        }
        if(partOn){
            m_drawParticles = true;
        }
    }
    if(m_toggleSim){
        if(m_drawMesh){
            m_drawMesh = false;
        }
        m_drawMesh = false;
        m_drawParticles = false;
        m_SPHGrid.toggleSim();
        m_SPHGrid.setCam(m_cam);
        m_SPHGrid.resetClock();
        m_toggleSim = false;
        m_drawParticles = true;
        m_play = false;

    }

}

void NGLScene::keyPressEvent(QKeyEvent *_event){

    switch(_event->key()){
    case(Qt::Key_Escape):
        this->close();
    break;
    //show or hide our mesh
    case( Qt::Key_1):
        if(m_drawMesh==true){
            m_drawMesh=false;
        }
        else{
            m_drawMesh = true;
        }
    break;
    //Show or hide our particles
    case(Qt::Key_2):
        if(m_drawParticles==true){
            m_drawParticles=false;
        }
        else{
            m_drawParticles = true;
        }
    break;
    case(Qt::Key_R):
        m_restart = true;
    break;
    case(Qt::Key_Space):
    if(m_play==true){
        m_play=false;
    }
    else{
        m_play = true;
        m_SPHGrid.resetClock();
    }
    break;
    case(Qt::Key_Control):
        m_setForce = true;
    break;
    case(Qt::Key_3):
        m_toggleSim = true;
    break;
    case(Qt::Key_Equal):
        m_forceStrength += 0.1;
        m_SPHGrid.setExternalForce(m_externalForcePos,m_forceRadius,m_forceStrength,m_forcePush);
    break;
    case(Qt::Key_Minus):
        if(m_forceStrength -= 0.1<0){
        m_forceStrength -= 0.1;
        }
        else{
            m_forceStrength = 0.0;
        }
        m_SPHGrid.setExternalForce(m_externalForcePos,m_forceRadius,m_forceStrength,m_forcePush);
    break;
    case(Qt::Key_0):
        m_forceRadius += 0.1;
        m_SPHGrid.setExternalForce(m_externalForcePos,m_forceRadius,m_forceStrength,m_forcePush);
    break;
    case(Qt::Key_9):
        if(m_forceRadius -= 0.1<0){
        m_forceRadius -= 0.1;
        }
        else{
            m_forceRadius = 0.0;
        }
        m_SPHGrid.setExternalForce(m_externalForcePos,m_forceRadius,m_forceStrength,m_forcePush);
    break;
    case(Qt::Key_E):
        if(m_forcePush){
            m_forcePush = false;
        }
        else{
            m_forcePush = true;
        }
        m_SPHGrid.setExternalForce(m_externalForcePos,m_forceRadius,m_forceStrength,m_forcePush);
    break;
    default:
    break;
    }
}
void NGLScene::keyReleaseEvent(QKeyEvent *_event){
    if(_event->key()==Qt::Key_Control){
        m_setForce = false;
    }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  if(m_setForce && _event->button() == Qt::LeftButton){
      // convert from screen space to world space
      ngl::Mat4 MV;
      ngl::Mat4 MVP;
      MV= m_transformStack.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
      MVP=MV*m_cam->getProjectionMatrix() ;
      ngl::Mat4 invProjMat = MVP.inverse();
      ngl::Vec4 Pos((2.0*(_event->x()/(float)m_windowWidth))-1.0,
                    (2.0*((m_windowHeight - _event->y())/(float)m_windowHeight))-1.0,0.972,1.0);
      Pos =  Pos * invProjMat;
      Pos.m_x /= Pos.m_w;
      Pos.m_y /= Pos.m_w;
      Pos.m_z /= Pos.m_w;
      // set our external force position
      m_externalForcePos = ngl::Vec3(Pos.m_x,Pos.m_y,Pos.m_z);
      m_SPHGrid.setExternalForce(ngl::Vec3(Pos.m_x,Pos.m_y,Pos.m_z),m_forceRadius, m_forceStrength,m_forcePush);

  }
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  else if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	renderNow();
}

