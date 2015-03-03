#include "SPHMelt.h"
#include <stdio.h>
#include "signed_distance_field_from_mesh.hpp"
#include <ngl/Random.h>
#include <ngl/Util.h>



SPHMelt::SPHMelt(std::string _meshLocation){
    //Read in the information for our mesh
    m_meshLocation = _meshLocation;
    m_mesh= new ngl::Obj(_meshLocation);
    //Turn position vectors into pointers
    setMeshVerts(m_mesh->getVertexList());
    m_meshFaces = m_mesh->getFaceList();
    m_meshNormals = m_mesh->getNormalList();
    // Show our mesh simulation first
    m_meshSim = true;
}


SPHMelt::~SPHMelt(){
    // clear all our assigned memory
    for(unsigned int i=0;i<m_particles.size();i++){
        delete m_particles[i];
    }
}

void SPHMelt::setMeshVerts(std::vector<ngl::Vec3> _vertex){
    // change all the mesh verts to pointers so we can load them into our particle class
    for (unsigned int i=0; i<_vertex.size();i++){
        m_meshVerts.push_back(new ngl::Vec3(_vertex[i]));
    }
}

void SPHMelt::init(){
    vertData d;
    unsigned int nFaces=m_meshFaces.size();
    // loop for each of the faces
    for(unsigned int i=0;i<nFaces;++i)
    {
      // now for each triangle in the face (remember we ensured tri above)
      for(int j=0;j<3;++j)
      {
        // pack in the vertex data first
        d.p1=*m_meshVerts[m_meshFaces[i].m_vert[j]];
        //std::cout<<d.p1<<std::endl;
        // now do the normals
        d.n1=m_meshNormals[m_meshFaces[i].m_norm[j]];
        // finally add it to our mesh VAO structure
        m_vboMesh.push_back(d);
      }
    }
    // first we grab an instance of our VOA class as a TRIANGLE_STRIP
    m_meshVAO= ngl::VertexArrayObject::createVOA(GL_TRIANGLES);
    // next we bind it so it's active for setting data
    m_meshVAO->bind();
    unsigned int meshSize=m_vboMesh.size();
    // now we have our data add it to the VAO, we need to tell the VAO the following
    // how much (in bytes) data we are copying
    // a pointer to the first element of data (in this case the address of the first element of the
    // std::vector
    m_meshVAO->setData(meshSize*sizeof(vertData),m_vboMesh[0].p1.m_x);
    // so data is Vert / Normal for each mesh
    m_meshVAO->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(vertData),0);
    m_meshVAO->setVertexAttributePointer(1,3,GL_FLOAT,sizeof(vertData),3);
    // now we have set the vertex atm_vaoMeshtributes we tell the VAO class how many indices to draw when
    // glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
    // specify less (in steps of 3))
    m_meshVAO->setNumIndices(meshSize);
    // finally we have finished for now so time to unbind the VAO
    m_meshVAO->unbind();

    // Generate our particles if >0 then if will generate particles inside our mesh
    genParticles(5000,0.5);
    // Set the smoothing length for our hash function
    m_SPHHash.setSmoothingLength(0.5);
    //fill our hash table with our particles
    m_SPHHash.createHashTable(m_particles);

    // Initialize our SPH solver
    m_SPHSolver.setSmoothingLength(0.5);
    m_SPHSolver.initDensity(m_particles);
    m_SPHSolver.setGasConstant(2760);
    m_SPHSolver.setVisCoef(1000);
    //initiallize our external force
    setExternalForce();

    // set the coeficient of restitution between the particle and the ground
    m_collisionSolver.setCoefOfRest(0.85);
    m_collisionSolver.setCoefOfFric(0.01);
    m_collisionSolver.addWall(ngl::Vec3(1,0,0),ngl::Vec3(-15,15,0),true,30,30,30);
    m_collisionSolver.addWall(ngl::Vec3(-1,0,0),ngl::Vec3(15,15,0),true,30,30,30);
    m_collisionSolver.addWall(ngl::Vec3(0,0,1),ngl::Vec3(0,15,-15),true,30,30,30);
    m_collisionSolver.addWall(ngl::Vec3(0,0,-1),ngl::Vec3(0,15,15),false,0,0,0);
    m_collisionSolver.addWall(ngl::Vec3(0,1,0), ngl::Vec3(0,0,0),true,30,30,30);
    for(unsigned int i=0; i<m_particles.size();i++){
        //Check for initial collision, need to implement new function for this
        m_collisionSolver.initParticleCollision(m_particles[i],m_particles);
        //m_collisionSolver.calculateGroundCollision(m_particles[i],0);
    }
    //Start our timer for our calculations
    clock_t time = clock();
    m_startTime = time;

    std::cerr<<"SPH Melt Init Finished"<<std::endl;
}

void SPHMelt::drawParticles(ngl::TransformStack trans){
    //for(unsigned int i=6559; i<m_particles.size();i++){

    for(unsigned int i=0; i<m_particles.size();i++){
    //for(unsigned int i=0; i<6;i++){

        m_particles[i]->draw(trans);
    }
}

void SPHMelt::twoDWaterSimStart(){
    // Delete all our particles
    int numParticles = m_particles.size();
    float mass = m_particles[0]->getMass();
    for (unsigned int i = 0; i<m_particles.size();i++){
        delete m_particles[i];
    }
    m_meshVerts.clear();
    m_particles.clear();
    //Create new particles
    int row = 0;
    for(int i=0; i<numParticles;i++){
        ngl::Vec3 Pos(4 - (row%2)*0.1,15 + i*0.1,0.05);
        m_particles.push_back(new particle(&Pos, mass,false));
        row++;
    }
    // Found that I had better results in the 2d simulation with these prime numbers
    m_SPHHash.setPrime(0, 1);
    m_SPHHash.setPrime(0, 1);
    m_SPHHash.setPrime(0, 1);
    // Create our hash table for our new particles
    m_SPHHash.createHashTable(m_particles);
    // Initialize the density for all our particles
    m_SPHSolver.initDensity(m_particles);
    // Move our walls closer together
    m_collisionSolver.setWall(0,ngl::Vec3(1,0,0),ngl::Vec3(-5,15,10),true,0.1,0.1,30);
    m_collisionSolver.setWall(1,ngl::Vec3(-1,0,0),ngl::Vec3(5,15,10),true,0.1,0.1,30);
    m_collisionSolver.setWall(2,ngl::Vec3(0,0,1),ngl::Vec3(0,15,10.1),true,10,10,10);
    m_collisionSolver.setWall(3,ngl::Vec3(0,0,-1),ngl::Vec3(0,15,10),false);
    m_collisionSolver.setWall(4,ngl::Vec3(0,1,0),ngl::Vec3(0,0,0),true,30,30,30);

}

void SPHMelt::drawExternalForce(ngl::TransformStack *_transformStack, ngl::Camera *_cam){
    // Toon shader 4 lyf!
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["nglToonShader"]->use();
    // load all our trandforms and colour to our shader
    _transformStack->pushTransform();
    {
        _transformStack->setPosition(m_externalForce.sourcePos);
        _transformStack->setScale(1, 1, 1);
        ngl::Mat4 MVP= _transformStack->getCurrAndGlobal().getMatrix()*_cam->getVPMatrix();
        shader->setShaderParamFromMat4("MVP",MVP);
        shader->setShaderParam4f("Colour",1,0,0,0.05);
        prim->draw("sphere");
    }
    _transformStack->popTransform();

}

void SPHMelt::setCam(ngl::Camera *_c){
    for(unsigned int i = 0; i<m_particles.size();i++){
        m_particles[i]->setCamera(_c);
    }
}

void SPHMelt::drawMesh(){
    m_meshVAO->bind();
    m_meshVAO->draw();
    m_meshVAO->unbind();
}
void SPHMelt::toggleSim(){
    //if mesh simulation running change to 2d simulation
    if(m_meshSim){
        twoDWaterSimStart();
        m_meshSim = false;
    }
    //if 2d simulation running change to our mesh simulation
    else{
        startMeshSim();
        m_meshSim = true;
    }
}

void SPHMelt::restart(){
    // If mesh sim running restart mesh sim
    if(m_meshSim){
        startMeshSim();
    }
    // else restart 2d sim
    else{
        twoDWaterSimStart();
    }
}

void SPHMelt::startMeshSim(){
    // remove all our previous particles
    int numParticles = m_particles.size();
    float mass = m_particles[0]->getMass();
    for (unsigned int i = 0; i<m_particles.size();i++){
        delete m_particles[i];
    }
    m_meshVerts.clear();
    m_particles.clear();
    // gerate all our particles from our mesh again
    setMeshVerts(m_mesh->getVertexList());
    genParticles(numParticles,mass);
    // Set our hash prime numbers for the simulation
    m_SPHHash.setPrime(0, 73856093);
    m_SPHHash.setPrime(0, 19349663);
    m_SPHHash.setPrime(0, 83492791);
    // create our hash table from our new particles
    m_SPHHash.createHashTable(m_particles);
    //initialize the particles density
    m_SPHSolver.initDensity(m_particles);
    // Move our walls into collision
    m_collisionSolver.setWall(0,ngl::Vec3(1,0,0),ngl::Vec3(-15,15,0),true,30,30,30);
    m_collisionSolver.setWall(1,ngl::Vec3(-1,0,0),ngl::Vec3(15,15,0),true,30,30,30);
    m_collisionSolver.setWall(2,ngl::Vec3(0,0,1),ngl::Vec3(0,15,-15),true,30,30,30);
    m_collisionSolver.setWall(3,ngl::Vec3(0,0,-1),ngl::Vec3(0,15,15),false,0,0,0);
    m_collisionSolver.setWall(4,ngl::Vec3(0,1,0), ngl::Vec3(0,0,0),true,30,30,30);
    for(unsigned int i=0; i<m_particles.size();i++){
        //Check for initial collision, need to implement new function for this
        m_collisionSolver.initParticleCollision(m_particles[i],m_particles);
        //m_collisionSolver.calculateGroundCollision(m_particles[i],0);
    }
}

void SPHMelt::update(){
    clock_t currenTime = clock();
    float timeStep = (currenTime-m_startTime) / (float)CLOCKS_PER_SEC;
    ngl::Vec3 newPos, newVel;
    std::vector<particle*> neighbours;
    for(unsigned int i = 0; i<m_particles.size(); i++){
        //Fetch the neighbours of our current particle
        neighbours = m_SPHHash.getNeighbours(m_particles[i],15);
        m_SPHSolver.setExternalForce(m_externalForce.sourcePos,m_externalForce.radius,m_externalForce.strength,m_externalForce.push);
        //Calculate and update our particles forces
        m_SPHSolver.calcForces(m_particles[i],neighbours,timeStep);
//        m_SPHSolver.calcForces(m_particles[i],m_particles,timeStep);
        //Check for collision
        m_collisionSolver.testCollisionWithWalls(m_particles[i],timeStep);
//        m_collisionSolver.testCollisionWithParticles(m_particles[i],neighbours);

    }

    // rehash all our particles with our new locations
    m_SPHHash.createHashTable(m_particles);
    // reset the start time so we can calculate our timestep
    m_startTime = clock();

    // Update our VAO
    m_meshVAO->bind();
    // Grabs a pointer to the data in our gpu
    ngl::Real* ptr = m_meshVAO->getDataPointer(0,GL_READ_WRITE);
    unsigned int nFaces=m_meshFaces.size();
    // loop for each of the faces
    unsigned int step=0;
    ngl::Vec3 finalNormal(0);
    for(unsigned int i=0;i<nFaces;++i)
    {
      // Calcualate new normals
      finalNormal = ngl::calcNormal(*m_meshVerts[m_meshFaces[i].m_vert[0]],*m_meshVerts[m_meshFaces[i].m_vert[2]],*m_meshVerts[m_meshFaces[i].m_vert[1]]);


      // now for each triangle in the face (remember we ensured tri above)
      // loop for all the verts and set the new vert value
      // the data is packed uv, nx,ny,nz then x,y,z
      // as we only want to change x,y,z, we need to skip over
      // stuff
      for(int j=0;j<3;++j)
      {
        //std::cout<<"VAO vertex ="<<ptr[step]<<" Our vertex ="<<m_meshVerts[m_meshFaces[i].m_vert[j]]->m_x<<std::endl;
        ptr[step]=m_meshVerts[m_meshFaces[i].m_vert[j]]->m_x;
        ptr[step+1]=m_meshVerts[m_meshFaces[i].m_vert[j]]->m_y;
        ptr[step+2]=m_meshVerts[m_meshFaces[i].m_vert[j]]->m_z;
        ptr[step+3]=finalNormal.m_x;
        ptr[step+4]=finalNormal.m_y;
        ptr[step+5]=finalNormal.m_z;
        step+=6;
      }

    }
    m_meshVAO->freeDataPointer();
    m_meshVAO->unbind();
}

void SPHMelt::genParticles(int _numParticles, float _mass){


    for (unsigned int i=0; i<m_meshVerts.size(); i++){
        particle *tempPart = new particle(m_meshVerts[i],_mass,true);
        //std::cout<<m_vboMesh[i].p1<<std::endl;
        m_particles.push_back(tempPart);
    }
    std::cout<<"m_vboMesh.size()"<<m_vboMesh.size()<<std::endl;
    std::cout<<"m_vboMesh.size()/m_meshVerts.size()"<<m_vboMesh.size()/m_meshVerts.size()<<std::endl;
    std::cout<<"There are "<<m_particles.size()<<" Particles in the scene"<<std::endl;


//-----------------------Gen Particles inside the mesh currently in maintenence-------------------//
    sdf::signed_distance_field_from_mesh *SDFmesh = new sdf::signed_distance_field_from_mesh(m_meshLocation);
    //The range in which to create our random numbers in
    float xRange = SDFmesh->maximum_bound()[0]-SDFmesh->minimum_bound()[0];
    float yRange = SDFmesh->maximum_bound()[1]-SDFmesh->minimum_bound()[1];
    float zRange = SDFmesh->maximum_bound()[2]-SDFmesh->minimum_bound()[2];

    ngl::Random *rnd = ngl::Random::instance();
    ngl::Vec3 randomPoint;
    rnd->setSeed();
    while(m_particles.size()<(unsigned)_numParticles){
        //Create a random point in the bounds of our mesh
        randomPoint = rnd->getRandomPoint(xRange,yRange,zRange);
        randomPoint.m_x+=SDFmesh->minimum_bound()[0];
        randomPoint.m_y+=SDFmesh->minimum_bound()[1];
        randomPoint.m_z+=SDFmesh->minimum_bound()[2];
        //If this point is inside our mesh add it to our particles vector
        if(SDFmesh->operator ()(randomPoint.m_x,randomPoint.m_y,randomPoint.m_z)<0){
            particle *tempPart = new particle(&randomPoint,_mass,false);
            m_particles.push_back(tempPart);
        }
    }
    std::cout<<"particles in scene = "<<m_particles.size()<<std::endl;

}


