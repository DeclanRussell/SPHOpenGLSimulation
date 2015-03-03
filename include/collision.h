#ifndef COLLISION_H__
#define COLLISION_H__

//----------------------------------------------------------------------------------------------------------------------
/// @file collision
/// @brief This is my collision class, It will be used to test collisions of particles and update accordingly.
/// @author Declan Russell
/// @version 1.0
/// @date 25/02/14
/// @class collision
/// @brief Used to calculate collisions from particles
//----------------------------------------------------------------------------------------------------------------------

#include "ngl/Vec3.h"
#include <vector>
#include "particle.h"



class collision
{
public:
    /// @brief default ctor
    collision();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default dtor
    ~collision(){};
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A functioin to run through all of the walls and test a selected particle for collision
    /// @param _testparticle The particle that we wish to test
    /// @param _timestep used in calculating the friction against the wall
    void testCollisionWithWalls(particle* _testParticle, float _timeStep);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Particle and particle reflected collision, implemented but does not run default.
    /// @param _testParticle the particle we wish to test for
    /// @param _neighbours the partciles we wish to test our particle against
    /// @warning advised to run init particle collision first so that we dont have partciles inside each other, can cause errors
    void testCollisionWithParticles(particle* _testParticle, std::vector<particle *> _neighbours);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Add a wall to our scene
    /// @param _normal the direction we wish for our wall to point in
    /// @param _center the postion we wish for our wall to be
    /// @param _draw a bool for if we want to draw
    /// @param _drawSizeX how big we want to draw our wall in X scale
    /// @param _drawSizeY how big we want to draw our wall in Y scale
    /// @param _drawSizeZ how big we want to draw our wall in Z scale
    void addWall(ngl::Vec3 _normal, ngl::Vec3 _center, bool _draw, float _drawSizeX, float _drawSizeY, float _drawSizeZ);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief If particles are inside each other moves them apart
    /// @param _testParticle the particle we want to test
    /// @param _particles our array of particles we wish to test against
    void initParticleCollision(particle* _testParticle, std::vector<particle *> _particles);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Set the coefficient of restitution for our walls
    /// @param _x Value we wish to set to
    inline void setCoefOfRest(float _x){m_coefOfRest = _x;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Set the coefficient of friction for our walls
    /// @param _x Value we wish to set to
    inline void setCoefOfFric(float _x){m_coefOfFric = _x;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief draws our walls to the scene
    /// @param _trans Our global transformation stack of our scene so we can retrieve model matrix
    /// @param _cam The camera of our scene to retrieve view and perspective matrix
    void drawWalls(ngl::TransformStack *_trans, ngl::Camera *_cam);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief finds the rotation for Y of a vector
    /// @param _vec the vector we wish to find the rotation from Y
    ngl::Vec3 getRotationFromY(ngl::Vec3 _vec) const;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A mutator to change the position of our walls
    /// @param _wall the wall in our array we wish to change
    /// @param _normal the normal we wish to change it to
    /// @param _center the center of the wall we wish to change to
    void setWall(unsigned int _wall, ngl::Vec3 _normal = ngl::Vec3(0), ngl::Vec3 _center = ngl::Vec3(0), bool _draw = true, float _drawSizeX = 0, float _drawSizeY = 0, float _drawSizeZ = 0);

protected:
    /// @brief calculates and updates a particles position and velocity depending on if it has a collision with the ground
    /// @param _testParticle the particle we wish to update
    /// @param _timeStep the time step between our calcuations, this is used in calculting the friction
    void calculateWallCollision(particle* _testParticle, float _timeStep, ngl::Vec4 _ground);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculates and updates a particles position and velocity depending on if it has a collision with other particles
    /// @param _testParticle the particle we wish to update
    /// @param _neighbours the time step between our calcuations, this is used in calculting the friction
    /// @warning collision init should be called or you could get errors with calculation as particles could be spawned within particles
    void calculateParticleCollision(particle* _testParticle, std::vector<particle *> _neighbours);
    //----------------------------------------------------------------------------------------------------------------------

private:

    /// @brief Struct to store our wall data neatly
    typedef struct wall{
        ngl::Vec3 center;
        ngl::Vec4 plane;
        float drawSizeX;
        float drawSizeY;
        float drawSizeZ;
        bool draw;
    }wall;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A vector to store all our walls
    std::vector<wall> m_walls;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the coefficient of restitution i.e. how much the particles bouce, it 1 then 0 bounce
    float m_coefOfRest;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the coefficient of friction
    float m_coefOfFric;

};




#endif
