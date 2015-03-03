#include "particle.h"

particle::particle(ngl::Vec3* _pos, float _mass, bool _partOfMesh){

    if(_partOfMesh){
        m_position = _pos;
        m_partOfMesh = true;
    }
    else{
        m_position = new ngl::Vec3(*_pos);
        m_partOfMesh = false;
    }
    m_mass = _mass;
    m_velocity = 0;
    m_acceleration = 0;
    m_radius = 0.1;
    m_colour = ngl::Colour(255,255,255);
}

particle::~particle(){
    m_position = new ngl::Vec3(0);
    m_mass = 1;
    m_velocity = 0;
    m_acceleration = 0;
    m_partOfMesh = false;
}


void particle::draw(ngl::TransformStack trans){

    m_colour.set(0,(255/m_density)*100,(255/m_density)*100);
    trans.setPosition(*m_position);
    trans.setScale(0.1,0.1,0.1);
    loadMatricesToShader(trans);
    ngl::VAOPrimitives::instance()->draw("sphere");
}

void particle::loadMatricesToShader(ngl::TransformStack trans)
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["nglToonShader"]->use();
  shader->setShaderParam4f("Colour",m_colour.m_r,m_colour.m_g,m_colour.m_b,m_colour.m_a);
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=trans.getCurrAndGlobal().getMatrix();
  MV=  trans.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
  MVP= M*m_cam->getVPMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  //shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
}
