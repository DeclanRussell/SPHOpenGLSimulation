/**
  \mainpage SPH Water Simulation by Declan Russll

  @image html Sim.png
  \htmlonly
  <b><u>Installation</u></b><br>
  \endhtmlonly
  This program should run pretty much out of the box, just rebuild all once you boot up the project and click run!\n
  \htmlonly
  <b><u>Controles</u></b><br>
  \endhtmlonly
  Space - play/pause our simulation \n
  1 - toggle drawing of the mesh \n
  2 - toggle drawing of the particles \n
  3 - toggle between our mesh simulation and my brand new 2d simulation! \n
  Left click - Rotate the scene \n
  Right click - Pan the scene\n
  Ctrl + Left click - Move the external force postion. You can click and drag with this\n
  +/- - increase and decrease the strength of the external force\n
  0/9 - increase and decrease the radial influence of the external force. Note this force has linear falloff\n
  E - toggle the force between pushing and sucking\n
  R - restart the simulation\n
  Feel free to change the input mesh, just change the modelLocation in NGLScene::initialize()\n
  \htmlonly
  <b><u>Research</u></b><br>
  \endhtmlonly
  Navier Stokes Equations\n
  The over all Navier stokes euquation goes as follows,\n
  @image html NavierStokesComplete.png
  This calculates the acceleration of a particle. Now although this may look quite complicated we can break it down into 3 main parts.\n
  \n
  The Density of the particle\n
  @image html CalcDensity.png
  In the equation xi is the position of the particle that we wish to find the density for. So to find the density for this particle we sum
  together the mass of neighbouring particles j multiplied by weighting kernal W.\n
  What is a weighting kernal?\n
  The weighting kernal determins the influence each particle has one each other. For instance if 2 particle are very close, then the influence
  will be very high. Alternativly if the two particles are very far away from each other they will have very low or zero influence on each other.\n
  \n
  Calculateing the weighting kernal\n
  @image html DensityWeightingKern.png
  In this equation there are 2 variables.\n
 'r' This is the distance between the particle we are calculating density for and the neighbouring particle we
  are sampling.\n
  'h' this is our smoothing length. This ultimately determins the distance in which the particles continue to have infulence. You can see
  this from the limitations in our W default equation. If r > h then we will return 0 and this particle will have no influence.

  The Pressure gradient\n
  @image html CalcPressureGrad.png
  This is the pressure gradient, this indicates the direction of pressure and how dtring it is. If the pressure is negative then there is
  a sucking force, alternatively is positive then it is a pushing force. This equation is slightly more complicated that the previous but
  again if we split it up it becomes much easier to handle.
  We'll start with the part in the brackets. The p stands for pressure, pi is the pressure of the particle that we are calculating for
  and pj is the neighbouring particle that we are sampling. But how do we calculate the pressure you may ask?\n
  @image html PressPerParticle.png
  This euqation calculates the pressure per particle. Row (the p shaped letter) stands for density. Rowi is our current density and
  Row0 is the resting density of our particle. k is a gas constant which we can change depending on what fluid we are trying to imitate.\n
  Moving onto the next part of the pressure equation we have mj which is simply the mass of our neighbouring particle, multiplied by another
  weighting kernal.
  \n
  Calculating the pressure wighting kernal\n
  @image html PressureWeightingKern.png
  As you can see this weighting kernal is very similar to our density one. 'r' is still the distance between particles and 'h' is still
  our smoothing length. Notice the Wdefault, this means that we still have the same limitations that we did in our density kernal.\n
  The Viscosity Term\n
  @image html CalcViscTerm.png
  The Viscosity in simple terms influences particles to flow in the same direction. If a large amount of particles are moving in one direction
  others should follow them.\n
  Lets break this equation down. Firstly we have meu, this represents our viscosity coeficitent, a scaler to our viscosity equation. This can
  be changed depending on what liquid we are trying to imitate. Next 'u', this is the velocity of a particle. (uj - ui) is the vector from our
  currents particles velcity to our neighbours. 'm' we already know as mass and 'Row' we know as the density. Finally we have our third and final
  weighting kernal.
  \n
  Calculating the viscosity weighting kernal\n
  @image html ViscWeightKern.png
  Just like before we have 'r' the distance between particles and 'h' our smoothing length. Once again we just pump in these values to calculate
  the influence weight of this particle.\n
  Any External Forces\n
  @image html Forces.png
  External forces in this simulation can be things such as gravity or wind. In my simulation I have used this to create a external force to interact
  with the fluid from mouse clicks.\n
  \n
  \htmlonly
  <u>Integration</u><br>
  \endhtmlonly
  As you can see the Navier stokes equations calculate the acceleration of our particles. From this we can use the time step between updates to
  calcute our velcocity and our postion of our particles. The first itegrator I used was simple Euler euqations,
  @image html EulerInt.png
  Here we have 2 equations, the first to calculate 'u' our velocity for a particle. This uses 'a' our calculated acceleration and delta(the d) t, which
  represents the change in time. The second equation calculates 'x' our postion in a similar way.
  Also though these euqations are acurate in real world physics, for our simulations they will create large errors due to the delay between updating
  each particle. So we must use another method.\n
  @image html LeapFrog1.png
  @image html LeapFrog2.png
  This is the Leap Frog integration method. It works by using future half step velocities to calculate a more accurate value for position.
  To calculate the actual velocity at a point in time from these half step velocies we must use a forth equation.
  @image html LeapFrog3.png
  This uses our half back velocity and our half forward to calculate our final particle velocity.
  \n
  \htmlonly
  <u>Optimisations</u><br>
  \endhtmlonly
  For this simulation, for us to achieve a respectable frame rate we are going to have to sacrifice some accuracy in our equations. The Navier Stokes
  equations in real life implementation need to sample every point in exsistance to find the true acceleration of one particle. For our simulation this
  creates a colexity of O(n^2) which is far too slow! To optimize our simulation, to calculate the acceleration for one particle we should only sample
  a few particles that lie close to that particle as sampling thousands of particle that have no infulence on a particle is redundant and wastes cpu
  time.\n
  \n
  How to find our neighbours - The Spatial Hash\n
  To find the neighbouring particles from one particle we will use a hash function. This will generate a key based on the position of a particle and store
  it in a table. Particles with a similar position will have the same key assigned to them. Then all we need to do is seach for particles with the same
  key to find our neighbouring particles.
  @image html SpatialHash1.png
  This is the spatial hash function that we are going to use to generate the keys based on our particles position. Yes this may look complicated but lets
  break it up again. The x with the hat on is a function that takes in a vector of floating point numbers and returns an integer based on cell size l.
  @image html SpatialHash2.png
  For our simulation we can just set cell size l to the smoothing length of our Navier Stokes equations.\n
  Back to our original hash function notice we have three variables p1, p2, and p3. These are just very large prime numbers which in our simulation are,\n
          p1 = 73,856,093\n
          p2 = 19,349,663\n
          p3 = 83,492,791\n

  The final unknown in our equation is nh. This is the size of our hash table and is calculated with the following formulae
  @image html HashTableSize.png
  Note the prime is just an equation to find the next prime number from an input number. Which is done with this piece of code,\n\n
    int nextPrimeNum(int _currentNum){\n
    int nextPrime = _currentNum;\n
    bool Prime = false;\n
    if(_currentNum<=0){\n
        std::cerr<<"The number input is less than or equal to zero"<<std::endl;\n
        return 1;\n
    }\n
    if(_currentNum==2){\n
        return 2;\n
    }\n
    if((_currentNum % 2 ) == 0){\n
        nextPrime+=1;\n
    }\n
    while(!Prime){\n
        Prime = true;\n
        for(int i = 3; i<sqrt(nextPrime); i+=2){\n
            if((nextPrime % i)==0){\n
                Prime = false;\n
            }\n
        }\n
        if(!Prime){\n
            nextPrime+=2;\n
        }\n
    }\n
    return nextPrime;\n
}\n
\n
  \htmlonly
  <b><u>Implementation</u></b><br>
  \endhtmlonly
@image html UML.png
Here is the class diagram for my simulation. The main class is our SPHMelt class, this handles spawning all of our particles into our scene then putting them into our SPHHash class to assign them a key.
The next step is to calculate our the particles next position with our SPHSolver class. Once complete we check all the particles for collision class and correct their postion and velocity if this occures. Finally
we draw everything in our scene. With drawing I ran into a few problems. One of the viewing modes of the simulation is to be able to view the mesh as it is deformed by the SPH calculations. So this meant
firstly I would have do update the normals on the fly, secondly in the shader I had to render any back faces that had shown as front faces. Which I achieved with the following simple peice of code in the fragment shader \n
    if(!gl_FrontFacing){\n
        newNormal = normal*-1;\n
    }\n
    else{\n
        newNormal = normal;\n
    }\n
    fragColour=vec4(phongModelHalfVector(),1.0);\n

 Sadly this is not a very good solution as the mesh still deformes in a bad way. You will notice it looks very spikey. To imprrove on this I would like to implement a new way of rendering my mesh but for this
 project I have ran out of time.\n


 */
