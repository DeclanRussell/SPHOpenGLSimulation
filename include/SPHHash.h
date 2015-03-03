#ifndef SPHHASH_H__
#define SPHHASH_H__

//----------------------------------------------------------------------------------------------------------------------
/// @file SPHHash.h
/// @brief This is my SPHHash class, it will be used to calulate the hash keys from particle positions.
/// @author Declan Russell
/// @version 1.0
/// @date 2/3/14
/// @class SPHHash
/// @brief Our hash class. this class will hash particles based on their position and return neighbours of the same key
//----------------------------------------------------------------------------------------------------------------------

#include "ngl/Vec3.h"
#include <vector>
#include <map>
#include "particle.h"

class SPHHash{
public:
    /// @brief default constructor
    SPHHash();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief default destructor
    ~SPHHash();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief mutator for our prime numbers if we want to set them to somthing else
    /// @param _arrayPos - position of the array that holds the 3 prime numbers
    /// @param _prime - the number you want to set it to
    inline void setPrime(int _arrayPos, double _prime){if (_arrayPos>=0 && _arrayPos<3){ m_primes[_arrayPos] = _prime;}}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor for our prime numbers if we want to query them
    /// @param _arrayPos - the position of the array that holds the 3 prime numbers
    inline int getPrime(int _arrayPos){if (_arrayPos>=0 && _arrayPos<3){ return m_primes[_arrayPos];} return EXIT_FAILURE;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A mutatator for our smoothing length, h variable in navier stokes eqations
    /// @param _smoothingLength - the smoothing length
    inline void setSmoothingLength(float _smoothingLength){m_smoothingLength = _smoothingLength;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns our Smoothing Length if we need to query it
    inline float getSmoothingLength(){return m_smoothingLength;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a function to fill our initially fill our hash table wil our particles
    /// @param _particles - the vector of our particles
    void createHashTable(std::vector<particle*> _particles);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Returns a vector of a particles neighbours
    /// @param _currentParticle - the particle we want to ind the neighbours of
    /// @return vector of particle neighbours to our current particles
    std::vector<particle*> getNeighbours(particle* _currentParticle);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Returns a vector of a limited number of particles neighbours, used for optimization
    /// @param _currentParticle - the particle we want to ind the neighbours of
    /// @param numParticles - the max number of particles you want to sample
    /// @return numParticle amount or neighbours to our current particle
    std::vector<particle*> getNeighbours(particle* _currentParticle, int numParticles);
    //----------------------------------------------------------------------------------------------------------------------


protected:
    /// @brief Hash function that generates a key based on the particle position
    /// @param _particle - the particle we want to create the key for
    int hashFunction(particle * _particle);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief A simple function that returns the next prime number from an input number. This is used in calculating our hash
    /// @param _currentNum - the number you want to calclate for
    int nextPrimeNum(int _currentNum);

private:
    int m_primes[3];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief stores the smoothing length of our simulation
    float m_smoothingLength;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a multimap to store our particles and their hashed keys
    std::multimap<int,particle*> m_hashTable;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief store the size of our hash table
    int m_hashTableSize;
    //----------------------------------------------------------------------------------------------------------------------
};










#endif
