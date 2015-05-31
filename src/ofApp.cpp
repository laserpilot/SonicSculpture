#include "ofApp.h"

string drawTypeArray[] = {"Plane", "Circular","Spiral", "Curvy", "Spring", "Spire", "Disc", "Wavy Disc"};

void ofApp::setup() {
	ofSetVerticalSync(true);	
	ofSetFrameRate(30); ///lets only sample 30 times a second instead of 60 - too much data otherwise
    
    material.setShininess( 120 );
    // the light highlight of the material //
	material.setSpecularColor(ofColor(255, 255, 255, 255));
    ofSetSmoothLighting(true);
    
    // Point lights emit light in all directions //
    // set the diffuse color, color reflected from the light source //
    light.setDiffuseColor( ofColor(0.f, 255.f, 0.f));
    
    // specular color, the highlight/shininess color //
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
	//ofBackground(0, 0, 0);
    ofBackgroundGradient(ofColor(100), ofColor::white);
	
	ofPushMatrix();
	ofSetColor(255);
    shapes.draw();
	//draw3Dfigure(); //actually draws everything
	ofPopMatrix();
    
    mainGui.draw();
    
    
	
    ofSetColor(255);
	string msg = ofToString((int) ofGetFrameRate()) + " fps";
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);
    
    //if (bDrawDebug) drawDebug();
    
}

void ofApp::draw3Dfigure() {
        
    ofVec3f prevPt;
    ofVec3f currentPt;
    //cout<<fft.getUnScaledLoudestValue()<<endl;
    if(!pauseMesh){
        if (fft.getUnScaledLoudestValue()>1.0){ //dont draw if incoming volume is too low
            //cout<<fft.getUnScaledLoudestValue()<< " "<<endl;
      
            if (ofGetElapsedTimef()>genericTimer+timeStep) {
                
                ofFloatColor tempPeakColor = ofFloatColor(peakColor->r/255, peakColor->g/255, peakColor->b/255);
                ofFloatColor tempBottomColor = ofFloatColor(bottomColor->r/255, bottomColor->g/255, bottomColor->b/255);
                ofFloatColor tempQuietColor = ofFloatColor(quietColor->r/255, quietColor->g/255, quietColor->b/255);
                ofFloatColor tempLoudColor = ofFloatColor(loudColor->r/255, loudColor->g/255, loudColor->b/255);
                
                if(fft.getNormalized()){
                    maxVol = 1; //just 0-1 when normalized
                }
                
                genericTimer = ofGetElapsedTimef();
                
                for (int z = (timePos-1)*meshSpacingDepth; z<timePos*meshSpacingDepth; z = z+meshSpacingDepth) {
                    for (int x=0; x<fft.getNumFFTbins()*meshSpacingWidth; x = x+meshSpacingWidth) {
                        
                        float y;
                        if(fft.getNormalized()){
                            y = fft.getSpectrum()[x/meshSpacingWidth]*maxSpikeHeight;
                        }else{
                            y = ofMap(fft.getSpectrum()[x/meshSpacingWidth], 0, maxVol, 0, 1)*maxSpikeHeight;
                        }
                        
                        //cout << "y: " << y <<endl;
                        //cout << "maxVol: " << maxVol <<endl;
                        //float y = avgCol[x/meshSpacingWidth]*0.25; //scale it down
                        
                        float angle = ofMap(z, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
                        
                        float alpha = ofMap(z,0,maxMeshLength,1,1); //sets alpha value so it fades out, not currently used
                        
                        numRevolutions = angle/TWO_PI;
                        
                        
                        //OUTER RADIUS
                        float wigglyRadius = 2*spinRadius + y*(spikeSize)*(spinRadius*radToSpike); //diameter + offset from audio
                        float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
                        float tempY = (float)wigglyRadius * sin(spinSpeed*angle); 
                        
                        //INNER RADIUS
                        float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
                        float tempZsmall = (float)wigglyRadiusSmall * cos(spinSpeed*angle); //inner radius
                        float tempYsmall = (float)wigglyRadiusSmall * sin(spinSpeed*angle);
            //-------------------------           
                        if (drawTypeInt == CIRCULAR) { //CIRCULAR
                            
                            //LEFT SIDE
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth, tempY,tempZ)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            
                            //MAIN PART
                            mesh.addVertex(ofVec3f(x, tempY,tempZ)); //draw as a plane
                            
                            //Color adding via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                
                                mesh.addColor(tempPeakColor);
                    
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                   mesh.addColor(
                                                 ofColor(
                                                    ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                    ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                    ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                                 );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){ //right side for the rest
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth,tempY,tempZ)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                        }
                        //-------------------------           
                        if (drawTypeInt==SPRING) { //draw Spring
                            
                                  float extraOffset = 1;
                            //LEFT SIDE
                            if(x==0 && y!= 0){ //left side for the rest
                                mesh.addVertex(ofVec3f(x+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset))-meshSpacingWidth, tempY,tempZ)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            
                            //MAIN PART
                            mesh.addVertex(ofVec3f(x+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset)),tempY,tempZ));
                            
                            //Color adding via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                                
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){ //right side for the rest
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()+extraOffset)*meshSpacingWidth)+meshSpacingWidth,tempY,tempZ)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()+extraOffset)*meshSpacingWidth)+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                        }
            //----------------------------
                        else if(drawTypeInt==DISC){ //DISCUS DISCUSS
                            //LEFT SIDE
                            
                            //OUTER RADIUS
                            float tempRadius = spinSpeed*((fft.getNumFFTbins())*meshSpacingWidth/2)*numRevolutions;//use this if you want it to spiral outwards
                            float ySpike = y*(spikeSize)*(spinRadius*radToSpike);
                            float wigglyRadius = 2*tempRadius + x; //diameter + offset from audio
                            float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
                            float tempX = (float)wigglyRadius * sin(spinSpeed*angle); 
                            
                            
                            float extraOffset = 0;
                           // float yOffset = thickness*5*numRevolutions;
                           // float yOffset = thickness*6*numRevolutions - (1-(float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                            //float yOffset = thickness*6*numRevolutions - ((float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                            //float yOffset = thickness*6*numRevolutions - (sin(ofGetElapsedTimef())-(float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                           // float yOffset = tempX*sin(numRevolutions*TWO_PI);
                            //float yOffset = (timePos%30); //stiple bottom
                            float yOffset = 0;
                            

                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(tempX,-thickness+yOffset,tempZ)); //bottom vert
                                mesh.addColor(ofColor(200, 200, 200));
                                mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top vert
                                mesh.addColor(ofColor(100, 100, 100));
                            }
                            
                            //MAIN PART
                             mesh.addVertex(ofVec3f(tempX,ySpike,tempZ));
                            
                            
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top right piece
                                mesh.addColor(ofColor(100, 100, 100));
                                mesh.addVertex(ofVec3f(tempX, -thickness+yOffset,tempZ)); //bottom right piece
                                mesh.addColor(ofColor(100, 100, 100));
                            }
                            
                        }
            //------------------------------------
                        else if(drawTypeInt==WAVYDISC){ //WAVY DISCUS DISCUSS
                            //LEFT SIDE
                            
                            //OUTER RADIUS
                            float tempRadius = spinSpeed*((fft.getNumFFTbins())*meshSpacingWidth/2)*numRevolutions;//use this if you want it to spiral outwards
                            float ySpike = y*(spikeSize)*(spinRadius*radToSpike);
                            float wigglyRadius = 2*tempRadius + x; //diameter + offset from audio
                            float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
                            float tempX = (float)wigglyRadius * sin(spinSpeed*angle); 
                            
                            
                            float extraOffset = 0;
                            // float yOffset = thickness*5*numRevolutions;
                            // float yOffset = thickness*6*numRevolutions - (1-(float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                            //float yOffset = thickness*6*numRevolutions - ((float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                            //float yOffset = thickness*6*numRevolutions - (sin(ofGetElapsedTimef())-(float)x/(eqCols*meshSpacingWidth)-1/(eqCols*meshSpacingWidth))*(6*thickness); //use this to make it lean left or right as x
                            // float yOffset = tempX*sin(numRevolutions*TWO_PI);
                            //float yOffset = (timePos%30); //stiple bottom
                            float yOffset = 10*sin(0.10*timePos);
                            
                            // float yOffset = 0; //nevermind
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(tempX,-thickness+yOffset,tempZ)); //bottom vert
                                mesh.addColor(ofColor(200, 200, 200));
                                mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top vert
                                mesh.addColor(ofColor(150, 150, 150));
                            }
                            
                            //MAIN PART
                            mesh.addVertex(ofVec3f(tempX,ySpike,tempZ)); 
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top right piece
                                mesh.addColor(ofColor(0, 100, 179));
                                mesh.addVertex(ofVec3f(tempX, -thickness+yOffset,tempZ)); //bottom right piece
                                mesh.addColor(ofColor(100, 100, 179));
                            }
                            
                        }
            //-----------------------------
                        else if(drawTypeInt == SPIRAL){ //SPIRAL ---------
                            float tempZ = (float)(wigglyRadius-ofMap(numRevolutions, 0,7,0,wigglyRadius))* cos(spinSpeed*angle);
                            float tempY = (float)(wigglyRadius -ofMap(numRevolutions, 0,7,0,wigglyRadius))* sin(spinSpeed*angle);
                            float tempZsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,7,0,wigglyRadiusSmall)) * cos(spinSpeed*angle); //inner radius
                            float tempYsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,7,0,wigglyRadiusSmall)) * sin(spinSpeed*angle);
                            
                            //LEFT SIDE
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth, tempY,tempZ)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            
                            //mainPart
                            mesh.addVertex(ofVec3f(x, tempY,tempZ));
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth,tempY,tempZ)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                        }
            //-------------------------
                        else if(drawTypeInt == SPIRE){ //SPIRE---------

                            float tempZ = (float)(wigglyRadius-ofMap(numRevolutions, 0,5,0,wigglyRadius))* cos(spinSpeed*angle);
                            float tempY = (float)(wigglyRadius -ofMap(numRevolutions, 0,5,0,wigglyRadius))* sin(spinSpeed*angle);
                            float tempZsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,5,0,wigglyRadiusSmall)) * cos(spinSpeed*angle); //inner radius
                            float tempYsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,5,0,wigglyRadiusSmall)) * sin(spinSpeed*angle);
                            
                            //LEFT SIDE
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))-meshSpacingWidth, tempY,tempZ)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            
                            //MAIN PIECE
                            mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth)), tempY,tempZ));
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))+meshSpacingWidth,tempY,tempZ)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                        }
            //---------------------------
                        else if(drawTypeInt == CURVY){
                            
                            float curviness = 500;
                            //LEFT SIDE
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(x+(curviness*sin(ofGetElapsedTimef()))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(curviness*sin(ofGetElapsedTimef()))-meshSpacingWidth, tempY,tempZ)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }

                            mesh.addVertex(ofVec3f(x+(curviness*sin(ofGetElapsedTimef())), tempY,tempZ)); //arbitrary curves right now
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                                mesh.addColor(
                                              ofColor(
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.r,tempLoudColor.r),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.g,tempLoudColor.g),
                                                      ofMap(meshHeight,0,maxVol,tempQuietColor.b,tempLoudColor.b))
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(x+(curviness*sin(ofGetElapsedTimef()))+meshSpacingWidth,tempY,tempZ)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+(curviness*sin(ofGetElapsedTimef()))+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                        }
            //---------------------------
                        else if(drawTypeInt == PLANE){ //draw plane
                            float y = fft.getSpectrum()[x/meshSpacingWidth]*maxSpikeHeight;
                            
                            //LEFT SIDE
                            if(x==0 && y!= 0){ 
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth,0-thickness,z)); //bottom vert
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x-meshSpacingWidth,0,z)); //top vert
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            
                            mesh.addVertex(ofVec3f(x,y,z));
                            //Color via intensity
                            if (x/meshSpacingWidth == fft.getLoudBand()) {
                                mesh.addColor(tempPeakColor);
                            }
                            else{
                                float meshHeight =fft.getSpectrum()[x/meshSpacingWidth];
                
                                mesh.addColor(
                                              tempQuietColor.lerp(tempLoudColor, ofMap(meshHeight,0,maxVol,0,1))
                                              
                                              );
                            }
                            
                            //RIGHT SIDE
                            if(x==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth,0,z)); //top right piece
                                mesh.addColor(ofColor(255, 255, 255));
                                mesh.addVertex(ofVec3f(x+meshSpacingWidth, 0-thickness,z)); //bottom right piece
                                mesh.addColor(ofColor(255, 255, 255));
                            }
                            

                        }
        
                    }
                }

                int indexHeight = timePos;
                indexHeight = ofClamp(indexHeight, 0, maxMeshLength);
                int indexWidth = fft.getNumFFTbins();
                indexWidth = ofClamp(indexWidth, 0, fft.getNumFFTbins())+4;
                
                //ADD ALL THEM INDICES!
                if(timePos>0){
                    for (int y = timePos-1; y<timePos; y++){ //time
                        for (int x=0; x<indexWidth-1; x++){ //eqCols
                            
                            if(x==0 && y!=0){ //left side and right side connections
                                mesh.addIndex(x+y*indexWidth);				// 0
                               mesh.addIndex((x+1)+y*indexWidth);			// 1
                                mesh.addIndex(x+(y+1)*indexWidth);			// 10
                                
                                mesh.addIndex(x+(y+1)*indexWidth);			// 10
                                mesh.addIndex((x+1)+y*indexWidth);			// 1
                                mesh.addIndex((x+1)+(y+1)*indexWidth);		// 11
                                
                                mesh.addIndex(x+y*indexWidth);				// 0
                                mesh.addIndex((x+fft.getNumFFTbins()+3)+y*indexWidth);	// 1
                                mesh.addIndex(x+(y+1)*indexWidth);			// 10
                                
                                mesh.addIndex((x+fft.getNumFFTbins()+3)+y*indexWidth);	// 1
                                mesh.addIndex((x+fft.getNumFFTbins()+3)+(y+1)*indexWidth);	// 11
                                mesh.addIndex(x+(y+1)*indexWidth);			// 10
                            }
                            
                            
                            mesh.addIndex(x+y*indexWidth);				// 0
                            mesh.addIndex((x+1)+y*indexWidth);			// 1
                            mesh.addIndex(x+(y+1)*indexWidth);			// 10
                            
                            mesh.addIndex((x+1)+y*indexWidth);			// 1
                            mesh.addIndex((x+1)+(y+1)*indexWidth);		// 11
                            mesh.addIndex(x+(y+1)*indexWidth);			// 10
                        }
                    }
                }
                
                timePos++; //time step
                timePos=timePos%maxMeshLength; //reset after it maxes out on length
                if (timePos==0) {
                    mesh.clear();
                    numRevolutions = 0;
                }
            }

        }
    }
    
    glEnable(GL_DEPTH_TEST);
    //ofEnableLighting();
    //light.enable();
    //material.begin();
    easyCam.begin();
    ofPushMatrix();
    if(shapeCircular) ofTranslate(meshX, meshY,meshZ);
    if(drawTypeInt == PLANE)ofTranslate(meshX, meshY,meshZ-(timePos*1.15)); //automover - doesnt work well at the moment
    ofScale(scaleMesh, scaleMesh, scaleMesh);
    //ofEnableLighting();
    ofSetColor(255);
    bShowWireframe ? mesh.drawWireframe() : mesh.draw();

    ofPopMatrix();
    //material.end();
    easyCam.end();
    //light.disable();
    //ofDisableLighting();
    glDisable(GL_DEPTH_TEST);

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

void ofApp::drawDebug(){
    
    
    ofPushMatrix();
    ofTranslate(100, 0);
    fft.drawDebug();
    ofDrawBitmapStringHighlight("Num Mesh Vertices: " + ofToString(mesh.getNumVertices()), 250, 240);
    
    ofPopMatrix();
    if(bShowBars){
        fft.drawBars();
    }
    ofNoFill();
    ofRect(1080, 0, 200, 200);
    ofRect(1080, 200, 200, 200);
    ofRect(1080, 400, 200, 200);
    ofRect(1080, 600, 200, 200);

    fft.drawHistoryGraph(ofPoint(1080,0), LOW);
    fft.drawHistoryGraph(ofPoint(1080,200),MID );
    fft.drawHistoryGraph(ofPoint(1080,400),HIGH );
    fft.drawHistoryGraph(ofPoint(1080,600),MAXSOUND );
    ofDrawBitmapString("LOW",1100,20);
    ofDrawBitmapString("HIGH",1100,420);
    ofDrawBitmapString("MID",1100,220);
    ofDrawBitmapString("MAX VOLUME",1100,620);
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofFill();
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255,127);
    ofRect(0,ofGetHeight()-80, ofMap(timePos, 0, maxMeshLength, 0,ofGetWidth()), 30); //draw progress bar
    ofSetColor(80);
    int meshLengthHolder = maxMeshLength;
    ofDrawBitmapString("Time Pos: " + ofToString(timePos) + "/" + ofToString(meshLengthHolder) + "  Time Elapsed: " + ofToString(((float)timePos/1800)*60) + "sec", 20, ofGetHeight()-60);
    
    mainGui.draw();
    

}


void ofApp::setupGUI(){
    std::vector <string> drawTypeVector(drawTypeArray, drawTypeArray+8); //init vector with list of draw types
    
    mainGui.setup();
    mainGui.loadFont("Avenir Light.otf", 10);
    
    soundParams.setName("Sound Params");
    soundParams.add(fftNormalize.set("Normalize over history", false));
    soundParams.add(maxVol.set("Max Volume", 200,1,500));
    soundParams.add(fftBins.set("Num FFT Bins", 32, 8, 96));
    soundParams.add(binAmount.set("Percentage of FFT", 0.14, 0.05, 0.5));
    soundParams.add(exponent.set("Exponent Curve", 1.6, 1, 3.0 ));
    soundParams.add(volHistoryLength.set("Norm. Vol. History Length", 600, 5, 800));
    soundParams.add(timeStep.set("Time Step", 0, 0, 5)); //how often should we sample the sound input?
    
    shapeParams.setName("Shape Controls");
    shapeParams.add(autoClear.set("Auto reset mesh",false)); //determines whether mesh will auto clear at the end of its max length
    shapeParams.add(meshSpacingDepth.set("Space btwn time pts",10, 1, 50)); //this is how far apart the mesh points are when spaced by time
    shapeParams.add(meshSpacingWidth.set("Mesh Spacing Width", 20, 1, 300)); //this is how far apart the mesh points are in width for each time step, like if there are 16 FFT bands 10 units apart, it would be 160 units wide
    shapeParams.add(maxMeshLength.set("Max Mesh Length", 10000,200,15000)); //this is the longest the mesh can be...too high and the graphics will bog down due to too many points
    shapeParams.add(thickness.set("Thickness", 30,-100,100));
    shapeParams.add(spinRadius.set("Spin Radius", 700,0,5000)); //radius of the circle that the shape spins around(depends on drawing mode)     //spin radius would be (circumference/pi)/2
    shapeParams.add(spinSpeed.set("Spin Speed", 1, -10, 10)); //1 to revolve normally, higher or lower to love slower or faster around a circle (depends on drawing type)
    //shapeParams.add(smoosh.set("Smoosh Ratio", 1, -2, 4)); //only used for autosize, but will smoosh the curves down to make them less spiky))
    shapeParams.add(maxSpikeHeight.set("Max Spike Size", 40,1,100));
    shapeParams.add(radToSpike.set("Rad To Spike", 0.008, 0, 0.1)); //ratio of the radius of the circle to the size of a spike...needs work
    
    shapeTypes.setName("Select a shape"); //this really needs to be a radio button group...
    shapeTypes.add(shapeDisc.set("Disc", true));
    shapeTypes.add(shapeSpring.set("Spring", false));
    shapeTypes.add(shapePlane.set("Plane", false));
    shapeTypes.add(shapeSpiral.set("Spiral", false));
    shapeTypes.add(shapeSpire.set("Spire", false));
    shapeTypes.add(shapeWavyDisc.set("Wavy Disc", false));
    shapeTypes.add(shapeCurve.set("Curvy", false));
    
    camPosParams.setName("Cam Position Controls");
    camPosParams.add(scaleMesh.set("Scale Mesh", 0.1, 0.01, 0.4));
    camPosParams.add(meshX.set("Mesh X", 0, -ofGetWidth(), ofGetWidth()));
    camPosParams.add(meshY.set("Mesh Y", 0, -ofGetHeight(), ofGetHeight()));
    camPosParams.add(meshZ.set("Mesh Z", 200, -ofGetWidth(), 4000));
    
    colorParams.setName("Color Controls");
    colorParams.add(peakColor.set("Peak Color", ofColor::red,ofColor(0,0), ofColor(255)));
    colorParams.add(quietColor.set("Quiet Color", ofColor::black,ofColor(0,0), ofColor(255)));
    colorParams.add(loudColor.set("Loud Color", ofColor::white,ofColor(0,0), ofColor(255)));

    colorParams.add(bottomColor.set("Bottom Color", ofColor::whiteSmoke,ofColor(0,0), ofColor(255)));
    
    allParams.add(bShowBars.set("Draw Bars", true));
    allParams.add(bDrawDebug.set("Show Debug", true));
    allParams.add(bShowWireframe.set("Show Wireframe", false));
    allParams.add(pauseMesh.set("Pause Mesh", true));
    allParams.add(shapeTypes);
    allParams.add(camPosParams);
    allParams.add(soundParams);
    allParams.add(shapeParams);
    allParams.add(colorParams);
   
    mainGui.setup(allParams);
    mainGui.minimizeAll();
    
    
    
    // for (int i =0; i<5; i++){
    //  drawType.push_back("hey "+ofToString(i));
    //  }
    
    /*
    controlPanel.setup();
    controlPanel.addPanel("Mesh Stuff");
    controlPanel.addMultiToggle("Draw Type", "drawType", 6, drawTypeVector);
    controlPanel.addSlider("Thickness", 30, -100, 500);
    controlPanel.addSlider("Spin Speed", "Spin Speed", 1, -10, 10); //1 to revolve normally, higher or lower to love slower or faster around a circle (depends on drawing type)
    controlPanel.addSlider("Smoosh Ratio", "Smoosh Ratio", 1, -2, 4); //only used for autosize, but will smoosh the curves down to make them less spiky
    controlPanel.addSlider("Spin Radius", "Spin Radius", 700, 0, 5000); //radius of the circle that the shape spins around(depends on drawing mode)     //spin radius would be (circumference/pi)/2
    controlPanel.addSlider("Spike Size", "Spike Size", 1, -3, 3);//similar to smoosh ratio...this scales the spike size
    controlPanel.addSlider("Max Spike Size", "Max Spike Size", 40,1,100);
    controlPanel.addSlider("Rad to Spike", "Rad To Spike", 0.008, 0, 0.1); //ratio of the radius of the circle to the size of a spike...needs work

    controlPanel.addToggle("Auto Clear", "Auto Clear", true); //determines whether mesh will auto clear at the end of its max length
    controlPanel.addToggle("Pause Mesh", "Pause Mesh", false); //pauses drawing to mesh, enabled automatically when auto clear is set to false and it reaches end of max length of mesh
    
    controlPanel.addPanel("FFT Controls");
    controlPanel.addToggle("Draw Bars", "Draw Bars", true);
    //controlPanel.addSlider("Max Vol", "Max Vol", 200, 1,500); //just a general amplitude scaling value for input
    controlPanel.addSlider("FFT Bins", "FFT Bins", 16, 8, 256); //how many fft bands to look at...more is more work, but more fine grained
    controlPanel.addSlider("Percentage of FFT", "Percentage of FFT", 0.14, 0.05, 0.5); //amount of the total fft to look at since it returns things way outside of the necessary frequency range
    controlPanel.addSlider("Exponent Curve", "Exponent Curve", 1.6, 1, 3.0); //curves the low end down so the high end is more evenly weighted
    controlPanel.addSlider("Mesh Spacing Width", "Mesh Spacing Width", 20, 1, 300); //this is how far apart the mesh points are in width for each time step, like if there are 16 FFT bands 10 units apart, it would be 160 units wide
    controlPanel.addSlider("Mesh Spacing Depth", "Mesh Spacing Depth", 10, 1, 50); //this is how far apart the mesh points are when spaced by time
    controlPanel.addSlider("Max Mesh Length", "Max Mesh Length", 10000, 200, 15000); //this is the longest the mesh can be...too high and the graphics will bog down due to too many points
    controlPanel.addSlider("Time Step", "Time Step", 0, 0, 5); //use this to sample less often than 30fps, value of 1 means sample once per second
    controlPanel.addSlider("Vol History Length", "Vol History Length", 600, 5, 800);
    controlPanel.addSlider("Volume Range", "Volume Range", 1000, 0, 1300);
    controlPanel.addToggle("Normalize", "Normalize", false);
    
    controlPanel.addPanel("3D Controls");
    controlPanel.addSlider("Scale Mesh", "Scale Mesh", 0.1, 0.01, 0.4); //this just scales the viewport, not the actual output mesh
    controlPanel.addSlider("Light X" , "Light X", 0, -ofGetWidth(), ofGetWidth());
    controlPanel.addSlider("Light Y", "Light Y", 0, -ofGetHeight(), ofGetHeight());
    controlPanel.addSlider("Light Z", "Light Z", 600, -ofGetWidth(), 4000);
    controlPanel.addSlider("Mesh X" , "Mesh X", 0, -ofGetWidth(), ofGetWidth());
    controlPanel.addSlider("Mesh Y", "Mesh Y", 0, -ofGetHeight(), ofGetHeight());
    controlPanel.addSlider("Mesh Z", "Mesh Z", 600, -ofGetWidth(), 4000);
    
    //controlPanel.addPanel("Misc.");

   // controlPanel.addSlider("Song Seconds", "Song Seconds", 60, 1, 300);
   // controlPanel.addToggle("Auto Size", "Autosize",false); //set song length first and this will create an appropriately sized mesh..not quite working
    
    
    */
    //getGuiValues();

}

