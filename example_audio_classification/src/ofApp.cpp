/*
 
 */

#include "ofApp.h"

#define AUDIO_BUFFER_SIZE 512
#define AUDIO_SAMPLE_RATE 44100
#define FFT_WINDOW_SIZE 2048
#define FFT_HOP_SIZE AUDIO_BUFFER_SIZE

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    
    FFT fft;
    fft.init(FFT_WINDOW_SIZE,FFT_HOP_SIZE,1,FFT::RECTANGULAR_WINDOW,true,false,DATA_TYPE_MATRIX);

    RandomForests forest;
    forest.setForestSize( 10 );
    forest.setNumRandomSplits( 100 );
    forest.setMaxDepth( 10 );
    forest.setMinNumSamplesPerNode( 10 );

    trainingClassLabel = 1;
    record = false;
    processAudio = true;
    trainingData.setNumDimensions( 1 );
    trainingSample.resize( AUDIO_BUFFER_SIZE, 1 );

    pipeline.addFeatureExtractionModule( fft );
    pipeline.setClassifier( forest );

    magnitudePlot.setup( FFT_WINDOW_SIZE/2, 1 );

    ofSoundStreamSetup(2, 1, this, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 4);
}

//--------------------------------------------------------------
void ofApp::update(){

    FFT *fft = pipeline.getFeatureExtractionModule< FFT >( 0 );

    if( fft ){
        vector< FastFourierTransform > &results =  fft->getFFTResultsPtr();
        magnitudePlot.setData( results[0].getMagnitudeData() );
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0, 0, 0);

    string text;
    const int MARGIN = 20;
    const int graphSpacer = 15;
    int textX = MARGIN;
    int textY = MARGIN;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
    text = "------------------- TrainingInfo -------------------";
    ofDrawBitmapString(text, textX,textY);
    
    if( record ) ofSetColor(255, 0, 0);
    else ofSetColor(255, 255, 255);
    textY += 15;
    text = record ? "RECORDING" : "Not Recording";
    ofDrawBitmapString(text, textX,textY);
    
    ofSetColor(255, 255, 255);
    textY += 15;
    text = "TrainingClassLabel: " + ofToString(trainingClassLabel);
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "NumTrainingSamples: " + ofToString(trainingData.getNumSamples());
    ofDrawBitmapString(text, textX,textY);

    textY += 15;
    text = "Info: " + infoText;
    ofDrawBitmapString(text, textX,textY);
    
    float margin = 10;
    float x = margin;
    float y = textY += 35;
    float w = ofGetWidth() - margin*2;
    float h = 250;
    magnitudePlot.draw( x, y, w, h );

    if( pipeline.getTrained() ){
        y += h + 15;
        classLikelihoodsPlot.draw( x, y, w, h );
    }

}

void ofApp::audioIn(float * input, int bufferSize, int nChannels){

    if( !processAudio ) return;

    for (int i=0; i<bufferSize; i++) {
        trainingSample[i][0] = input[i];
    }

    if( record ){
        trainingData.addSample( trainingClassLabel, trainingSample );
    }

    if( pipeline.getTrained() ){

        //Run the prediction using the matrix of audio data
        pipeline.predict( trainingSample );

        //Update the likelihood plot
        classLikelihoodsPlot.update( pipeline.getClassLikelihoods() );
    }
}

void ofApp::exit(){
    processAudio = false;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch ( key) {
        case 'r':
            record = !record;
            break;
        case '1':
            trainingClassLabel = 1;
            break;
        case '2':
            trainingClassLabel = 2;
            break;
        case '3':
            trainingClassLabel = 3;
            break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";

                //Update the plots
                classLikelihoodsPlot.setup( 60 * 5, pipeline.getNumClasses() );
                classLikelihoodsPlot.setRanges(0,1);
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.save("TrainingData.grt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.load("TrainingData.grt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}