#Connected Component Labeling 
It's a technique in computer vision to detect connected regions.

[Wikipedia: Connect Component Labeling](http://en.wikipedia.org/wiki/Connected-component_labeling)

##Implementation Details
One-pass (one component at a time) algorithm is used.

Connected regions are painted with different colors.

###Notes
* 16 distinct colors are used to represent regions. If there are more than 16 regions, try changing the constant NUM\_OF\_COLORS to avoid color re-use
* Program looks for components.png in the Images dir.

###Instructions to Run
1. Install and configure OpenCV.
2. Compile and run.
3. output.jpg will be generated in the same dir as the executable.



  
