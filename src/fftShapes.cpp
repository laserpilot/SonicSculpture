//
//  Shapes.cpp
//  3dFFT_v2
//
//  Created by Blair Neal on 5/22/15.
//
//

#include "fftShapes.h"


void fftShapes::setup(){
        fft.setup();
        timePos = 0;
        setupGUI();
        
}

void fftShapes::update(){

    
    
    setShapeTypeString();
    
    if(prevFftBins!=fft.getNumFFTbins()){
        reset();
    }
    prevFftBins = fft.getNumFFTbins();
    
    fft.update();
    
    fft.setExponent(exponent);
    fft.setFFTpercentage(binAmount);
    fft.setNumFFTBins(fftBins);
    fft.setHistorySize(volHistoryLength);
    fft.setNormalize(fftNormalize);
    fft.setVolumeRange(maxVol);
    
    fPeakColor = ofFloatColor(peakColor->r/255, peakColor->g/255, peakColor->b/255);
    fBottomColor = ofFloatColor(bottomColor->r/255, bottomColor->g/255, bottomColor->b/255);
    fQuietColor = ofFloatColor(quietColor->r/255, quietColor->g/255, quietColor->b/255);
    fLoudColor = ofFloatColor(loudColor->r/255, loudColor->g/255, loudColor->b/255);
    
    
    buildMainShape();
    
    //CURRENTLY REMOVED
    /*
     if(autoSize){ //size the mesh so that a circle completes when the number of seconds has elapsed - useful if you're trying to make a song fit a single circular traversal
     //we need to adjust for the spacing of Y in time, the number of sample points, and the radius of the circle all in relation to the number of seconds in a particular song
     //err on the side of slightly lower sample count since FPS drops and causes the number of samples to drop as well
     
     maxMeshLength = songSeconds*30; //at 30FPS (set above) we sample 30 times. 1800 times in a minute
     spinRadius = ((maxMeshLength*meshSpacingDepth*smoosh)/PI)/2; //derive the radius from the length of the mesh, taking into account the spacing between sample points if you're smoothing them out
     spinSpeed = (1/(float)meshSpacingDepth);
     scaleMesh = (float)maxMeshLength/(10*ofGetWidth());
     autoSize = false;
     }*/
    
}

void fftShapes::draw(){
    
    glEnable(GL_DEPTH_TEST);
    //ofEnableLighting();
    //light.enable();
    //material.begin();
    easyCam.begin();
    ofPushMatrix();
    if(shapeCircular) ofTranslate(meshX, meshY,meshZ);
    if(shapeTypeInt == PLANE)ofTranslate(meshX, meshY,meshZ-(timePos*1.15)); //automover - doesnt work well at the moment
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
    
    if (bDrawDebug) drawDebug();
}

void fftShapes::buildMainShape(){
    ofVec3f prevPt;
    ofVec3f currentPt;
    //cout<<fft.getUnScaledLoudestValue()<<endl;
    if(!pauseMesh){
        if(ofGetFrameNum()%timeStep==0){
            if (fft.getUnScaledLoudestValue()>1.0 || recIfSilent){ //dont draw if incoming volume is too low
                
                if(fft.getNormalized()){
                    maxVol = 1; //just 0-1 when normalized
                }
                
                for (int z = (timePos-1)*meshSpacingDepth; z<timePos*meshSpacingDepth; z = z+meshSpacingDepth) {
                    for (int x=0; x<fft.getNumFFTbins()*meshSpacingWidth; x = x+meshSpacingWidth) {
                        
                        float y;
                        if(fft.getNormalized()){
                            y = fft.getSpectrum()[x/meshSpacingWidth]*maxSpikeHeight;
                        }else{
                            y = ofMap(fft.getSpectrum()[x/meshSpacingWidth], 0, maxVol, 0, 1)*maxSpikeHeight;
                        }
                        
                        
                        float angle = ofMap(z, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
                        
                        float alpha = ofMap(z,0,maxMeshLength,1,1); //sets alpha value so it fades out, not currently used
                        
                        numRevolutions = angle/TWO_PI;
                        
                        
                        //OUTER RADIUS
                        float wigglyRadius = 2*spinRadius + y*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
                        float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
                        float tempY = (float)wigglyRadius * sin(spinSpeed*angle);
                        
                        //INNER RADIUS
                        float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
                        float tempZsmall = (float)wigglyRadiusSmall * cos(spinSpeed*angle); //inner radius
                        float tempYsmall = (float)wigglyRadiusSmall * sin(spinSpeed*angle);
                        
                        //DRAW SHAPES
                        
                        if (shapeTypeInt==DISC) {
                            makeDisc(x,y,z);
                        }else if(shapeTypeInt==SPRING){
                            makeSpring(x,y,z);
                        }else if(shapeTypeInt==PLANE){
                            makePlane(x,y,z);
                        }else if(shapeTypeInt==CIRCULAR){
                            makeCircular(x,y,z);
                        }else if(shapeTypeInt==SPIRAL){ //below this are mostly template shapes...not really fully built out
                            makeSpiral(x,y,z);
                        }else if(shapeTypeInt==CURVY){
                            makeCurvy(x, y, z);
                        }else if(shapeTypeInt==SPIRE){
                            makeSpire(x,y,z);
                        }else if(shapeTypeInt==WAVYDISC){
                            makeWavyDisc(x, y, z);
                        }else if(shapeTypeInt==STAFF){
                            makeStaff(x, y, z);
                        }


                    }
                }
                
                //Connect INDICES so our render will look correct in other apps
                connectIndices();
                
                timePos++; //time step
                timePos=timePos%maxMeshLength; //reset after it maxes out on length
                if (timePos==0) {
                    mesh.clear();
                    numRevolutions = 0;
                }
            }
        }
    }
    
}

void fftShapes::colorVertices(int xIndex, float yHeight){
    //Color via intensity
    if (xIndex/meshSpacingWidth == fft.getLoudBand()) {
        mesh.addColor(fPeakColor);
    }
    else{
        float meshHeight =fft.getSpectrum()[xIndex/meshSpacingWidth];
        mesh.addColor(
                      ofColor(
                              ofMap(meshHeight,0,maxVol,quietColor->r,loudColor->r),
                              ofMap(meshHeight,0,maxVol,quietColor->g,loudColor->g),
                              ofMap(meshHeight,0,maxVol,quietColor->b,loudColor->b))
                      );
    }
}

void fftShapes::connectIndices(){
    
    //Todo: add normals as well
    
    
    int indexHeight = timePos;
    indexHeight = ofClamp(indexHeight, 0, maxMeshLength);
    int indexWidth = fft.getNumFFTbins();
    indexWidth = ofClamp(indexWidth, 0, fft.getNumFFTbins())+4;
    
    //Add all indices
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
}

void fftShapes::reset(){
    timePos = 0;
    mesh.clear();
    numRevolutions = 0;//reset
}

void fftShapes::makeDisc(int xIndex, float yHeight, int zTime){
     //DISCUS DISCUSS
    
        float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
        //LEFT SIDE
        
        //OUTER RADIUS
        float tempRadius = spinSpeed*((fft.getNumFFTbins())*meshSpacingWidth/2)*numRevolutions;//use this if you want it to spiral outwards
        float ySpike = yHeight*(spikeScaleFactor)*(spinRadius*radToSpike);
        float wigglyRadius = 2*tempRadius + xIndex; //diameter + offset from audio
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
    

    float thickBump;
    
    float timeDimple = fmod(((float)timePos/1800)*60,10.0); //this section is for putting notches in the mesh at every 10th second so you can get timestamp approximations later
    if( timeDimple<0.5){
        thickBump = 0.2;
    }else{
        thickBump = 1;
    }
        
        //cout<<"Timecount"<<fmod(((float)timePos/1800)*60,10.0) <<endl;
        if(xIndex==0 && yHeight!= 0){
            
            mesh.addVertex(ofVec3f(tempX,-(thickness*thickBump)+yOffset,tempZ)); //bottom vert
            mesh.addColor(ofColor(200, 200*thickBump, 200));
            mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top vert
            mesh.addColor(ofColor(100, 100*thickBump, 100));
        }
        
        //MAIN PART
        mesh.addVertex(ofVec3f(tempX,ySpike,tempZ));

    
        colorVertices(xIndex, yHeight);
    
        //RIGHT SIDE
        if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){
            mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top right piece
            mesh.addColor(ofColor(100, 100*thickBump, 100));
            mesh.addVertex(ofVec3f(tempX, -thickness*thickBump+yOffset,tempZ)); //bottom right piece
            mesh.addColor(ofColor(100, 100*thickBump, 100));
        }
}

void fftShapes::makeSpring(int xIndex, float yHeight, int zTime){
    
        float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
        float wigglyRadius = 2*spinRadius + yHeight*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
        float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
        float tempY = (float)wigglyRadius * sin(spinSpeed*angle);
        float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
        float tempYsmall = (float)wigglyRadiusSmall * sin(spinSpeed*angle);
        float tempZsmall = (float)wigglyRadiusSmall * cos(spinSpeed*angle); //inner radius
        
        float extraOffset = 1;
        //LEFT SIDE
        if(xIndex==0 && yHeight!= 0){ //left side for the rest
            mesh.addVertex(ofVec3f(xIndex+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
            mesh.addColor(ofColor(255, 255, 255));
            mesh.addVertex(ofVec3f(xIndex+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset))-meshSpacingWidth, tempY,tempZ)); //top vert
            mesh.addColor(ofColor(255, 255, 255));
        }
        
        //MAIN PART
        mesh.addVertex(ofVec3f(xIndex+(numRevolutions*((fft.getNumFFTbins()+extraOffset)*meshSpacingWidth+extraOffset)),tempY,tempZ));
        
        colorVertices(xIndex, yHeight);
        
        //RIGHT SIDE
        if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){ //right side for the rest
            mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()+extraOffset)*meshSpacingWidth)+meshSpacingWidth,tempY,tempZ)); //top right piece
            mesh.addColor(ofColor(255, 255, 255));
            mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()+extraOffset)*meshSpacingWidth)+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
            mesh.addColor(ofColor(255, 255, 255));
        }
    
}

void fftShapes::makePlane(int xIndex, float yHeight, int zTime){
    
        float y = fft.getSpectrum()[xIndex/meshSpacingWidth]*maxSpikeHeight*spikeScaleFactor;
    
        //LEFT SIDE
        if(xIndex==0 && y!= 0){
            mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth,0-thickness,zTime)); //bottom vert
            mesh.addColor(ofColor(255, 255, 255));
            mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth,0,zTime)); //top vert
            mesh.addColor(ofColor(255, 255, 255));
        }
    
        mesh.addVertex(ofVec3f(xIndex,y,zTime));
        //Color via intensity
        colorVertices(xIndex, y);
    
        //RIGHT SIDE
        if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && y!=0){
            mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth,0,zTime)); //top right piece
            mesh.addColor(ofColor(255, 255, 255));
            mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth, 0-thickness,zTime)); //bottom right piece
            mesh.addColor(ofColor(255, 255, 255));
        }
    
}

void fftShapes::makeCircular(int xIndex, float yHeight, int zTime){
    
    float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
    float wigglyRadius = 2*spinRadius + yHeight*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
    float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
    float tempY = (float)wigglyRadius * sin(spinSpeed*angle);
    
    //INNER RADIUS
    float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
    float tempZsmall = (float)wigglyRadiusSmall * cos(spinSpeed*angle); //inner radius
    float tempYsmall = (float)wigglyRadiusSmall * sin(spinSpeed*angle);
    
    
    //LEFT SIDE
    if(xIndex==0 && yHeight!= 0){
        mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth, tempY,tempZ)); //top vert
        mesh.addColor(ofColor(255, 255, 255));
    }
    
    //MAIN PART
    mesh.addVertex(ofVec3f(xIndex, tempY,tempZ)); //draw as a plane
    
    //Color adding via intensity
      colorVertices(xIndex, yHeight);
    
    //RIGHT SIDE
    if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){ //right side for the rest
        mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth,tempY,tempZ)); //top right piece
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
        mesh.addColor(ofColor(255, 255, 255));
    }
}

void fftShapes::makeSpiral(int xIndex, float yHeight, int zTime){
    
    float wigglyRadius = 2*spinRadius + yHeight*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
    float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
    float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
    float tempZ = (float)(wigglyRadius-ofMap(numRevolutions, 0,7,0,wigglyRadius))* cos(spinSpeed*angle);
    float tempY = (float)(wigglyRadius -ofMap(numRevolutions, 0,7,0,wigglyRadius))* sin(spinSpeed*angle);
    float tempZsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,7,0,wigglyRadiusSmall)) * cos(spinSpeed*angle); //inner radius
    float tempYsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,7,0,wigglyRadiusSmall)) * sin(spinSpeed*angle);
    
    //LEFT SIDE
    if(xIndex==0 && yHeight!= 0){
        mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex-meshSpacingWidth, tempY,tempZ)); //top vert
        mesh.addColor(ofColor(255, 255, 255));
    }
    
    //mainPart
    mesh.addVertex(ofVec3f(xIndex, tempY,tempZ));
    //Color via intensity
    colorVertices(xIndex, yHeight);
    
    //RIGHT SIDE
    if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){
        mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth,tempY,tempZ)); //top right piece
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
        mesh.addColor(ofColor(255, 255, 255));
    }
}

void fftShapes::makeCurvy(int xIndex, float yHeight, int zTime){
    float curviness = 500;
    
    float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
    float wigglyRadius = 2*spinRadius + yHeight*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
    float tempZ = (float)wigglyRadius * cos(spinSpeed*angle); //outer radius
    float tempY = (float)wigglyRadius * sin(spinSpeed*angle);
    
    //INNER RADIUS
    float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
    float tempZsmall = (float)wigglyRadiusSmall * cos(spinSpeed*angle); //inner radius
    float tempYsmall = (float)wigglyRadiusSmall * sin(spinSpeed*angle);

    
    
    //LEFT SIDE
    if(xIndex==0 && yHeight!= 0){
        mesh.addVertex(ofVec3f(xIndex+(curviness*sin(ofGetElapsedTimef()))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+(curviness*sin(ofGetElapsedTimef()))-meshSpacingWidth, tempY,tempZ)); //top vert
        mesh.addColor(ofColor(255, 255, 255));
    }
    
    mesh.addVertex(ofVec3f(xIndex+(curviness*sin(ofGetElapsedTimef())), tempY,tempZ)); //arbitrary curves right now
    //Color via intensity
    colorVertices(xIndex, yHeight);
    
    //RIGHT SIDE
    if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){
        mesh.addVertex(ofVec3f(xIndex+(curviness*sin(ofGetElapsedTimef()))+meshSpacingWidth,tempY,tempZ)); //top right piece
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+(curviness*sin(ofGetElapsedTimef()))+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
        mesh.addColor(ofColor(255, 255, 255));
    }
}

void fftShapes::makeSpire(int xIndex, float yHeight, int zTime){
    float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
    float wigglyRadius = 2*spinRadius + yHeight*(spikeScaleFactor)*(spinRadius*radToSpike); //diameter + offset from audio
    float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
    float tempZ = (float)(wigglyRadius-ofMap(numRevolutions, 0,5,0,wigglyRadius))* cos(spinSpeed*angle);
    float tempY = (float)(wigglyRadius -ofMap(numRevolutions, 0,5,0,wigglyRadius))* sin(spinSpeed*angle);
    float tempZsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,5,0,wigglyRadiusSmall)) * cos(spinSpeed*angle); //inner radius
    float tempYsmall = (float)(wigglyRadiusSmall-ofMap(numRevolutions, 0,5,0,wigglyRadiusSmall)) * sin(spinSpeed*angle);
    
    //LEFT SIDE
    if(xIndex==0 && yHeight!= 0){
        mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))-meshSpacingWidth,tempYsmall,tempZsmall)); //bottom vert
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))-meshSpacingWidth, tempY,tempZ)); //top vert
        mesh.addColor(ofColor(255, 255, 255));
    }
    
    //MAIN PIECE
    mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth)), tempY,tempZ));
    
    //Color via intensity
    colorVertices(xIndex, yHeight);
    
    //RIGHT SIDE
    if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){
        mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))+meshSpacingWidth,tempY,tempZ)); //top right piece
        mesh.addColor(ofColor(255, 255, 255));
        mesh.addVertex(ofVec3f(xIndex+(numRevolutions*(fft.getNumFFTbins()*meshSpacingWidth))+meshSpacingWidth, tempYsmall,tempZsmall)); //bottom right piece
        mesh.addColor(ofColor(255, 255, 255));
    }
}

void fftShapes::makeWavyDisc(int xIndex, float yHeight, int zTime){
    //LEFT SIDE
    
    //OUTER RADIUS
    float angle = ofMap(zTime, 0,maxMeshLength, 0,TWO_PI); //sets starting point of angle //phi
    float wigglyRadiusSmall = 2*spinRadius-thickness; //diameter with offset for thickness
    float tempRadius = spinSpeed*((fft.getNumFFTbins())*meshSpacingWidth/2)*numRevolutions;//use this if you want it to spiral outwards
    float ySpike = yHeight*(spikeScaleFactor)*(spinRadius*radToSpike);
    float wigglyRadius = 2*tempRadius + xIndex; //diameter + offset from audio
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
    if(xIndex==0 && yHeight!= 0){
        mesh.addVertex(ofVec3f(tempX,-thickness+yOffset,tempZ)); //bottom vert
        mesh.addColor(ofColor(200, 200, 200));
        mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top vert
        mesh.addColor(ofColor(150, 150, 150));
    }
    
    //MAIN PART
    mesh.addVertex(ofVec3f(tempX,ySpike,tempZ));
    //Color via intensity
    colorVertices(xIndex, yHeight);
    
    //RIGHT SIDE
    if(xIndex==(fft.getNumFFTbins()*meshSpacingWidth)-meshSpacingWidth && yHeight!=0){
        mesh.addVertex(ofVec3f(tempX,ySpike+yOffset,tempZ)); //top right piece
        mesh.addColor(ofColor(0, 100, 179));
        mesh.addVertex(ofVec3f(tempX, -thickness+yOffset,tempZ)); //bottom right piece
        mesh.addColor(ofColor(100, 100, 179));
    }
}

void fftShapes::makeStaff(int xIndex, float yHeight, int zTime){
    //Haven't nailed the math for this yet, and it will take a new structuring of the indices to make the proper surfaces, so its more of an undertaking...not currently working...

    
    float angle = ofMap(xIndex, 0,fftBins, 0,TWO_PI); //sets starting point of angle //phi
    float wigglyRadius = spinRadius; //diameter + offset from audio
    float tempX = (float)wigglyRadius * sin(angle); //outer radius
    float tempY = (float)wigglyRadius * cos(angle);
    

    
    mesh.addVertex(ofVec3f(tempX,tempY,zTime));
    //Color via intensity
    colorVertices(xIndex, yHeight);

    
}

void fftShapes::drawDebug(){
    
    
    ofPushMatrix();
    ofTranslate(100, 0);
    fft.drawDebug();
    ofDrawBitmapStringHighlight("Num Mesh Vertices: " + ofToString(mesh.getNumVertices()), 250, 240);
    
    ofPopMatrix();
    if(bShowBars){
        fft.drawBars();
    }
    ofNoFill();
    ofDrawRectangle(1080, 0, 200, 200);
    ofDrawRectangle(1080, 200, 200, 200);
    ofDrawRectangle(1080, 400, 200, 200);
    ofDrawRectangle(1080, 600, 200, 200);
    
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
    ofDrawRectangle(0,ofGetHeight()-80, ofMap(timePos, 0, maxMeshLength, 0,ofGetWidth()), 30); //draw progress bar
    ofSetColor(80);
    int meshLengthHolder = maxMeshLength;
    ofDrawBitmapString("Time Pos: " + ofToString(timePos) + "/" + ofToString(meshLengthHolder) + "  Time Elapsed: " + ofToString(((float)timePos/1800)*60,2) + "sec", 20, ofGetHeight()-60);
    //ofDrawBitmapString("Mesh will reset after :", 20, ofGetHeight()-40);
    
    ofDrawBitmapString("Press 's' to save, 'r' to reset, 'c' to change color scheme\nClick/drag to move, right click and drag to zoom, \nDouble click to reset view\n1 to Pause iTunes, 2 to Play iTunes,  ", 400, ofGetHeight()-80);
    
    
    
}

void fftShapes::setupGUI(){
    
    soundParams.setName("Sound Params");
    soundParams.add(fftNormalize.set("Normalize over history", false));
    soundParams.add(maxVol.set("Max Volume", 80,1,500));
    soundParams.add(fftBins.set("Num FFT Bins", 32, 8, 96));
    soundParams.add(binAmount.set("Percentage of FFT", 0.14, 0.01, 0.5));
    soundParams.add(exponent.set("Exponent Curve", 1.6, 1, 3.0 ));
    soundParams.add(volHistoryLength.set("Norm. Vol. History Length", 600, 5, 800));
    soundParams.add(timeStep.set("Sample frame skip", 2, 1, 30)); //how often should we sample the sound input? Every frame, or less often? App runs at 60fps, so we just sample every other frame
    soundParams.add(recIfSilent.set("RecIfSilent", false)); //if the volume is silent, we can choose not to record
    
    shapeParams.setName("Shape Controls");
    shapeParams.add(autoClear.set("Auto reset mesh",false)); //determines whether mesh will auto clear at the end of its max length
    shapeParams.add(meshSpacingDepth.set("Space btwn time pts",10, 1, 50)); //this is how far apart the mesh points are when spaced by time
    shapeParams.add(meshSpacingWidth.set("Mesh Spacing Width", 20, 1, 300)); //this is how far apart the mesh points are in width for each time step, like if there are 16 FFT bands 10 units apart, it would be 160 units wide
    shapeParams.add(spikeScaleFactor.set("Spike Scale Factor", 1,-3,3));
    shapeParams.add(maxMeshLength.set("Max Mesh Length", 10000,200,15000)); //this is the longest the mesh can be...too high and the graphics will bog down due to too many points
    shapeParams.add(thickness.set("Thickness", 30,-100,100));
    shapeParams.add(spinRadius.set("Spin Radius", 700,0,5000)); //radius of the circle that the shape spins around(depends on drawing mode)     //spin radius would be (circumference/pi)/2
    shapeParams.add(spinSpeed.set("Spin Speed", 1, -10, 10)); //1 to revolve normally, higher or lower to love slower or faster around a circle (depends on drawing type)
    //shapeParams.add(smoosh.set("Smoosh Ratio", 1, -2, 4)); //only used for autosize, but will smoosh the curves down to make them less spiky))
    shapeParams.add(maxSpikeHeight.set("Max Spike Size", 40,1,100)); //another spike scaling factor...
    shapeParams.add(radToSpike.set("Rad To Spike", 0.008, 0, 0.1)); //ratio of the radius of the circle to the size of a spike...needs work
    
    shapeTypes.setName("Select a shape");
    shapeTypes.add(shapeTypeInt.set("Select Type", 0,0,7)); //this really needs to be a dropdown menu...
    shapeTypes.add(shapeName.set("Type", ""));
    
    
    camPosParams.setName("Cam Position Controls");
    camPosParams.add(scaleMesh.set("Scale Mesh", 0.1, 0.01, 0.4));
    camPosParams.add(meshX.set("Mesh X", 0, -ofGetWidth(), ofGetWidth()));
    camPosParams.add(meshY.set("Mesh Y", 0, -ofGetHeight(), ofGetHeight()));
    camPosParams.add(meshZ.set("Mesh Z", 0, -ofGetWidth(), 4000));
    
    colorParams.setName("Color Controls");
    colorParams.add(peakColor.set("Peak Color", ofColor::red,ofColor(0,0), ofColor(255)));
    colorParams.add(quietColor.set("Quiet Color", ofColor::black,ofColor(0,0), ofColor(255)));
    colorParams.add(loudColor.set("Loud Color", ofColor::white,ofColor(0,0), ofColor(255)));
    
    colorParams.add(bottomColor.set("Bottom Color", ofColor::whiteSmoke,ofColor(0,0), ofColor(255)));
    
    allParams.add(bShowBars.set("Draw Bars", true));
    allParams.add(bDrawDebug.set("Show Debug", true));
    allParams.add(bShowWireframe.set("Show Wireframe", false));
    allParams.add(pauseMesh.set("Pause Mesh", false));
    
    allParams.add(shapeTypes);
    allParams.add(soundParams);
    allParams.add(shapeParams);
    allParams.add(camPosParams);
    allParams.add(colorParams);

}

void fftShapes::saveToFile(){
    string songTitle = ofSystemTextBoxDialog("Please Enter the name of the song");
    if (songTitle=="") {
        mesh.save("mesh_stuff_"+ofToString(ofGetSystemTime())+".ply");
    }else{
        mesh.save(songTitle+ " "+ofToString(ofGetSystemTime())+".ply");
    }
}

void fftShapes::setShowDebug(bool drawIt){
    bDrawDebug = drawIt;
}

void fftShapes::setPauseMesh(bool pauseIt){
    pauseMesh = pauseIt;
}


void fftShapes::setShapeTypeString(){

    
    if (shapeTypeInt!=prevShapeTypeInt) {
        reset(); //clear mesh when making a different shape..
    }
    
    prevShapeTypeInt = shapeTypeInt;
    
    if (shapeTypeInt==DISC) {
        shapeName = "Disc";
    }else if(shapeTypeInt==SPRING){
        shapeName="Spring";
    }else if(shapeTypeInt==PLANE){
        shapeName="Plane";
    }else if(shapeTypeInt==CIRCULAR){
        shapeName="Circular";
    }else if(shapeTypeInt==SPIRAL){
        shapeName="Spiral";
    }else if(shapeTypeInt==CURVY){
        shapeName="Curvy";
    }else if(shapeTypeInt==SPIRE){
        shapeName="Spire";
    }else if(shapeTypeInt==WAVYDISC){
        shapeName="Wavy Disc";
    }else if(shapeTypeInt){
        shapeName="Staff";
    }
}

void fftShapes::setColorScheme(int scheme){
    
    //Todo: have these make some kind of aesthetic sense...
    if (scheme==0) {
        peakColor = ofColor::red;
        quietColor = ofColor::black;
        loudColor = ofColor::white;
    }else if(scheme==1){
        peakColor = ofColor::yellow;
        quietColor = ofColor::black;
        loudColor = ofColor::green;
    }else if(scheme==2){
        peakColor = ofColor(255,148,112);
        quietColor = ofColor(0,61,245);
        loudColor = ofColor(245,184,0);
    }else if(scheme==3){
        peakColor = ofColor(255,165,0);
        quietColor = ofColor(116,9,178);
        loudColor = ofColor(25,255,132);
    }else if(scheme==4){
        peakColor = ofColor::orange;
        quietColor = ofColor(0,0,0);
        loudColor = ofColor::orangeRed;
    }else if(scheme==5){
        peakColor = ofColor::white;
        quietColor = ofColor(0,0,0);
        loudColor = ofColor::yellow;
    }
}
