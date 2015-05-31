#pragma once

#include "ofMain.h"
#include "ofxControlPanel.h"
#include "ofxAutoControlPanel.h"
#include "ofxProcessFFT.h"
#include "ofxGui.h"
#include "fftShapes.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed(int key);
    void drawDebug();
	
	void draw3Dfigure();
    
    void setupGUI();
    
    fftShapes shapes;
	
    
    float maxSound;
    int loudestBand;
    
    int timePos;
    ofMesh mesh;
    
    ofEasyCam easyCam;
    ofLight light;
    ofMaterial material;
    
    ofxPanel mainGui;
    ofParameterGroup allParams;
    ofParameterGroup soundParams;
    ofParameterGroup colorParams;
    ofParameterGroup shapeParams;
    ofParameterGroup shapeTypes;
    ofParameterGroup camPosParams;
    
    ofParameter<int> fftBins, maxVol;
    int prevFftBins;
    ofParameter<float> binAmount, exponent, timeStep, scaleMesh, spinSpeed, spinRadius, spikeSize, radToSpike, smoosh;
    ofParameter<int> meshSpacingWidth, meshSpacingDepth, maxMeshLength, thickness, maxSpikeHeight, volHistoryLength;
    ofParameter<int> meshX, meshY, meshZ;
    ofParameter<bool> bDrawDebug, autoClear,fftNormalize, autoSize, shapeCircular, shapeSpiral, shapeCurve, shapeSpring, shapeSpire, shapePlane, bShowBars, shapeDisc, shapeWavyDisc, bShowWireframe, pauseMesh;
    ofParameter<ofColor> peakColor, bottomColor, quietColor, loudColor;
    
    //Need to set up color palettes
    
//    ofxPanel gui;
//    ofxFloatSlider maxVol;
//    ofxIntSlider fftBins;
//    ofxFloatSlider binAmount;
//    ofxFloatSlider exponent, timeStep, scaleMesh, spinSpeed, spinRadius, spikeSize, radToSpike, smoosh;
//    ofxIntSlider meshSpacingWidth, meshSpacingDepth, maxMeshLength, thickness;
//    ofxIntSlider lightX, lightY, lightZ;
//    ofxIntSlider meshX, meshY, meshZ;
//    ofxToggle autoSize, drawCircular, drawSpiral, drawCurvy, drawSpring, drawSpire,drawPlane, drawBars, drawDisc, drawWireframe;
//    ofxIntSlider songSeconds;
    
    //float maxVol;
   // int fftBins, prevFftBins;
    //float binAmount, exponent, timeStep, scaleMesh, spinSpeed, spinRadius, spikeSize, radToSpike, smoosh;
    //int meshSpacingWidth, meshSpacingDepth, maxMeshLength, thickness;
    int lightX, lightY, lightZ;
    //int meshX, meshY, meshZ;
    //bool autoSize, drawCircular, drawSpiral, drawCurvy, drawSpring, drawSpire, drawPlane, drawBars, drawDisc, drawWireframe;
    int songSeconds;
    
    ProcessFFT fft;
    
    float genericTimer;
    
    float numRevolutions;
    
//    ofColor peakColor;
//    ofColor bottomColor;
//    ofColor quietColor;
//    ofColor loudColor;
    
    ofxAutoControlPanel controlPanel;
    
    enum drawingNames {DISC, SPRING, PLANE, CIRCULAR, SPIRAL, CURVY, SPIRE, WAVYDISC};
    int drawTypeInt, prevDrawTypeInt;
    
};
