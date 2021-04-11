1.  The folder "Normal_input" contains the input files for the cantilever beam which was a standard test case given on the MBDyn website. The plot shown in that folder shows the positions of the 3 structural nodes in X,Y,Z direction. We see that the the structural node at start of the beam shows no displacement as that was given as static, but the other nodes show a deviation from the 0 line in a linear fashion owing the force applied at the structural node 3.

2.  The folder "Velociy_input" contains the input files for the cantilever beam where we apply a a velocity in the Z direction to the structural node 3 and by the figure we can see that the position of the structural node 3 oscilates from positive to negative region and slowly damps down to zero.

3. The folder "module-gsoc-shubhaditya" contains input file "Custom_drive.txt" which contains drive caller calls for custom drive which performs numerical integration of any function by trapezoidal rule. 

4. The folder "module-gsoc-shubhaditya-new" contains 4 different folders namely "Case_1", "Case_2", "Case_3", "Case_4" which contain input files for which individual READMEs are provided in each folder. This module has custom implementation of logarithmic driver. 
