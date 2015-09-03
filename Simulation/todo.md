=== boost.mpi ===
* re-write what we have using boost's mpi stuff
* write everything else using boost

=== Big three! ===
* move
* collide
* applyGravity

=== other stuff ===
* make sure we send individual aspects of Particle instance after calculating them back to rank 0, then assemble valid Particle instance and broadcast it.
