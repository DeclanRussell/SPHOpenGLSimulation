#ifndef PARTICLE_H
#define PARTICLE_H

//----------------------------------------------------------------------------------------------------------------------
/// @file particle.h
/// @brief Ths is my particle class, it will store all the data for my particles for my simulation
/// @author Declan Russell
/// @version 1.0
/// @date 15/11/13
/// @class particle
/// @brief Our particle class
//----------------------------------------------------------------------------------------------------------------------


#include <ngl/Vec4.h>
#include <ngl/Camera.h>
#include <ngl/TransformStack.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>


class particle{
public:
    /// @brief our constructor to create our particle
    /// @param _pos -  a ngl::vec3* of the position of our particle
    /// @param _mass -  the mass of our particle used for later calculations
    /// @param _partOfMesh - if part of mesh will store a pointer to the vector location else will create a local one
    particle(ngl::Vec3 *_pos, float _mass, bool _partOfMesh = false);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default dtor
    ~particle();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to return our particles position
    ngl::Vec3 getPos() const {return *m_position;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to return our particles Acceleration
    ngl::Vec3 getAcc() const{return m_acceleration;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to return our particles velocity
    ngl::Vec3 getVel() const{return m_velocity;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to return the radius of our particle
    float getRadius() const{return m_radius;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to update our Postion once calculated
    /// @param _pos Vertex position
    inline void setPos(ngl::Vec3 _pos){*m_position = _pos;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to update our Acceleration once calculated
    /// @param _acc the acceleration Vertext
    inline void setAcc(ngl::Vec3 _acc){m_acceleration = _acc;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to update our Velocity once calculated
    /// @param _vel the Velocity Vertex
    inline void setVel(ngl::Vec3 _vel){m_velocity = _vel;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the mass of our particle
    /// @param _mass - the mass
    inline void setMass(float _mass){m_mass = _mass;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns the mass of our particle
    inline float getMass(){return m_mass;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the dynamic density of our particle
    /// @param _density - the density
    inline void setDensity(float _density){m_density = _density;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns the rest density of our particle
    inline float getRestDensity(){return m_restDensity;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the rest density of our particle
    /// @param _density - the rest density
    inline void setRestDensity(float _density){m_restDensity = _density;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns the density of our particle
    inline float getDensity(){return m_density;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draws our particles
    /// @param trans - our scenes transform stack to get our M matrix
    void draw(ngl::TransformStack trans);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set our scenes camera so we know our VP matrices for drawing
    /// @param our scenes camera
    void inline setCamera(ngl::Camera *_c){m_cam = _c;}
    //----------------------------------------------------------------------------------------------------------------------

private:
    /// @brief load Our MVP matrices and colour to the shader, uses NGL's toon shader. Called by draw()
    /// @param trans - our scenes transform stack to get our M matrix
    void loadMatricesToShader(ngl::TransformStack trans);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A variable to store our particles position
    /// @brief this is a pointer so that we can update the postion of the mesh data when we do our calculations rather then having to copy it over every time
    ngl::Vec3* m_position;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A variable to store our particles acceleration
    ngl::Vec3 m_acceleration;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A variable to store our particles velocity
    ngl::Vec3 m_velocity;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A variable to store our particles mass
    float m_mass;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A variable to store the dynamic density of our particle
    float m_density;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The resting density of our particle
    float m_restDensity;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A boolian to indicate if this particle is a part of our mesh and needs to be drawn
    bool m_partOfMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The radius of our particle for collision detection
    float m_radius;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief store the camera so we know the MVP matricies to draw in the right posotion
    ngl::Camera *m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the colour of the particle
    ngl::Colour m_colour;


};




#endif // PARTICLE_H
