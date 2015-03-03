#include "SPHHash.h"

SPHHash::SPHHash(){
    m_smoothingLength = 1;
    m_primes[0] = 73856093;
    m_primes[1] = 19349663;
    m_primes[2] = 83492791;
}


SPHHash::~SPHHash(){}


void SPHHash::createHashTable(std::vector<particle *> _particles){
    // clear any previous data we have
    m_hashTable.clear();
    // Hash all our particles
    m_hashTableSize = nextPrimeNum(2*_particles.size());
    for(unsigned int i=0; i<_particles.size(); i++){
        m_hashTable.insert(std::pair<int,particle*>((hashFunction(_particles[i])),_particles[i]));
    }
}

std::vector<particle*> SPHHash::getNeighbours(particle *_currentParticle){
    // calculate the key for our particle
    int key = hashFunction(_currentParticle);
    std::vector<particle*> neighbours;
    std::pair<std::multimap<int,particle*>::iterator,std::multimap<int,particle*>::iterator> foundParticles;
    //find all the particles with the same key
    foundParticles = m_hashTable.equal_range(key);
    //Push back all found particles into and vector
    std::multimap<int,particle*>::iterator it;
    for(it = foundParticles.first; it!=foundParticles.second; ++it){
        neighbours.push_back(it->second);
    }
    return neighbours;
}
std::vector<particle*> SPHHash::getNeighbours(particle *_currentParticle, int numParticles){
    // calculate the key for our particle
    int key = hashFunction(_currentParticle);
    std::vector<particle*> neighbours;
    std::pair<std::multimap<int,particle*>::iterator,std::multimap<int,particle*>::iterator> foundParticles;
    //find all the particles with the same key
    foundParticles = m_hashTable.equal_range(key);
    std::multimap<int,particle*>::iterator it;
    //Push back all found particles into and vector until we hit our sample limit
    int samples = 0;
    for(it = foundParticles.first; it!=foundParticles.second && samples<numParticles; ++it, samples++){
        neighbours.push_back(it->second);
    }
    return neighbours;
}

int SPHHash::hashFunction(particle *_particle){
    int x_x = floor(_particle->getPos().m_x/m_smoothingLength);
    int x_y = floor(_particle->getPos().m_y/m_smoothingLength);
    int x_z = floor(_particle->getPos().m_z/m_smoothingLength);
    return (x_x*m_primes[0]^x_y*m_primes[1]^x_z*m_primes[2]) & m_hashTableSize;
}

int SPHHash::nextPrimeNum(int _currentNum){
    int nextPrime = _currentNum;
    bool Prime = false;
    if(_currentNum<=0){
        std::cerr<<"The number input is less than or equal to zero"<<std::endl;
        return 1;
    }
    if(_currentNum==2){
        return 2;
    }
    if((_currentNum % 2 ) == 0){
        nextPrime+=1;
    }
    while(!Prime){
        Prime = true;
        for(int i = 3; i<sqrt(nextPrime); i+=2){
            if((nextPrime % i)==0){
                Prime = false;
            }
        }
        if(!Prime){
            nextPrime+=2;
        }
    }
    return nextPrime;
}
