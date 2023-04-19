//Credit: https://github.com/allolib-s21/notes-Mitchell57/blob/main/drum%20sounds/Drum_Demo.cpp

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Gamma.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Spatial.h"
#include "Gamma/Types.h"
#include "Gamma/SamplePlayer.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

#include "theoryOne.h"

// using namespace gam;
using namespace al;
using namespace std;

class Kick : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Decay<> mDecay; // Added decay envelope for pitch
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>

  void init() override {
    // Intialize amplitude envelope
    // - Minimum attack (to make it thump)
    // - Short decay
    // - Maximum amplitude
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.3);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay 
    mDecay.decay(0.3);

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    mOsc.freq(getInternalParameterValue("frequency"));
    mPan.pos(0);
    // (removed parameter control for attack and release)

    while (io()) {
      mOsc.freqMul(mDecay()); // Multiply pitch oscillator by next decay value
      float s1 = mOsc() *  mAmpEnv() * getInternalParameterValue("amplitude");
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }

    if (mAmpEnv.done()) free();
  }

  void onTriggerOn() override { mAmpEnv.reset(); mDecay.reset(); }

  void onTriggerOff() override { mAmpEnv.release(); mDecay.finish(); }
};

/* ---------------------------------------------------------------- */

class Hihat : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>
  
  gam::Burst mBurst; // Resonant noise with exponential decay

  void init() override {
    // Initialize burst - Main freq, filter freq, duration
    mBurst = gam::Burst(20000, 15000, 0.05);

  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    while (io()) {
      float s1 = mBurst();
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // Left this in because I'm not sure how to tell when a burst is done
    if (mAmpEnv.done()) free();
  }
  void onTriggerOn() override { mBurst.reset(); }
  //void onTriggerOff() override {  }
};

/* ---------------------------------------------------------------- */

class Snare : public SynthVoice {
 public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Amplitude envelope
  gam::Sine<> mOsc; // Main pitch osc (top of drum)
  gam::Sine<> mOsc2; // Secondary pitch osc (bottom of drum)
  gam::Decay<> mDecay; // Pitch decay for oscillators
  gam::ReverbMS<> reverb;	// Schroeder reverberator
  gam::Burst mBurst; // Noise to simulate rattle/chains


  void init() override {
    // Initialize burst 
    mBurst = gam::Burst(10000, 5000, 0.3);

    // Initialize amplitude envelope
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.01);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay 
    mDecay.decay(0.8);

    reverb.resize(gam::FREEVERB);
		reverb.decay(0.5); // Set decay length, in seconds
		reverb.damping(0.2); // Set high-frequency damping factor in [0, 1]

  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    mOsc.freq(200);
    mOsc2.freq(150);

    while (io()) {
      
      // Each mDecay() call moves it forward (I think), so we only want
      // to call it once per sample
      float decay = mDecay();
      mOsc.freqMul(decay);
      mOsc2.freqMul(decay);

      float amp = mAmpEnv();
      float s1 = mBurst() + (mOsc() * amp * 0.1)+ (mOsc2() * amp * 0.05);
      s1 += reverb(s1) * 0.2;
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    
    if (mAmpEnv.done()) free();
  }
  void onTriggerOn() override { mBurst.reset(); mAmpEnv.reset(); mDecay.reset();}
  
  void onTriggerOff() override { mAmpEnv.release(); mDecay.finish(); }
};

/* ---------------------------------------------------------------- */

class MyApp : public App {
 public:
  SynthGUIManager<Kick> synthManager{"Kick"};

  // Set to 'true' if using samples
  bool hasSample = true; 

  // Added SamplePlayer to mix in external audio clips
  gam::SamplePlayer<> samplePlayer;
  bool paused = true;

  ParameterMIDI parameterMIDI;
  int midiNote;

  gam::Burst mBurst();

  void onInit() override {
    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
  }

  void onCreate() override {
    imguiInit();

    navControl().active(false);  // Disable navigation via keyboard, since we
                                 // will be using keyboard for note triggering

    // Play example sequence. Comment this line to start from scratch
    //    synthManager.synthSequencer().playSequence("synth4.synthSequence");
    synthManager.synthRecorder().verbose(true);

    // Load audio sample (files go in bin folder)
    if(hasSample) samplePlayer.load("guitartest.wav");

  }

  void onSound(AudioIOData& io) override {
    synthManager.render(io);  // Render audio
    
    // After rendering synths, 
    while(io() && !paused && hasSample){  
      float s = samplePlayer();
      io.out(0) +=  s;
      io.out(1) += s;
	  }
  }

  void onAnimate(double dt) override {
    imguiBeginFrame();
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  void onDraw(Graphics& g) override {
    g.clear();
    synthManager.render(g);

    imguiDraw();
  }

  bool onKeyDown(Keyboard const& k) override {
    
    // testing grounds
    if(k.key() == 'e') playSnare(0, 0.2);
    if(k.key() == 'w') playSnare(0, 2);
    if(k.key() == 'q') playKick(150, 0, 0.4, 0.9);

    if(k.key() == '1') playKick(50, 0, 0.4, 0.9);
    if(k.key() == '2') playKick(100, 0, 0.4, 0.9);
    if(k.key() == '3') playKick(150, 0, 0.4, 0.9);
    if(k.key() == '4') playKick(200, 0, 0.4, 0.9);
    if(k.key() == '5') playKick(250, 0, 0.4, 0.9);
    if(k.key() == '6') playKick(300, 0, 0.4, 0.9);

    if(k.key() == 'g') playReggaeton(96,0);

    if(k.key() == 'd') {
      float currTime=0;
      float tempo = 90;
      currTime = playTrap(tempo, currTime);
      currTime = playTrap(tempo, currTime, 'b');
      currTime = playTrap(tempo, currTime);
      currTime = playTrap(tempo, currTime, 'b');
    }

    if(k.key() == 'a') {
      float currTime=0;
      currTime= playBackbeat(120, currTime);
      currTime= playHouse(160, currTime);
      currTime= playReggaeton(95, currTime);
      currTime= playReggaeton(110, currTime);
      currTime= playBackbeat(120, currTime);
    }

    
    if(k.key() == 'h'){
      for(int i=0; i<4; i++){
        playHouse(140, i);
      }
    }

    return true;
  }

  bool onKeyUp(Keyboard const& k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.triggerOff(midiNote);
      synthManager.triggerOff(midiNote - 24);  // Trigger both off for safety
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  void playKick(float freq, float time, float duration = 0.5, float amp = 0.2, float attack = 0.01, float decay = 0.1)
  {
      auto *voice = synthManager.synth().getVoice<Kick>();
      // amp, freq, attack, release, pan
      vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.01, 0.1, 0.0});
      voice->setTriggerParams(params);
      // voice->setTriggerParams({amp, freq, 0.01, 0.1, 0.0});
      // voice->setInternalParameterValue("freq", freq);
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playHihat(float time, float duration = 0.3)
  {
      auto *voice = synthManager.synth().getVoice<Hihat>();
      // amp, freq, attack, release, pan
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playSnare(float time, float duration = 0.3)
  {
      auto *voice = synthManager.synth().getVoice<Snare>();
      // amp, freq, attack, release, pan
      synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  float playBackbeat(float tempo, float currTime=0, char take='a'){
    float beat = 60./tempo;
    float offset = currTime;

    for(int i=0; i<8; i++){
      float time = (i/2.)*beat;
      playHihat(time+offset);
    }

    switch(take){
      case 'a':
        playKick(100, 0*beat+offset, 0.4, 0.9);
        playKick(100, 2*beat+offset, 0.4, 0.9);
        break;
      case 'b':
        playKick(100, 0*beat+offset, 0.4, 0.9);
        playKick(100, 2*beat+offset, 0.4, 0.9);
        playKick(100, 2.5*beat+offset, 0.4, 0.9);
        break;
    }
    
    playSnare(1*beat+offset, 0.1);
    playSnare(3*beat+offset, 0.1);

    return (4*beat)+currTime;
  }

  float playHouse(float tempo, float currTime=0){
    float beat = 60./tempo;
    float offset = currTime;

    playHihat(0.5*beat+offset);
    playHihat(1.5*beat+offset);
    playHihat(2.5*beat+offset);
    playHihat(3.5*beat+offset);


    playKick(100, 0*beat+offset, 0.4, 0.9);
    playKick(100, 2.5*beat+offset, 0.4, 0.9);
    playKick(100, 3.5*beat+offset, 0.4, 0.9);
    playSnare(1*beat+offset, 0.1);
    playSnare(3*beat+offset, 0.1);

    return (4*beat)+currTime;
  }

  float playReggaeton(float tempo, float currTime=0){
    float beat = 60./tempo;
    float offset = currTime;

    playKick(150, 0*beat+currTime, 0.4, 0.9);
    playKick(150, 1*beat+currTime, 0.4, 0.9);
    playKick(150, 2*beat+currTime, 0.4, 0.9);
    playKick(150, 3*beat+currTime, 0.4, 0.9);

    playSnare(0.75*beat+offset, 0.1);
    playSnare(1.5*beat+offset, 0.1);
    playSnare(2.75*beat+offset, 0.1);
    playSnare(3.5*beat+offset, 0.1);

    return (4*beat)+offset;
  }

  float playTrap(float tempo, float currTime=0, char take='a'){
    float beat = 60./tempo;

    for(int i=0; i<16; i++){
      float time = (beat/4.0)*i;
      playHihat(time+currTime);
    }

    
    playKick(150, 0*beat+currTime, 0.4, 0.9);
    playKick(150, 2*beat+currTime, 0.4, 0.9);
    playKick(150, 2.75*beat+currTime, 0.4, 0.9);

    if(take == 'a'){
      playSnare(1.0*beat+currTime, 0.1);
      playSnare(3.0*beat+currTime, 0.1);
    }
    else{
      playSnare(1.0*beat+currTime, 0.1);
      playSnare(3.0*beat+currTime, 0.1);
      playSnare(3.25*beat+currTime, 0.1);
      playSnare(3.5*beat+currTime, 0.1);
      playSnare(3.75*beat+currTime, 0.1);
      playSnare(3.875*beat+currTime, 0.1);
    }
    
    

    return (4*beat)+currTime;
  }
};

int main() {
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
}