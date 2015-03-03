#ifndef SPHSOLVER_H__
#define SPHSOVLER_H__

//----------------------------------------------------------------------------------------------------------------------
/// @file SPHMelt.h
/// @brief This is my SPHSolver class, this will be used to calculate the forces of a particle with the navier stokes eqations
/// @author Declan Russell
/// @version 1.0
/// @date 28/03/14
/// @class SPHSolver
/// @brief Our solver class used to calculate our new particle positions with SPH equations
//----------------------------------------------------------------------------------------------------------------------


#include "particle.h"
#include "ngl/Vec3.h"
#include <vector>
#include "cmath"
class SPHSolver{
    public:
    /// @brief default ctor
    SPHSolver();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default dtor
    ~SPHSolver();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Calculates the forces for a particle using leap frog method of integration
    /// @param _currentParticle - The particle we are going to calculate the forces for
    /// @param _particleIndex - Vector of all our particles if we want to brut force or the neighbour particles for SPH
    /// @param _timeStep - the time step between updates
    void calcForces(particle* _currentParticle, std::vector<particle*> _particleIndex, float _timeStep);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Calculates the forces for a particle using Euler integration
    /// @param _currentParticle - The particle we are going to calculate the forces for
    /// @param _particleIndex - Vector of all our particles if we want to brut force or the neighbour particles for SPH
    /// @param _timeStep - the time step between updates
    void calcForcesEuler(particle* _currentParticle, std::vector<particle*> _particleIndex, float _timeStep);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A mutatator for our smoothing length, h variable in navier stokes eqations
    /// @param _smoothingLength - the smoothing length
    inline void setSmoothingLength(float _smoothingLength){m_smoothingLength = _smoothingLength;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns our Smoothing Length if we need to query it
    inline float getSmoothingLength(){return m_smoothingLength;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Set our viscosity coefficient for our navier stokes equations
    inline  void setVisCoef(float _x){m_viscosityCoefficient = _x;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Initialises the density for our particles
    /// @param _particleIndex - The vector of all our particles we want to initialize the density for
    void initDensity(std::vector<particle*> _particleIndex);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set the gas constant for our water simulation
    /// @param _x - the value we wish to set our constant to
    inline void setGasConstant(float _x){m_gasConstant = _x;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Add an external force to our SPH calculations. acts like wind
    /// @param _pos - The source position of the force
    /// @param _forceRadius - the area that the force has influence over. This has a linear falloff
    /// @param _forceStrength - How strong your want your force to be. This is just a scaler.
    /// @param _push - a bool so we know weather we are pushing the particles away or sucking them towards us
    inline void setExternalForce(ngl::Vec3 _pos, float _forceRadius, float _forceStrength, bool _push = true)
    {m_externalForcePos = _pos; m_externalForceRadius = _forceRadius; m_externalForceStrenth = _forceStrength; m_externalForcePush = _push;}
    //----------------------------------------------------------------------------------------------------------------------
    protected:
    /// @brief calculates the smoothing kernal for density, used in our SPH equations
    /// @param _currentParticle the particle we wish to test for
    /// @param _neighbour the particle we wish to test our _current particle against
    /// @return the weight of influence the particle has on our _currentParticle
    float calcDensityKern(particle* _currentParticle, particle* _neighbour);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculates the smoothing kernal for density, used in our SPH equations
    /// @param _currentParticle the particle we wish to test for
    /// @param _neighbour the particle we wish to test our _current particle against
    /// @return the weighted pressued gradient the particle has on our _currentParticle
    ngl::Vec3 calcPressureKern(particle* _currentParticle, particle* _neighbour);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculates the smoothing kernal for density, used in our SPH equations
    /// @param _currentParticle the particle we wish to test for
    /// @param _neighbour the particle we wish to test our _current particle against
    /// @return the weighted viscosity vector the particle has on our _currentParticle
    ngl::Vec3 calcViscosityKern(particle* _currentParticle, particle* _neighbour);

    private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the smoothing length of our simulation
    float m_smoothingLength;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the viscosity coefficient of our simulation
    float m_viscosityCoefficient;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the gas constant for our simulation
    float m_gasConstant;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the external force postion used in our SPH calculations
    ngl::Vec3 m_externalForcePos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the external force influence radius used in our SPH calculations
    float m_externalForceRadius;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Store the external force strength scaler used in our SPH calculations
    float m_externalForceStrenth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Bool to store if we are pushing or pulling with our external force
    bool m_externalForcePush;
    //----------------------------------------------------------------------------------------------------------------------








};














#endif
