A C++ particle gravity simulator

Using SFML for graphics, Newtonian gravity calculations, Velocity-Verlet Integration for smoother movement

and the Barnes-Hut Algorithm to speed up particle calculations
https://arborjs.org/docs/barnes-hut

optimization is very difficult for this problem, especially when I want it to be physically accurate, 
The basic algorithm is O(N^2) and with the hope of this project having 1,000,000 particles that simply won't do.  

I was going to develop my own algorithm that broke the world up into a grid that got smaller and only did the computation on a small number of particles and for large distances where gravity's effects were negligible would simply compute from the center of mass and position...
and then as I was looking at an article for 'Hierarchical Grids collision detection' I stumbled upon the exact thing I was doing: the Barnes-Hut Algorithm, yet again I'm beaten to the punch by some computer scientist in the 80s. But what can you do.
