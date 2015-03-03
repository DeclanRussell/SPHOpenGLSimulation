#include "collision.h"
#include <math.h>

collision::collision(){
    m_coefOfRest = 1;
    m_coefOfFric = 1;
}



void collision::initParticleCollision(particle *_testParticle, std::vector<particle *> _particles){
    ngl::Vec3 newVelP, newVelQ, newPosP, newPosQ, A, B, currentPosQ, currentVelQ,currentPosP, currentVelP,CollisionPosP,CollisionPosQ, N;
    float radius = _testParticle->getRadius();

    currentPosP = _testParticle->getPos();
    currentVelP = _testParticle->getVel();
        for (unsigned int i=0; i<_particles.size();i++){
            N = _testParticle->getPos().operator -(_particles[i]->getPos());
            if (N.length()>0&&N.length()<radius){
                    currentPosP = _testParticle->getPos();
                    currentPosQ = _particles[i]->getPos();
                    newPosP = (currentPosP.operator +(N.operator *(0.5)));
                    newPosQ = (currentPosQ.operator -(N.operator *(0.5)));
                    _testParticle->setPos(newPosP);
                    _particles[i]->setPos(newPosQ);
            }
    }
}

void collision::addWall(ngl::Vec3 _normal, ngl::Vec3 _center, bool _draw, float _drawSizeX, float _drawSizeY, float _drawSizeZ){
    wall tempWall;
    tempWall.draw = _draw;
    if (_draw){
        tempWall.drawSizeX = _drawSizeX;
        tempWall.drawSizeY = _drawSizeY;
        tempWall.drawSizeZ = _drawSizeZ;
    }
    tempWall.center = _center;
    tempWall.plane = ngl::Vec4(_normal.m_x,_normal.m_y,_normal.m_z,_center.m_x*_normal.m_x + _center.m_y*_normal.m_y + _center.m_z*_normal.m_z);
    m_walls.push_back(tempWall);
}

void collision::setWall(unsigned int _wall, ngl::Vec3 _normal, ngl::Vec3 _center,bool _draw, float _drawSizeX, float _drawSizeY, float _drawSizeZ){
    if (_wall>= m_walls.size()){
        std::cerr<<"wall not in range"<<std::endl;
        return;
    }
    if(_normal.length()!=0){

        m_walls[_wall].center = _center;
        m_walls[_wall].draw = _draw;
        m_walls[_wall].plane = ngl::Vec4(_normal.m_x,_normal.m_y,_normal.m_z,_center.m_x*_normal.m_x + _center.m_y*_normal.m_y + _center.m_z*_normal.m_z);
        m_walls[_wall].drawSizeX = _drawSizeX;
        m_walls[_wall].drawSizeY = _drawSizeY;
        m_walls[_wall].drawSizeZ = _drawSizeZ;
    }
    else{
        m_walls[_wall].center = _center;
        m_walls[_wall].draw = _draw;
        m_walls[_wall].plane = ngl::Vec4(m_walls[_wall].plane.m_x,m_walls[_wall].plane.m_y,m_walls[_wall].plane.m_z,_center.m_x*_normal.m_x + _center.m_y*_normal.m_y + _center.m_z*_normal.m_z);
        m_walls[_wall].drawSizeX = _drawSizeX;
        m_walls[_wall].drawSizeY = _drawSizeY;
        m_walls[_wall].drawSizeZ = _drawSizeZ;
    }
}

ngl::Vec3 collision::getRotationFromY(ngl::Vec3 _vec) const
{
    ngl::Vec3 rot;
    rot.m_z = 0.0;
    if(fabs(_vec.m_y)< 0.0001)
    {
        if (_vec.m_z>= 0.0)
            rot.m_x = -90;
        else
            rot.m_x = 90;
    }
    else
        rot.m_x = atan(_vec.m_z/_vec.m_y);
    if(fabs(_vec.m_y) + fabs(_vec.m_z) < 0.0001)
    {
        if(_vec.m_x > 0)
            rot.m_y = -90;
        else
            rot.m_y = 90;
    }
    else
        rot.m_z = atan(_vec.m_x/sqrt(_vec.m_y*_vec.m_y + _vec.m_z*_vec.m_z));

    return rot;
}

void collision::drawWalls(ngl::TransformStack *_trans, ngl::Camera * _cam){
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["nglColourShader"]->use();
    for(unsigned int i=0; i<m_walls.size(); i++){
        if(m_walls[i].draw){
            _trans->pushTransform();
            {
                _trans->setPosition(m_walls[i].center.m_x,m_walls[i].center.m_y,m_walls[i].center.m_z);
                _trans->setScale(m_walls[i].drawSizeX,m_walls[i].drawSizeY, 30);
                _trans->setRotation(getRotationFromY(ngl::Vec3(m_walls[i].plane.m_x,m_walls[i].plane.m_y,m_walls[i].plane.m_z)));
                ngl::Mat4 MVP= _trans->getCurrAndGlobal().getMatrix()*_cam->getVPMatrix();
                shader->setShaderParamFromMat4("MVP",MVP);
                shader->setShaderParam4f("Colour",0,0,0,1);
                prim->draw("wall");
            }
            _trans->popTransform();
        }
    }
}

void collision::testCollisionWithWalls(particle *_testParticle, float _timeStep){
    //for all our walls calculate collision
    for(unsigned int i=0; i<m_walls.size();i++){
        calculateWallCollision(_testParticle,_timeStep,m_walls[i].plane);
    }
}

void collision::testCollisionWithParticles(particle *_testParticle, std::vector<particle *> _neighbours){
    ngl::Vec3 normal;
    float radius = _testParticle->getRadius();
    //check if the particle is intersecting walls, if so dont do anything
    for(unsigned int i=0; i<m_walls.size();i++){
        normal = ngl::Vec3(m_walls[i].plane.m_x,m_walls[i].plane.m_y,m_walls[i].plane.m_z);
        if (_testParticle->getPos().dot(normal)-m_walls[i].plane.m_w < radius){
            return;
        }
    }
    //If we get here then we are successful
    calculateParticleCollision(_testParticle,_neighbours);

}

void collision::calculateParticleCollision(particle *_testParticle, std::vector<particle *> _neighbours){
        //Test with other particles
        ngl::Vec3 newVelP, newVelQ, newPosP, newPosQ, A, B, currentPosQ, currentVelQ,currentPosP, currentVelP,CollisionPosP,CollisionPosQ, N;
        float radius = _testParticle->getRadius();
        currentPosP = _testParticle->getPos();
        currentVelP = _testParticle->getVel();
        //2 times becuse parametric equation
        float time1, time2;
            for (unsigned int i=0; i<_neighbours.size();i++){
                N = _testParticle->getPos().operator -(_neighbours[i]->getPos());
                // If the particle is not moving then just serperat the particles
                if (currentVelP.length()==0){
                    if(N.length()<radius){
                        currentPosP = _testParticle->getPos();
                        currentPosQ = _neighbours[i]->getPos();
                        newPosP = (currentPosP + N.operator *(0.5*radius));
                        newPosQ = (currentPosQ - N.operator *(0.5*radius));
                        _testParticle->setPos(newPosP);
                        _neighbours[i]->setPos(newPosQ);
                    }
                }
                else if(_testParticle!=_neighbours[i]){
                    if(N.length()<radius && N.length()!=0){
                        currentPosQ = _neighbours[i]->getPos();
                        currentVelQ = _neighbours[i]->getVel();
                        N.normalize();
                        A = (currentPosP-currentVelP)-(currentPosQ-currentVelQ);
                        B = (currentVelP-currentVelQ);
                        time1 =  -(A.dot(B))  +  sqrt( (A.dot(B)*A.dot(B))  -   (B.dot(B)*B.dot(B))   *    (   A.dot(A) -  ((radius + radius)* (radius + radius))  ) );
                        time1 = time1 / (B.dot(B)*B.dot(B));
                        time2 =  -(A.dot(B))  -  sqrt( (A.dot(B)*A.dot(B))  -   (B.dot(B)*B.dot(B))   *    (   A.dot(A) -  ((radius + radius)* (radius + radius))  ) );
                        time2 = time1 / (B.dot(B)*B.dot(B));

                        newVelP = (currentVelP - N.operator *(currentVelP.dot(N))) + N.operator *(currentVelQ.dot(N));
                        newVelQ = (currentVelQ - N.operator *(currentVelQ.dot(N))) + N.operator *(currentVelP.dot(N));

                        _testParticle->setVel(newVelP);
                        _neighbours[i]->setVel(newVelQ);

                        if((time1<=1)&&(time1>=0)){
                            CollisionPosP = currentPosP-currentVelP.operator *((1-time1));
                            CollisionPosQ = currentPosQ-currentVelQ.operator *((1-time1));
                            _testParticle->setPos(CollisionPosP+currentVelP.operator *(1-time1));
                            _neighbours[i]->setPos(CollisionPosQ+currentVelQ.operator *(1-time1));
                        }
                        else if((time1<=1)&&(time1>=0)){
                            CollisionPosP = currentPosP-currentVelP.operator *((1-time2));
                            CollisionPosQ = currentPosQ-currentVelQ.operator *((1-time2));
                            _testParticle->setPos(CollisionPosP+currentVelP.operator *(1-time2));
                            _neighbours[i]->setPos(CollisionPosQ+currentVelQ.operator *(1-time2));
                        }
                    }
                }
            }

}

void collision::calculateWallCollision(particle *_testParticle, float _timeStep, ngl::Vec4 _ground){
//    std::cout<<"pointer is:"<<_testParticle<<std::endl;
    ngl::Vec3 currentPos = _testParticle->getPos();
    ngl::Vec3 currentVel = _testParticle->getVel();
    float radius = _testParticle->getRadius();
    ngl::Vec3 Vel = _testParticle->getVel(), Pos = _testParticle->getPos();
    ngl::Vec3 normal = ngl::Vec3(_ground.m_x,_ground.m_y,_ground.m_z);
    normal.normalize();
    //Test with ground
    if ((currentPos.dot(normal)-_ground.m_w<radius)){
        //-----------------Calculate Velocity------------------
        //Calculate our new velocity with momentum included
        Vel = -(currentVel.dot(normal))*normal+(currentVel-(currentVel*normal)*normal);
        Vel+= m_coefOfRest*(currentVel.dot(normal))*normal;

        //If moving parallel to our plane decrease speed due to friction unless it has already stopped
        if(currentVel.length()>0 && currentVel.dot(normal)==0){
            ngl::Vec3 VelNormalized(Vel);
            VelNormalized.normalize();
            VelNormalized.operator *=(-1);
            ngl::Vec3 friction((1-normal.m_x)*VelNormalized.m_x,(1-normal.m_y)*VelNormalized.m_y,(1-normal.m_z)*VelNormalized.m_z);
            friction.operator *=((m_coefOfFric*_timeStep)/(_testParticle->getMass()));
            if(friction.length()<Vel.length()){
                Vel.operator +=(friction);
            }
            else if(friction.length()>=Vel.length()){
                Vel.set(0,0,0);
            }
        }
        _testParticle->setVel(Vel);

        //---------------Calculate Position----------------------
        //If particle has a velocity which is not parallel to our plane find its new position
        if(currentVel.length()!=0||currentVel.dot(normal)!=0){
            ngl::Vec3 curPosRadius = currentPos - normal.operator *(radius);
//            float t = (_ground.m_w - normal.dot(curPosRadius)) / currentVel.dot(normal);
//            ngl::Vec3 groundIntersect = curPosRadius + currentVel.operator *(t);
            float t = (_ground.m_w - normal.dot(curPosRadius)) / normal.dot(normal);
            ngl::Vec3 closestPoint = curPosRadius + normal.operator *(t);

            if(t>0&&t<1){
//                Pos = groundIntersect + normal.operator *(radius) + Vel.operator *(1-t);
                Pos = closestPoint + normal.operator *(radius);
            }
            else{
//                Pos = groundIntersect + normal.operator *(radius);
                Pos = closestPoint + normal.operator *(radius);
            }

            _testParticle->setPos(Pos);
        }
    }


}
