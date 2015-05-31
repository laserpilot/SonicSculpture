# Sonic Prints

###**License:**

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License. To view a copy of this license, visit [http://creativecommons.org/licenses/by-nc-sa/4.0/.](http://creativecommons.org/licenses/by-nc-sa/4.0/)

####Requires:
* openFrameworks 0.8.4
* ofxGui
* ofxFFT
* OSX 10.9 or 10.10 (could be built on older OS, just isn't tested)

#####**To use (simple instructions):**

* Have an active audio input set in System Preferences
* Select your shape from the left menu (Disc, Spring, Plane, Circle, Spiral, Curvy, Spire, Wavy Disc)
* Use mouse to move shape around. Right click and drag to zoom in and out. Double click to reset camera to start position.
* Adjust sound and shape sliders to achieve desired look of the spikes
* When finished, press ’s’ to save mesh down to a file.

####**Important! Keep the app in the folder it came in. It will need the "data" folder that comes in its original zip file. **

#####**Audio input:**

Audio input must be selected before opening the program! If you change the input while the program is running, it will not switch over. Close and open the app to have it reset to the new input.

Application should default to the audio input you have selected in system preferences. a future version will allow you to select the input.

On OSX, you can use Soundflower ( https://rogueamoeba.com/freebies/soundflower/ ) to route audio directly into the program. You’ll need to select sound flower as your input and output in System preferences.

#####**What’s going on here?**

The application is performing FFT analysis on the incoming audio. This analysis allows it to look at discrete frequency ranges and their intensity. For example, the application defaults to 32 bins of frequency data, and this is distributed across a frequency range. Human hearing is about 20hz to 20khz, but the majority of musical activity is going to be in the 20hz to 4000hz region.

As a default, this application analyzes only 15% of the entire frequency spectrum (up to about 3000hz). That 3000hz is divided into 32 bins of approximately 95hz each. So bin 1 is all of the activity from 0hz to 95hz, bin 2 is 95 to 190, etc. So the bars you see on the bottom of the screen are the intensity of each bin. The far left is the bass/low end, and the far right is the treble/high end. These bars are essentially extruded in the time dimension to create the 3D shapes seen. 

The 3D waveforms are colored according to intensity, with the loudest bin being a different color from the rest. Various math transforms are applied to create different shapes like discs, springs, circles, etc.

The app was originally conceived to look at the sonic structure of a standard western music pop song of around 3-5 minutes. It can be adjusted to look at longer form music or much shorter sound bites.

#####Working with the parameters:

There are a lot of parameters exposed to allow you to tweak your design to get the look you want. Some of them will take some practice to understand how they work, and not every parameter works the same on each shape. Most often used parameters are in bold.

* Press ‘r’ to reset the current Mesh.
* Press ‘c’ to select from a few random color settings
* Press ’s’ to save the current Mesh
* Press ‘1’ to Pause iTunes
* Press ‘2’ to hit Play on iTunes

Press the “+” to expose all the parameters.

* Draw bars: show hide FFT bars on the bottom.
* Show Debug: Show the numeric readout for different frequency ranges.
* Show Wireframe: Draw the 3d shape as a wireframe (slow)
* Pause Mesh - stop adding new points into the mesh
* Select a shape: 
* Move the slider to select the different shape types. Each change here, will completely reset the shape.

* ####Sound Params
	* Normalize over history (toggle) - Default Off. Will attempt to normalize the audio over a self amount of past values. Useful if your audio has a lot of dynamic range. Default is off.
	* **Max Volume** - Default 200. Most useful to scaling the spikes to your input. You can see “Curr. Max Sound Val: “ to the right if you have debug turned on and this will give you a good range to go for. Move this if your spikes are getting too large or too small.
	* **Num FFT Bins**: Default 32 - how many divisions of the frequency range do you want to look at. Too high, and you may experience slowness or crashes could potentially go up to 256 or higher. Useful if you want to see more activity between bands. Changing this value will reset the current mesh. 
	* **Percentage of FFT**: Default 0.14. Percentage of the total frequency range the FFT analysis is capable of looking at from 0 to 1. Defaults to around a max of 3000hz. Useful if you want to see more or less of the entire frequency range. Not a lot of interesting stuff typically happens in music past 5000hz or so except for harmonics and cymbal noise and other brightness things.
	* Exponent Curve: Default 1.6. The FFT analysis is not equally weighted across the frequency spectrum. The bass range will show higher energies than the higher frequency ranges. This value will attempt to raise the value of the higher frequencies to be more equal to the bass range. Not perfect but you can see the effect if you are outputting white noise. 
	* Normalized Vol History length: Default 600. Normalization must be enabled. This is equal to the number of samples that are being used to normalize the audio over time. One sample is taken 30 times per second. 
	* Time Step: Default 0. Equal to the number of seconds between recorded samples. 0 records as fast as possible and 1 would only record once per second. 
* ####Shape Controls:
	* NOTE: not all of these apply to every shape. And some of the math will differ for the plane versus the disc. 
	* Auto reset mesh: Default true. Resets mesh automagically when it reaches the determined maximum length. 
	* **Space btwn time pts**: Default 10. This is how much space is places between samples in time. A larger value will result in the shape growing quicker but also a smoothing of sharp peaks. Similar to "spin speed" below. Changing this will mess up your current mesh, so best to reset after changing.
	* **Mesh Spacing Width**: Default 20. This is the width between points made by each bin. Increasing this will make the whole mesh much wider. 
	* Max Mesh Length: Default 10000. This is the maximum length of a mesh if the auto reset toggle is enabled. 10000 points will give you about 6 minutes of song data. 
	* Thickness: Default 30. Thickness of the flat bottom of the mesh. Could go negative if you want a weird look. 
	* Spin Radius: Default 700. This mostly applies to the circular shape and spiral. This is the radius of the shape and will determine how long it takes to complete a single revolution. Set this high if you want the circular shape to complete over the course of an entire song. 
	* Spin Speed: Default 1. Similar to "space between time points". Determines speed that circular shapes will move around. Can help smooth down spikes but will result in a much larger end result. 
	* Max Spike Size: Default 40. Not working for all shapes currently. Helps to reign in spike size but "Max volume" is a better control. 
	* Rad to Spike: Default 0.008. For certain shapes like circular and spiral, this scales the spikes to be relative to the radius. Useful to change if you have a huge radius and want your spikes to be more proportional. 
* ####Cam Position Controls
	* Scale Mesh: Default 0.1. Just scales the mesh inside the window. No change for final output. 
	* Mesh X, Y, Z: moves the mesh around in the window. Useful if it's getting too large and you want to follow something. 
* ####Color Controls:
	* Peak Color - RGB Value of the currently loudest FFT band
	* Quiet Color - RGB Value when the sounds are at their quietest
	* Loud Color - RGB Value when sounds are at their loudest


###Saving a file:

The app is programmed to automatically reset after a certain number of points has been recorded. If this value is set too high, the program will get very slow or unresponsive - so make sure you save it before it resets! It can handle about 5-6min of audio with 10000 collected samples.

When your song has finished, you can save it out with the song title so you can find it later. This saved mesh will end up in the application’s “data” Folder as a .ply file.

###Preparing a saved mesh for printing:

This part of the process still isn’t completely smooth, but it is workable with a few minutes of work. The models aren’t “sealed” properly for printing, so you must seal them by hand. They also have way more precision/vertices than you may reasonably need for 3D printing. Simply put - if you try to print with too many vertices, the print will take much much longer to process and actually print.

Once you have the .ply saved out - You can open it with Meshlab ( http://meshlab.sourceforge.net ). From there, you can Export it to any desired format to use in other applications.

I typically export to .obj and then import into Cinema 4D. Once in Cinema 4D, you can manually close the beginning and end of the mesh so that the 3D printing slicer knows to make it a solid object and not a hollow one.

I also use Cinema 4D to perform Polygon Reduction to decrease the complexity of the model so that it is faster to print. I can knock it down to 20 or even 10% of its original complexity and it still looks pretty close to what the original render was.

###Source code usage:

The code for this was developed using openFrameworks 0.8.4 originally. 

Anyone is welcome to make their own build of the app - the source is hosted on github. It was made for my specific purposes and may not fit every use case.

Please respect the license at the top and credit me when appropriate, and definitely let me know if you make anything cool!