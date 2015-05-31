#include "ofApp.h"

string drawTypeArray[] = {"Plane", "Circular","Spiral", "Curvy", "Spring", "Spire", "Disc", "Wavy Disc"};

void ofApp::setup() {
	ofSetVerticalSync(true);	
	ofSetFrameRate(30); ///lets only sample 30 times a second instead of 60 - too much data otherwise
    
    material.setShininess( 120 );
    // the light highlight of the material //
	material.setSpecularColor(ofColor(255, 255, 255, 255));
    ofSetSmoothLighting(true);
    
    light.setDiffuseColor( ofColor(0.f, 255.f, 0.f));
    
	light.setSpecularColor( ofColor(255.f, 255.f, 0.f));
    light.setPointLight();
    
    shapes.setup();
    
    mainGui.setup();
    mainGui.setName("Parameters");
    mainGui.loadFont("Avenir Light.otf", 10);

    mainGui.add(shapes.getParameters());
    mainGui.minimizeAll();
    
}

void ofApp::update() {
    shapes.update();

}

void ofApp::draw() {

    ofBackgroundGradient(ofColor(100), ofColor::white);
	
	ofPushMatrix();
	ofSetColor(255);
    shapes.draw();
	ofPopMatrix();
    
    mainGui.draw();
    
    ofSetColor(255);
	string msg = ofToString((int) ofGetFrameRate()) + " fps";
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);
    

    
}

void ofApp::keyPressed(int key){
    if(key=='r'){
        shapes.reset();
    }
    //mesh.clear();
    if(key=='s'){
        shapes.saveToFile();
    }
    if(key=='p') shapes.setPauseMesh(!shapes.getPauseMesh());
    if(key=='d'){
        shapes.setShowDebug(!shapes.getShowDebug());
    };
    if (key=='c') {
        shapes.setColorScheme(ofRandom(0,6));
    }
    
    if(key=='1'){
        //Pause Itunes from the app
        char tempor[255];
        sprintf(tempor, "osascript -e \'tell application \"iTunes\"\' -e \"pause\" -e \"end tell\"");
        //osascript -e 'tell application "iTunes"' -e "play" -e "end tell"
        system(tempor);
    }
    if(key=='2'){
        //Press play on itunes from the app
        char tempor[255];
        sprintf(tempor, "osascript -e \'tell application \"iTunes\"\' -e \"play\" -e \"end tell\"");
        system(tempor);
    }
}





