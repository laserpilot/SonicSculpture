#include "ofMain.h"
#include "ofxProcessFFT.h"
#include "ofxGui.h"


class fftShapes {
    
public:
    
    void setup();
    void setupGUI();
    void update();
    void draw();
    void drawDebug();
    void reset();
    ofParameterGroup getParameters(){ return allParams; };
    
    void buildMainShape();
    void connectIndices();
    void colorVertices(int xIndex, float yHeight);
    
    void makeDisc(int xIndex, float yHeight, int zTime);
    void makeSpring(int xIndex, float yHeight, int zTime);
    void makePlane(int xIndex, float yHeight, int zTime);
    void makeCircular(int xIndex, float yHeight, int zTime);
    void makeSpiral(int xIndex, float yHeight, int zTime);
    void makeCurvy(int xIndex, float yHeight, int zTime);
    void makeSpire(int xIndex, float yHeight, int zTime);
    void makeWavyDisc(int xIndex, float yHeight, int zTime);
    
    void saveToFile();
    void setShowDebug(bool drawIt);
    void setPauseMesh(bool pauseIt);
    void setColorScheme(int scheme);
    
    bool getShowDebug(){return bDrawDebug;};
    bool getPauseMesh(){return pauseMesh;};

    
private:
  
    ofEasyCam easyCam;
    ProcessFFT  fft;
    ofMesh      mesh;
    
    void setShapeTypeString();
    
    ofParameterGroup allParams;
    ofParameterGroup soundParams;
    ofParameterGroup colorParams;
    ofParameterGroup shapeParams;
    ofParameterGroup shapeTypes;
    ofParameterGroup camPosParams;
    

    ofParameter<int> fftBins, maxVol;
    int prevFftBins;
    ofParameter<float> binAmount, exponent, timeStep, scaleMesh, spinSpeed, spinRadius, spikeScaleFactor, radToSpike, smoosh;
    ofParameter<int> meshSpacingWidth, meshSpacingDepth, maxMeshLength, thickness, maxSpikeHeight, volHistoryLength;
    ofParameter<int> meshX, meshY, meshZ;
    ofParameter<bool> bDrawDebug, autoClear,fftNormalize, autoSize, shapeCircular, shapeSpiral, shapeCurve, shapeSpring, shapeSpire, shapePlane, bShowBars, shapeDisc, shapeWavyDisc, bShowWireframe, pauseMesh;
    ofParameter<int> shapeTypeInt;
    int prevShapeTypeInt;
    ofParameter<string> shapeName;
    ofParameter<ofColor> peakColor, bottomColor, quietColor, loudColor;
    ofFloatColor fPeakColor, fBottomColor, fQuietColor, fLoudColor;
    
    //const string shapeNames[8] = {"Disc", "Spring", "Plane", "Circle", "Spiral", "Curvy", "Spire", "WavyDisc"};
    
    enum drawingNames {DISC, SPRING, PLANE, CIRCULAR, SPIRAL, CURVY, SPIRE, WAVYDISC};
    
    
    int timePos;
    int loudestBand;
    float genericTimer;
    float numRevolutions;
    
};