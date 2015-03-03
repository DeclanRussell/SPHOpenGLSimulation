#include "SPHSolver.h"
#define pi 3.1415926535897

SPHSolver::SPHSolver(){
    m_smoothingLength = 1;
    m_viscosityCoefficient = 0;
    m_externalForcePos = ngl::Vec3(0);
    m_externalForceRadius = 0;
    m_externalForceStrenth = 0;
    m_externalForcePush = true;
}
SPHSolver::~SPHSolver(){

}

void SPHSolver::calcForces(particle *_currentParticle, std::vector<particle *> _particleIndex, float _timeStep){
    float densityCurrent = 0;
    ngl::Vec3 gravitiy(0,-9.8,0);
    ngl::Vec3 pressureGrad(0);
    ngl::Vec3 viscosity(0);
    ngl::Vec3 viscosityVector(0);
    ngl::Vec3 Acceleration(0);
    for(unsigned int i=0; i<_particleIndex.size();i++){
    // Calculate desity of all particles
        if(_currentParticle!=_particleIndex[i]){
            densityCurrent += _particleIndex[i]->getMass()*calcDensityKern(_currentParticle, _particleIndex[i]);
        }
    }
    _currentParticle->setDensity(densityCurrent);
    ngl::Vec3 neighbourPartPos;
    ngl::Vec3 pressTemp(0), visTemp(0),surfaceTensionForce(0), surfaceTensionDirection(0);
    float pressureCurrent = 0, pressureNeighbour = 0, pressure = 0;
    for(unsigned int i=0; i<_particleIndex.size();i++){
        neighbourPartPos = _particleIndex[i]->getPos();
    // Calcualate pressure
        pressTemp = 0;
        pressureCurrent = 0;
        pressureNeighbour = 0;
        pressureCurrent = m_gasConstant* (_currentParticle->getDensity() - _currentParticle->getRestDensity());
        pressureNeighbour = m_gasConstant* (_particleIndex[i]->getDensity() - _particleIndex[i]->getRestDensity());
        pressure=((pressureCurrent /(pressureCurrent*pressureCurrent)) + (pressureNeighbour /(pressureNeighbour*pressureNeighbour)))*(_particleIndex[i]->getMass());
        pressTemp = calcPressureKern(_currentParticle,_particleIndex[i]);
        pressTemp.operator *(pressure);
        pressureGrad.operator +=(pressTemp);
    // Calculate viscosiy vector
        viscosityVector = (_particleIndex[i]->getVel().operator -( _currentParticle->getVel()));
        viscosityVector.operator *(_particleIndex[i]->getMass());
        viscosityVector.operator /(_particleIndex[i]->getDensity());
    // Calculate viscosiy
        visTemp=calcViscosityKern(_currentParticle,_particleIndex[i]);
        visTemp.operator *(viscosityVector);
        viscosity.operator +=(visTemp);
    }
    viscosity.operator *(m_viscosityCoefficient);
    //Calcualate external force if the is one
    ngl::Vec3 Forces(0);
    if(m_externalForceStrenth!=0&&m_externalForceRadius!=0){
        Forces = _currentParticle->getPos().operator -(m_externalForcePos);
        if (Forces.length()<=m_externalForceRadius && Forces.length()>0){
            //find the direction the force is in
            Forces.normalize();
            Forces.operator *=((m_externalForceRadius-Forces.length())/m_externalForceRadius);
            Forces.operator *=(m_externalForceStrenth);
            if(!m_externalForcePush){
                Forces.operator *=(-1.0);
            }
        }
        else{
            Forces.set(ngl::Vec3(0));
        }
    }

    // Calculate our acceleration
    Acceleration = gravitiy - pressureGrad + viscosity + Forces;

    //---------------leap frog integration------------------------
    //Calculate velocity
    ngl::Vec3 VelHalfBack = _currentParticle->getVel()-(_currentParticle->getAcc()*(_timeStep/2));
    ngl::Vec3 VelHalfForward = VelHalfBack + Acceleration.operator *(_timeStep);
    ngl::Vec3 Velocity = (VelHalfBack.operator +(VelHalfForward)).operator /(2);
    //Calculate our new position
    ngl::Vec3 Position = _currentParticle->getPos() + VelHalfForward.operator *(_timeStep);
    _currentParticle->setVel(Velocity);
    _currentParticle->setPos(Position);

//---------------Debuging----------------
//    std::cout<<"the viscosity is "<<"["<<viscosity.m_x<<","<<viscosity.m_y<<","<<viscosity.m_z<<"]"<<std::endl;
//    std::cout<<"the pressure grad is "<<"["<<pressureGrad.m_x<<","<<pressureGrad.m_y<<","<<pressureGrad.m_z<<"]"<<std::endl;
//    std::cout<<"the accelleration is "<<"["<<Acceleration.m_x<<","<<Acceleration.m_y<<","<<Acceleration.m_z<<"]"<<std::endl;
//    std::cout<<"the velocity is "<<"["<<Velocity.m_x<<","<<Velocity.m_y<<","<<Velocity.m_z<<"]"<<std::endl;

}

void SPHSolver::calcForcesEuler(particle *_currentParticle, std::vector<particle *> _particleIndex, float _timeStep){
    float densityCurrent = 0;
    ngl::Vec3 gravitiy(0,-9.8,0);
    ngl::Vec3 pressureGrad(0);
    ngl::Vec3 viscosity(0);
    ngl::Vec3 viscosityVector(0);
    ngl::Vec3 Acceleration(0);
    for(unsigned int i=0; i<_particleIndex.size();i++){
    // Calculate desity of all particles
        if(_currentParticle!=_particleIndex[i]){
            densityCurrent += _particleIndex[i]->getMass()*calcDensityKern(_currentParticle, _particleIndex[i]);
        }
    }
    //std::cout<<"the desity is "<<_currentParticle->getDensity()<<std::endl;
    _currentParticle->setDensity(densityCurrent);
    ngl::Vec3 neighbourPartPos;
    ngl::Vec3 pressTemp(0), visTemp(0);
    float pressureCurrent = 0, pressureNeighbour = 0, pressure = 0;
    for(unsigned int i=0; i<_particleIndex.size();i++){
        neighbourPartPos = _particleIndex[i]->getPos();
    // Calcualate pressure
        pressTemp = 0;
        pressureCurrent = 0;
        pressureNeighbour = 0;
        pressureCurrent = m_gasConstant* (_currentParticle->getDensity() - _currentParticle->getRestDensity());
        pressureNeighbour = m_gasConstant* (_particleIndex[i]->getDensity() - _particleIndex[i]->getRestDensity());
        pressure=((pressureCurrent /(pressureCurrent*pressureCurrent)) + (pressureNeighbour /(pressureNeighbour*pressureNeighbour)))*(_particleIndex[i]->getMass());
        pressTemp = calcPressureKern(_currentParticle,_particleIndex[i]);
        pressTemp.operator *(pressure);
        pressureGrad.operator +=(pressTemp);
    // Calculate viscosiy vector
        viscosityVector = (_particleIndex[i]->getVel().operator -( _currentParticle->getVel()));
        viscosityVector.operator *(_particleIndex[i]->getMass());
        viscosityVector.operator /(_particleIndex[i]->getDensity());
    // Calculate viscosiy
        visTemp=calcViscosityKern(_currentParticle,_particleIndex[i]);
        visTemp.operator *(viscosityVector);
        viscosity.operator +=(visTemp);
    }
    viscosity.operator *(m_viscosityCoefficient);
    //Calcualate external force if the is one
    ngl::Vec3 Forces(0);
    if(m_externalForceStrenth!=0&&m_externalForceRadius!=0){
        std::cout<<"here"<<std::endl;
        Forces = _currentParticle->getPos().operator -(m_externalForcePos);
        if (Forces.length()<=m_externalForceRadius && Forces.length()>0){
            Forces.normalize();
            Forces.operator *=((m_externalForceRadius-Forces.length())/m_externalForceRadius);
            Forces.operator *=(m_externalForceStrenth);
        }
        else{
            Forces.set(ngl::Vec3(0));
        }
    }

    // Calculate our acceleration
    Acceleration = gravitiy - pressureGrad + viscosity + Forces;
    //--------------euler integration--------------------
    ngl::Vec3 Velocity = _currentParticle->getVel().operator +( Acceleration.operator *(_timeStep) );
    ngl::Vec3 Position = _currentParticle->getPos().operator +( Velocity.operator *(_timeStep) );

    _currentParticle->setVel(Velocity);
    _currentParticle->setPos(Position);


//-------------Debugging-------------
//    std::cout<<"the viscosity is "<<"["<<viscosity.m_x<<","<<viscosity.m_y<<","<<viscosity.m_z<<"]"<<std::endl;
//    std::cout<<"the pressure grad is "<<"["<<pressureGrad.m_x<<","<<pressureGrad.m_y<<","<<pressureGrad.m_z<<"]"<<std::endl;
//    std::cout<<"the accelleration is "<<"["<<Acceleration.m_x<<","<<Acceleration.m_y<<","<<Acceleration.m_z<<"]"<<std::endl;
//    std::cout<<"the velocity is "<<"["<<Velocity.m_x<<","<<Velocity.m_y<<","<<Velocity.m_z<<"]"<<std::endl;

}

void SPHSolver::initDensity(std::vector<particle *> _particleIndex){
    float densityCurrent = 0.0;
    std::cout<<"index size"<<_particleIndex.size()<<std::endl;
    for(unsigned int i=0; i<_particleIndex.size();i++){
    // Calculate desity of all particles
        for(unsigned int j=0; j<_particleIndex.size();j++){
            if(i!=j){
                densityCurrent += _particleIndex[j]->getMass()*calcDensityKern(_particleIndex[i], _particleIndex[j]);
            }
        }
        //We cant have 0 densities or it will effect out calculations this is fine for bruce mesh but for anything arbetory if could make problems
        //this isnt precise but for approximations it will do
        if(densityCurrent==0.0) densityCurrent = 1.0;

        //Set the densties for our particle
        _particleIndex[i]->setDensity(densityCurrent);
        _particleIndex[i]->setRestDensity(densityCurrent);

        //reinit our temp density for next loop
        densityCurrent=0.0;
    }
}

float SPHSolver::calcDensityKern(particle *_currentParticle, particle *_neighbour){
    ngl::Vec3 r = _currentParticle->getPos().operator -(_neighbour->getPos());
    if(r.length()>m_smoothingLength){
        return 0;
    }
    float densityKern = (315/(64*pi*pow(m_smoothingLength,9))) * pow(((m_smoothingLength*m_smoothingLength) - (r.length()*r.length())),3);
    return densityKern;
}
ngl::Vec3 SPHSolver::calcPressureKern(particle *_currentParticle, particle *_neighbour){
    ngl::Vec3 r = _currentParticle->getPos().operator -(_neighbour->getPos());
    if (r.length()>m_smoothingLength){
        return ngl::Vec3(0);
    }
    float pressureKern = -(945/(32*pi*pow(m_smoothingLength,9))) * pow(((m_smoothingLength*m_smoothingLength) - (r.length()*r.length())),3);
    return r.operator *(pressureKern);

}
ngl::Vec3 SPHSolver::calcViscosityKern(particle *_currentParticle, particle *_neighbour){
    ngl::Vec3 r = _currentParticle->getPos().operator -(_neighbour->getPos());

    if(r.length()>m_smoothingLength){
        return ngl::Vec3(0);
    }
    float viscosityKern = -(945/(32*pi*pow(m_smoothingLength,9))) * pow(((m_smoothingLength*m_smoothingLength) - (r.length()*r.length())),3) * ((3*(m_smoothingLength*m_smoothingLength)) - 7*(m_smoothingLength*m_smoothingLength));
    return r.operator *(viscosityKern);
}
