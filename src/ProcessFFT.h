#pragma once
#include "ofMain.h"
#include "ofxEasyFft.h"

//This does a simple breakout of Kyle McDonald's EasyFFT class from his ofxFFT implementation

enum fftRangeType {SUPERLOW, LOW, MID, HIGH, MAXSOUND};

class ProcessFFT {
public:

        
    ofxEasyFft fft;
    
    void setup();
    void update();
    
    //For Debugging audio trends
    void drawHistoryGraph(ofPoint pt, fftRangeType drawType);
    
    //To feed to other components
    float getLoudBand();
    float getSuperLowVal();
    float getLowVal();
    float getMidVal();
    float getHighVal();
    float getNoisiness();
    float getSpectralCentroid(); //this is not right yet
    float getDelta();
    float getUnScaledLoudestValue();
    float getSmoothedUnScaledLoudestValue();
    
    float getIntensityAtFrequency(float _freq);
    vector<float> getNormalizedSpectrum();
    
    
    int getNumFFTbins();
    float getFFTpercentage();
    float getExponent();
    
    void setNumFFTBins(int _numFFTBins);
    void setFFTpercentage(float _FFTpercentage);
    void setExponent(float _Exponent);
    void setHistorySize(int _framesOfHistory);
    
    void drawBars();
    void drawDebug();
    

    
private:
    
    int scaleFactor; //this is arbitrary - it raises the FFT numbers so they aren't 0.0000054
    int numBins;
    float noisiness;
    float spectralCentroid;
    
    float delta;

    void calculateFFT(vector<float>& buffer, float FFTpercentage, int numFFTbins);
    
    bool saveHistory;
    int graphMaxSize; //max sampling size
    
    int numFFTbins; //how many Columns are we analyzing - 16-32, etc
    float FFTpercentage; //how much of the FFT are we parsing out into bins? usually around 20% of the whole 0-5khz
    float exponent; //this is a factor for making the high end be valued slightly more than the low end since the bass tends to be higher energy than the high end in FFT - in a range from 1.0-1.6
    
    int loudestBand; //for each frame - this is the loudest frequency band at that time
    
    float maxSound; //this is the loudest sound for each frame - unnormalized
    
    float avgMaxSoundOverTime; //average max sound in relation to the graph max sound
    
    vector <float> fftSpectrum; //this holds the amplitudes of multiple columns of the FFT
    vector <float> graphLow;
    vector <float> graphMid;
    vector <float> graphHigh;
    vector <float> graphSuperLow;
    vector <float> graphMaxSound;
    
    float superLowEqAvg, lowEqAvg, midEqAvg, highEqAvg;
    
    void drawAvgGraph(ofPoint pt, vector<float> values, ofColor _color);
    void drawAvgGraphUnScaled(ofPoint pt, vector<float> values, ofColor _color);
    
};