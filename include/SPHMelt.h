#ifndef SPHMELT_H
#define SPHMELT_H

//----------------------------------------------------------------------------------------------------------------------
/// @file SPHMelt.h
/// @brief This is my SPHMelt class, If will be used to create my normal grid, particles and index them accordingly.
/// @brief This is also the class I will use for all my particle calculations and updates
/// @author Declan Russell
/// @version 1.0
/// @date 15/11/13
/// @class SPHMelt
/// @brief Our main simulation class. Used to call all the other classes we have created.
//----------------------------------------------------------------------------------------------------------------------


#include <stdlib.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <vector>
#include <list>
#include "particle.h"
#include <math.h>
#include <ngl/TransformStack.h>
#include "signed_distance_field_from_mesh.hpp"
#include <ctime>
#include "collision.h"
#include "SPHSolver.h"
#include "SPHHash.h"


class SPHMelt{
public:
    // a simple structure to hold our vertex data
    struct vertData
    {
      ngl::Vec3 p1;
      ngl::Vec3 n1;
    };
    // Simple structure to hold our external force data
    struct externalForce
    {
        ngl::Vec3 sourcePos;
        float radius;
        float strength;
        bool push;
    };
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief just a plain constructor that does nothing
    SPHMelt(){};
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our constructor, reads in the vetexes of our mesh and stores them in m_meshVerts
    SPHMelt(std::string _meshLocation);
    //----------------------------------------------------------------------------------------------------------------------
    ~SPHMelt();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our update function. Update particles with SPH, Collision and then rewrites to the VAO
    void update();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A functio to draw our mesh
    void drawMesh();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to create our particles from our mesh vertecies
    /// @param _numParticles - The number of particles we want to create. If more than mesh verticies it will generate particles inside the mesh
    /// @warning You must initialise the mesh first
    void genParticles(int _numParticles, float _mass);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Just a fucntion so the scene can queury how many particles so that we can write it to screen
    inline int getNumParticles(){return m_particles.size();}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Initialize our simulation. Set up VAO, generate particles, add collision walls and start the clock
    void init();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief we need a special function to convert the mesh vec3's to our vec3*
    /// @param _vertex - Takes in the obj verticies
    void setMeshVerts(std::vector<ngl::Vec3> _vertex);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the scenes camera for all our particles
    /// @param Our scenes particles
    void setCam(ngl::Camera *_c);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calls draw function in all our particles
    /// @param trans - the transformstack of our scene
    void drawParticles(ngl::TransformStack trans);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief reset our clock. used if we pause the simulation
    inline void resetClock(){m_startTime = clock();}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a fucntion to set the parameters of our external force for our SPH solver;
    inline void setExternalForce(ngl::Vec3 _sourcePos = ngl::Vec3(0), float _radius = 0, float _forceStrength = 0,bool _push = true)
    {m_externalForce.sourcePos = _sourcePos; m_externalForce.radius = _radius; m_externalForce.strength = _forceStrength; m_externalForce.push = _push;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to draw the source of our external force.
    /// @param _transformStack - Our global transform stack so that they can rotate with the scene
    /// @param _cam - our scene camera so we can rotate with the scene
    void drawExternalForce(ngl::TransformStack * _transformStack,ngl::Camera * _cam);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to draw the walls of our collision. Just used to pass paramiters to our collision class
    /// @param _transformStack - Our global transform stack so that they can rotate with the scene
    /// @param _cam - our scene camera so we can rotate the walls with the scene
    inline void drawCollision(ngl::TransformStack * _transformStack,ngl::Camera * _cam){m_collisionSolver.drawWalls(_transformStack, _cam);};
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief restarts the mesh simulation.
    void startMeshSim();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A function to toggle between our mesh simulation and our 2d simulation;
    void toggleSim();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief restarts the simumaltion
    void restart();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief change the scene to a 2d water simulation so you can see the physics
    void twoDWaterSimStart();
    //----------------------------------------------------------------------------------------------------------------------

private:
    /// @brief vectors to store our mesh information from our obj.
    std::vector<ngl::Vec3 *> m_meshVerts;
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<ngl::Face> m_meshFaces;
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<ngl::Vec3> m_meshNormals;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a vector to store particles created from the information in m_meshVerts
    std::vector<particle *> m_particles;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Vertex Array Object to hold our drawing data
    ngl::VertexArrayObject *m_meshVAO;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A member to store our mesh
    ngl::Obj *m_mesh;
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <vertData> m_vboMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the location of our mesh
    std::string m_meshLocation;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the start time of our simulation so that we do our pysics calculations
    int m_startTime;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the last update time of our simulation so that we can find our timestep and do our pysics calculations
    float m_lastUpdateTime;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our collision solver class
    collision m_collisionSolver;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our SPH solver class
    SPHSolver m_SPHSolver;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our SPH Hashing class
    SPHHash m_SPHHash;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Amemeber for our external force used in our SPH calculations
    externalForce m_externalForce;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A member so we know what sim we are running
    bool m_meshSim;
    //----------------------------------------------------------------------------------------------------------------------


};

#endif // SPHMELT_H
