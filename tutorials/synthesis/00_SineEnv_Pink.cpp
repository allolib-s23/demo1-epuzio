//Copied from 01_SineEnv.cpp
//Recreation of the opening notes of Boy's A Liar by PinkPanthress
//Song is in F major (one b flat)
//Used Mitchell's code for a kick (here): https://github.com/allolib-s21/notes-Mitchell57

#include <cstdio> // for printing to stdout

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

// using namespace gam;
using namespace al;
using namespace std;

// //From https://github.com/allolib-s21/notes-Mitchell57:
// class Kick : public SynthVoice {
//  public:
//   // Unit generators
//   gam::Pan<> mPan;
//   gam::Sine<> mOsc;
//   gam::Decay<> mDecay; // Added decay envelope for pitch
//   gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>
//   //note: AD = Attack, Decay, makes the sound "snappier"

//   void init() override {
//     // Intialize amplitude envelope
//     // - Minimum attack (to make it thump)
//     // - Short decay
//     // - Maximum amplitude
//     mAmpEnv.attack(0.01);
//     mAmpEnv.decay(0.3);
//     mAmpEnv.amp(1.0);

//     // Initialize pitch decay 
//     mDecay.decay(0.3);

//     createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
//     createInternalTriggerParameter("frequency", 60, 20, 5000);
//   }
// };

class SineEnv : public SynthVoice {
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Env<3> mAmpEnv;
  // envelope follower to connect audio output to graphics
  gam::EnvFollow<> mEnvFollow;

  // Additional members
  Mesh mMesh;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    // We have the mesh be a sphere
    addDisc(mMesh, 1.0, 30);

    // This is a quick way to create parameters for the voice. Trigger
    // parameters are meant to be set only when the voice starts, i.e. they
    // are expected to be constant within a voice instance. (You can actually
    // change them while you are prototyping, but their changes will only be
    // stored and aplied when a note is triggered.)

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("attackTime", 1.0, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 3.0, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    mOsc.freq(getInternalParameterValue("frequency"));
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io()) {
      float s1 = mOsc() * mAmpEnv() * getInternalParameterValue("amplitude");
      float s2;
      mEnvFollow(s1);
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001f))
      free();
  }

  // The graphics processing function
  void onProcess(Graphics &g) override {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    // Now draw
    g.pushMatrix();
    // Move x according to frequency, y according to amplitude
    g.translate(frequency / 200 - 3, amplitude, -8);
    // Scale in the x and y directions according to amplitude
    g.scale(1 - amplitude, amplitude, 1);
    // Set the color. Red and Blue according to sound amplitude and Green
    // according to frequency. Alpha fixed to 0.4
    g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }

  void onTriggerOff() override { mAmpEnv.release(); }
};

// We make an app.
class MyApp : public App {
public:
  // GUI manager for SineEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SineEnv> synthManager{"SineEnv"};

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override {
    navControl().active(false); // Disable navigation via keyboard, since we
                                // will be using keyboard for note triggering

    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());

    imguiInit();

    // Play example sequence. Comment this line to start from scratch
    playTune();
    // synthManager.synthSequencer().playSequence("synth1.synthSequence");
    synthManager.synthRecorder().verbose(true);
  }

  // The audio callback function. Called when audio hardware requires data
  void onSound(AudioIOData &io) override {
    synthManager.render(io); // Render audio
  }

  void onAnimate(double dt) override {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override {
    g.clear();
    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
  }

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const &k) override {
    if (ParameterGUI::usingKeyboard()) { // Ignore keys if GUI is using
                                         // keyboard
      return true;
    }
    if (k.shift()) {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    } else {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      if (midiNote > 0) {
        synthManager.voice()->setInternalParameterValue(
            "frequency", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
        synthManager.triggerOn(midiNote);
      }
    }
    return true;
  }

  // Whenever a key is released this function is called
  bool onKeyUp(Keyboard const &k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

//---- ADDED STUFF: ------
  //From Professor Conrad's Frere Jacques Demo:
  void playNote(float freq, float time, float duration, float amp = .2, float attack = 0.01, float decay = 0.01)
  {
    auto *voice = synthManager.synth().getVoice<SineEnv>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.1, 0.1, 0.0});
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  //My added code:
  void chord(char rootNote, float playTime, float sus, float vol){
    const float B3 = 233.08;
    const float C4 = 261.63;
    const float D4 = 293.66;
    const float E4 = 329.63;
    const float F4 = 349.23;
    const float G4 = 392.00;
    const float A4 = 440.00;

    switch (rootNote){
        case 'b': //b flat major 7 (i think)
          playNote(B3, playTime, sus, vol);
          playNote(D4, playTime, sus, vol);
          playNote(F4, playTime, sus, vol);
          playNote(A4, playTime, sus, vol);
        case 'a': //a major 7 (i think)
          playNote(C4, playTime, sus, vol);
          playNote(E4, playTime, sus, vol);
          playNote(G4, playTime, sus, vol);
          playNote(A4, playTime, sus, vol);
    }
  }  
  
  float sustain(int bpm, float duration){
    //May only work for 4/4 time, i'm rusty on my music theory
    //Duration = 4 for whole note, 2 for half, 1 for quarter (in 4/4), etc
    return (bpm * duration) / 60;
  }

  float timeElapsed(int bpm, float beatsElapsed){
    return (bpm * beatsElapsed) / 60;
  }

  void chordSequence1(float sequenceStart){
    float vol = .3;
    chord('b', 0 + sequenceStart, .01, vol);
    chord('b', timeElapsed(70, 2) + sequenceStart, .01, vol);
    chord('a', timeElapsed(70, 3) + sequenceStart, .01, vol);
    playNote(196.00, timeElapsed(70, 3.5) + sequenceStart, .01, vol); //G3
  }  

  void chordSequence2(float sequenceStart){
    float vol = .3;
    chord('b', 0 + sequenceStart, sustain(70, 2), vol);
    chord('b', timeElapsed(70, 2) + sequenceStart, sustain(70, 1.5), vol);
    playNote(440.00, timeElapsed(70, 3.5) + sequenceStart, sustain(70, .5), vol); //A4
    chord('a', timeElapsed(70, 3) + sequenceStart, sustain(70, .5), vol);
    playNote(196.00, timeElapsed(70, 3.5) + sequenceStart, sustain(70, .5), vol); //G3
  }

  void melody1(float sequenceStart){
    //start .75 of a beat in
    float vol = .5;
    const float C5 = 523.25;
    const float D5 = 587.33;
    const float E5 = 659.25;
    const float F5 = 698.46;
    const float G5 = 783.99;
    const float A5 = 880.00;
    playNote(F5, sequenceStart, .1);
    playNote(G5, timeElapsed(70, .125) + sequenceStart, .1);
    playNote(A5, timeElapsed(70, .25) + sequenceStart, .1);
    playNote(G5, timeElapsed(70, .5) + sequenceStart, .1);
    playNote(F5, timeElapsed(70, .75) + sequenceStart, .1);
    playNote(E5, timeElapsed(70, 1) + sequenceStart, .1);
    playNote(D5, timeElapsed(70, 1.25) + sequenceStart, .1);
    playNote(C5, timeElapsed(70, 1.375) + sequenceStart, .1);
  }

  void melody2(float sequenceStart){
    //start .75 of a beat in
    float vol = .5;
    const float C5 = 523.25;
    const float D5 = 587.33;
    const float E5 = 659.25;
    const float F5 = 698.46;
    const float G5 = 783.99;
    const float A5 = 880.00;
    playNote(F5, sequenceStart, sustain(70, .125), vol);
    playNote(G5, timeElapsed(70, .125) + sequenceStart, sustain(70, .125), vol);
    playNote(A5, timeElapsed(70, .25) + sequenceStart, sustain(70, .25), vol);
    playNote(G5, timeElapsed(70, .5) + sequenceStart, sustain(70, .25), vol);
    playNote(F5, timeElapsed(70, .75) + sequenceStart, sustain(70, .25), vol);
    playNote(E5, timeElapsed(70, 1) + sequenceStart, sustain(70, .25), vol);
    playNote(D5, timeElapsed(70, 1.25) + sequenceStart, sustain(70, .125), vol);
    playNote(C5, timeElapsed(70, 1.375) + sequenceStart, sustain(70, .25), vol);

    playNote(C5, timeElapsed(70, 2.5) + sequenceStart, sustain(70, .25), vol);
    playNote(D5, timeElapsed(70, 2.75) + sequenceStart, sustain(70, .25), vol);
    playNote(C5, timeElapsed(70, 3) + sequenceStart, sustain(70, .25), vol);
  }

  void playTune(){ //one measure of drums, then the tune!
    // chordSequence1(0.0);
    // chordSequence2(timeElapsed(70, 4));
    melody1(timeElapsed(70, .75));
    // melody2(timeElapsed(70, 4.75));  
  }

};

int main() {
  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
  return 0;
}
