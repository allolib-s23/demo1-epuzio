/*
  Original pseudo-randomized song
  Parameters: chord, drum progressions, length of song
  Next steps: integrating visuals, lengthening the song
  Inspired (and partially borrowed from) from Devi McCallion's work as Girls Rituals and Mom
  Songs referenced: S.N. Morning, I [Mess] Everything Up, Un See Through, 2, Joyfulthought, Bloodeater
*/

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

#include "randomness.h" //theory class I wrote to make transposition a little easier
#include <stdlib.h>     //rand
#include <time.h>       //rand also

using namespace al;
using namespace std;

// from https://github.com/AlloSphere-Research-Group/allolib_playground/blob/master/tutorials/synthesis/07_AddSyn.cpp
class AddSyn : public SynthVoice
{
public:
  gam::Sine<> mOsc;
  gam::Sine<> mOsc1;
  gam::Sine<> mOsc2;
  gam::Sine<> mOsc3;
  gam::Sine<> mOsc4;
  gam::Sine<> mOsc5;
  gam::Sine<> mOsc6;
  gam::Sine<> mOsc7;
  gam::Sine<> mOsc8;
  gam::Sine<> mOsc9;
  gam::ADSR<> mEnvStri;
  gam::ADSR<> mEnvLow;
  gam::ADSR<> mEnvUp;
  gam::Pan<> mPan;
  gam::EnvFollow<> mEnvFollow;

  // Additional members
  Mesh mMesh;

  virtual void init()
  {

    // Intialize envelopes
    mEnvStri.curve(-4); // make segments lines
    mEnvStri.levels(0, 1, 1, 0);
    mEnvStri.lengths(0.1, 0.1, 0.1);
    mEnvStri.sustain(2); // Make point 2 sustain until a release is issued
    mEnvLow.curve(-4);   // make segments lines
    mEnvLow.levels(0, 1, 1, 0);
    mEnvLow.lengths(0.1, 0.1, 0.1);
    mEnvLow.sustain(2); // Make point 2 sustain until a release is issued
    mEnvUp.curve(-4);   // make segments lines
    mEnvUp.levels(0, 1, 1, 0);
    mEnvUp.lengths(0.1, 0.1, 0.1);
    mEnvUp.sustain(2); // Make point 2 sustain until a release is issued

    // We have the mesh be a sphere
    addDisc(mMesh, 1.0, 30);

    createInternalTriggerParameter("amp", 0.01, 0.0, 0.3);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("ampStri", 0.5, 0.0, 1.0);
    createInternalTriggerParameter("attackStri", 0.1, 0.01, 3.0);
    createInternalTriggerParameter("releaseStri", 0.1, 0.1, 10.0);
    createInternalTriggerParameter("sustainStri", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("ampLow", 0.5, 0.0, 1.0);
    createInternalTriggerParameter("attackLow", 0.001, 0.01, 3.0);
    createInternalTriggerParameter("releaseLow", 0.1, 0.1, 10.0);
    createInternalTriggerParameter("sustainLow", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("ampUp", 0.6, 0.0, 1.0);
    createInternalTriggerParameter("attackUp", 0.01, 0.01, 3.0);
    createInternalTriggerParameter("releaseUp", 0.075, 0.1, 10.0);
    createInternalTriggerParameter("sustainUp", 0.9, 0.0, 1.0);
    createInternalTriggerParameter("freqStri1", 1.0, 0.1, 10);
    createInternalTriggerParameter("freqStri2", 2.001, 0.1, 10);
    createInternalTriggerParameter("freqStri3", 3.0, 0.1, 10);
    createInternalTriggerParameter("freqLow1", 4.009, 0.1, 10);
    createInternalTriggerParameter("freqLow2", 5.002, 0.1, 10);
    createInternalTriggerParameter("freqUp1", 6.0, 0.1, 10);
    createInternalTriggerParameter("freqUp2", 7.0, 0.1, 10);
    createInternalTriggerParameter("freqUp3", 8.0, 0.1, 10);
    createInternalTriggerParameter("freqUp4", 9.0, 0.1, 10);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
  }

  virtual void onProcess(AudioIOData &io) override
  {
    // Parameters will update values once per audio callback
    float freq = getInternalParameterValue("frequency");
    mOsc.freq(freq);
    mOsc1.freq(getInternalParameterValue("freqStri1") * freq);
    mOsc2.freq(getInternalParameterValue("freqStri2") * freq);
    mOsc3.freq(getInternalParameterValue("freqStri3") * freq);
    mOsc4.freq(getInternalParameterValue("freqLow1") * freq);
    mOsc5.freq(getInternalParameterValue("freqLow2") * freq);
    mOsc6.freq(getInternalParameterValue("freqUp1") * freq);
    mOsc7.freq(getInternalParameterValue("freqUp2") * freq);
    mOsc8.freq(getInternalParameterValue("freqUp3") * freq);
    mOsc9.freq(getInternalParameterValue("freqUp4") * freq);
    mPan.pos(getInternalParameterValue("pan"));
    float ampStri = getInternalParameterValue("ampStri");
    float ampUp = getInternalParameterValue("ampUp");
    float ampLow = getInternalParameterValue("ampLow");
    float amp = getInternalParameterValue("amp");
    while (io())
    {
      float s1 = (mOsc1() + mOsc2() + mOsc3()) * mEnvStri() * ampStri;
      s1 += (mOsc4() + mOsc5()) * mEnvLow() * ampLow;
      s1 += (mOsc6() + mOsc7() + mOsc8() + mOsc9()) * mEnvUp() * ampUp;
      s1 *= amp;
      float s2;
      mEnvFollow(s1);
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // if(mEnvStri.done()) free();
    if (mEnvStri.done() && mEnvUp.done() && mEnvLow.done() &&
        (mEnvFollow.value() < 0.001))
      free();
  }

  virtual void onProcess(Graphics &g)
  {
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    g.pushMatrix();
    g.translate(amplitude, amplitude, -4);
    // g.scale(frequency/2000, frequency/4000, 1);
    float scaling = 0.1;
    g.scale(scaling * frequency / 200, scaling * frequency / 400, scaling * 1);
    g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  virtual void onTriggerOn() override
  {

    mEnvStri.attack(getInternalParameterValue("attackStri"));
    mEnvStri.decay(getInternalParameterValue("attackStri"));
    mEnvStri.sustain(getInternalParameterValue("sustainStri"));
    mEnvStri.release(getInternalParameterValue("releaseStri"));

    mEnvLow.attack(getInternalParameterValue("attackLow"));
    mEnvLow.decay(getInternalParameterValue("attackLow"));
    mEnvLow.sustain(getInternalParameterValue("sustainLow"));
    mEnvLow.release(getInternalParameterValue("releaseLow"));

    mEnvUp.attack(getInternalParameterValue("attackUp"));
    mEnvUp.decay(getInternalParameterValue("attackUp"));
    mEnvUp.sustain(getInternalParameterValue("sustainUp"));
    mEnvUp.release(getInternalParameterValue("releaseUp"));

    mPan.pos(getInternalParameterValue("pan"));

    mEnvStri.reset();
    mEnvLow.reset();
    mEnvUp.reset();
  }

  virtual void onTriggerOff() override
  {
    //    std::cout << "trigger off" <<std::endl;
    mEnvStri.triggerRelease();
    mEnvLow.triggerRelease();
    mEnvUp.triggerRelease();
  }
};

// from Hunter's plucked string demo:
class PluckedString : public SynthVoice
{
public:
  float mAmp;
  float mDur;
  float mPanRise;
  gam::Pan<> mPan;
  gam::NoiseWhite<> noise;
  gam::Decay<> env;
  gam::MovingAvg<> fil{2};
  gam::Delay<float, gam::ipl::Trunc> delay;
  gam::ADSR<> mAmpEnv;
  gam::EnvFollow<> mEnvFollow;
  gam::Env<2> mPanEnv;

  // Additional members
  Mesh mMesh;

  virtual void init()
  {
    mAmp = 1;
    mDur = 2;
    mAmpEnv.levels(0, 1, 1, 0);
    mPanEnv.curve(4);
    env.decay(0.1);
    delay.maxDelay(1. / 27.5);
    delay.delay(1. / 440.0);

    addDisc(mMesh, 1.0, 30);
    createInternalTriggerParameter("amplitude", 0.1, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("attackTime", 0.001, 0.001, 1.0);
    createInternalTriggerParameter("releaseTime", 3.0, 0.1, 10.0);
    createInternalTriggerParameter("sustain", 0.7, 0.0, 1.0);
    createInternalTriggerParameter("Pan1", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("Pan2", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("PanRise", 0.0, -1.0, 1.0); // range check
  }

  //    void reset(){ env.reset(); }

  float operator()()
  {
    return (*this)(noise() * env());
  }
  float operator()(float in)
  {
    return delay(
        fil(delay() + in));
  }

  virtual void onProcess(AudioIOData &io) override
  {

    while (io())
    {
      mPan.pos(mPanEnv());
      float s1 = (*this)() * mAmpEnv() * mAmp;
      float s2;
      mEnvFollow(s1);
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    if (mAmpEnv.done() && (mEnvFollow.value() < 0.001))
      free();
  }

  virtual void onProcess(Graphics &g)
  {
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    g.pushMatrix();
    g.translate(amplitude, amplitude, -4);
    // g.scale(frequency/2000, frequency/4000, 1);
    float scaling = 0.1;
    g.scale(scaling * frequency / 200, scaling * frequency / 400, scaling * 1);
    g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  virtual void onTriggerOn() override
  {
    updateFromParameters();
    mAmpEnv.reset();
    env.reset();
    delay.zero();
  }

  virtual void onTriggerOff() override
  {
    mAmpEnv.triggerRelease();
  }

  void updateFromParameters()
  {
    mPanEnv.levels(getInternalParameterValue("Pan1"),
                   getInternalParameterValue("Pan2"),
                   getInternalParameterValue("Pan1"));
    mPanRise = getInternalParameterValue("PanRise");
    delay.freq(getInternalParameterValue("frequency"));
    mAmp = getInternalParameterValue("amplitude");
    mAmpEnv.levels()[1] = 1.0;
    mAmpEnv.levels()[2] = getInternalParameterValue("sustain");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[3] = getInternalParameterValue("releaseTime");

    mPanEnv.lengths()[0] = mDur * (1 - mPanRise);
    mPanEnv.lengths()[1] = mDur * mPanRise;
  }
};

// from christine's demo: https://github.com/allolib-s23/demo1-christinetu15/blob/main/tutorials/synthesis/demo-christine.cpp#L880
class SquareWave : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc1;
  gam::Sine<> mOsc3;
  gam::Sine<> mOsc5;

  gam::Env<3> mAmpEnv;

  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 440, 20, 5000);
    createInternalTriggerParameter("attackTime", 0.1, 0.01, 3.0);
    createInternalTriggerParameter("releaseTime", 0.1, 0.1, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    // Get the values from the parameters and apply them to the corresponding
    // unit generators. You could place these lines in the onTrigger() function,
    // but placing them here allows for realtime prototyping on a running
    // voice, rather than having to trigger a new voice to hear the changes.
    // Parameters will update values once per audio callback because they
    // are outside the sample processing loop.
    float f = getInternalParameterValue("frequency");
    mOsc1.freq(f);
    mOsc3.freq(f * 3);
    mOsc5.freq(f * 5);

    float a = getInternalParameterValue("amplitude");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io())
    {
      float s1 = mAmpEnv() * (mOsc1() * a +
                              mOsc3() * (a / 3.0) +
                              mOsc5() * (a / 5.0));

      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // We need to let the synth know that this voice is done
    // by calling the free(). This takes the voice out of the
    // rendering chain
    if (mAmpEnv.done())
      free();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }
  void onTriggerOff() override { mAmpEnv.release(); }
};

class HiHat : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>

  gam::Burst mBurst; // Resonant noise with exponential decay

  void init() override
  {
    // Initialize burst - Main freq, filter freq, duration
    mBurst = gam::Burst(20000, 15000, 0.05);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    while (io())
    {
      float s1 = mBurst();
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    // Left this in because I'm not sure how to tell when a burst is done
    if (mAmpEnv.done())
      free();
  }
  void onTriggerOn() override { mBurst.reset(); }
  // void onTriggerOff() override {  }
};

// From https://github.com/allolib-s21/notes-Mitchell57:
class Kick : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Decay<> mDecay; // Added decay envelope for pitch
  gam::AD<> mAmpEnv;   // Changed amp envelope from Env<3> to AD<>

  void init() override
  {
    // Intialize amplitude envelope
    // - Minimum attack (to make it thump)
    // - Short decay
    // - Maximum amplitude
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.3);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay
    mDecay.decay(0.3);

    createInternalTriggerParameter("amplitude", 0.5, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 150, 20, 5000);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    mOsc.freq(getInternalParameterValue("frequency"));
    mPan.pos(0);
    // (removed parameter control for attack and release)

    while (io())
    {
      mOsc.freqMul(mDecay()); // Multiply pitch oscillator by next decay value
      float s1 = mOsc() * mAmpEnv() * getInternalParameterValue("amplitude");
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }

    if (mAmpEnv.done())
    {
      free();
    }
  }

  void onTriggerOn() override
  {
    mAmpEnv.reset();
    mDecay.reset();
  }

  void onTriggerOff() override
  {
    mAmpEnv.release();
    mDecay.finish();
  }
};

// From https://github.com/allolib-s21/notes-Mitchell57:
// commented out reverbs bc I think they're in his "theory" class
class Snare : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv;   // Amplitude envelope
  gam::Sine<> mOsc;    // Main pitch osc (top of drum)
  gam::Sine<> mOsc2;   // Secondary pitch osc (bottom of drum)
  gam::Decay<> mDecay; // Pitch decay for oscillators
  // gam::ReverbMS<> reverb;	// Schroeder reverberator
  gam::Burst mBurst; // Noise to simulate rattle/chains

  void init() override
  {
    // Initialize burst
    mBurst = gam::Burst(10000, 5000, 0.1);
    // editing last number of burst shortens/makes sound snappier

    // Initialize amplitude envelope
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.01);
    mAmpEnv.amp(0.005);

    // Initialize pitch decay
    mDecay.decay(0.1);

    // reverb.resize(gam::FREEVERB);
    // reverb.decay(0.5); // Set decay length, in seconds
    // reverb.damping(0.2); // Set high-frequency damping factor in [0, 1]
    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
  }

  // The audio processing function
  void onProcess(AudioIOData &io) override
  {
    mOsc.freq(200);
    mOsc2.freq(150);

    while (io())
    {

      // Each mDecay() call moves it forward (I think), so we only want
      // to call it once per sample
      float decay = mDecay();
      mOsc.freqMul(decay);
      mOsc2.freqMul(decay);

      float amp = mAmpEnv();
      float s1 = (mBurst() + (mOsc() * amp * 0.1) + (mOsc2() * amp * 0.05)) * getInternalParameterValue("amplitude");
      // s1 += reverb(s1) * 0.2;
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }

    if (mAmpEnv.done())
      free();
  }
  void onTriggerOn() override
  {
    mBurst.reset();
    mAmpEnv.reset();
    mDecay.reset();
  }

  void onTriggerOff() override
  {
    mAmpEnv.release();
    mDecay.finish();
  }
};

class SineEnv : public SynthVoice
{
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
  void init() override
  {
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
  void onProcess(AudioIOData &io) override
  {
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
    while (io())
    {
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
    {
      free();
    }
  }

  // The graphics processing function
  void onProcess(Graphics &g) override
  {
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
class MyApp : public App
{
public:
  // GUI manager for SineEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<SquareWave> synthManager{"SquareWave"};

  // This function is called right after the window is created
  // It provides a grphics context to initialize ParameterGUI
  // It's also a good place to put things that should
  // happen once at startup.
  void onCreate() override
  {
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
  void onSound(AudioIOData &io) override
  {
    synthManager.render(io); // Render audio
  }

  void onAnimate(double dt) override
  {
    // The GUI is prepared here
    imguiBeginFrame();
    // Draw a window that contains the synth control panel
    synthManager.drawSynthControlPanel();
    imguiEndFrame();
  }

  // The graphics callback function.
  void onDraw(Graphics &g) override
  {
    g.clear();
    // Render the synth's graphics
    synthManager.render(g);

    // GUI is drawn here
    imguiDraw();
  }

  // Whenever a key is pressed, this function is called
  bool onKeyDown(Keyboard const &k) override
  {
    if (ParameterGUI::usingKeyboard())
    { // Ignore keys if GUI is using
      // keyboard
      return true;
    }
    if (k.shift())
    {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    }
    else
    {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());
      if (midiNote > 0)
      {
        synthManager.voice()->setInternalParameterValue(
            "frequency", ::pow(2.f, (midiNote - 69.f) / 12.f) * 432.f);
        synthManager.triggerOn(midiNote);
      }
    }
    return true;
  }

  // Whenever a key is released this function is called
  bool onKeyUp(Keyboard const &k) override
  {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0)
    {
      synthManager.triggerOff(midiNote);
    }
    return true;
  }

  void onExit() override { imguiShutdown(); }

  //---- ADDED STUFF: ------
  // From Professor Conrad's Frere Jacques Demo:
  void playNote(float freq, float time, float duration, float amp = .2, float attack = 0.01, float decay = 0.01)
  {
    auto *voice = synthManager.synth().getVoice<SquareWave>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.1, 0.1, 0.0});
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  // From Mitchell's code again:
  void playKick(float freq, float time, float duration = 0.5, float amp = 0.4, float attack = 0.01, float decay = 0.1)
  {
    auto *voice = synthManager.synth().getVoice<Kick>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.01, 0.1, 0.0});
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }
  void playSnare(float time, float duration = 0.3, float amplitude = 0.1)
  {
    auto *voice = synthManager.synth().getVoice<Snare>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>(amplitude);
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  void playHiHat(float time, float duration = 0.3)
  {
    auto *voice = synthManager.synth().getVoice<HiHat>();
    // amp, freq, attack, release, pan
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  // From Christine's Demo:
  void playAddSyn(float freq, float time, float duration, float amp = .8, float attack = 0.8, float decay = 0.01)
  {
    auto *voice = synthManager.synth().getVoice<SineEnv>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>({amp, freq, attack, decay, 0.0});
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  // From Hunter's plucked string demo:
  void playBass(float freq, float time, float duration, float amp = .2, float attack = 0.9, float decay = 0.001)
  {
    auto *voice = synthManager.synth().getVoice<SquareWave>();
    // amp, freq, attack, release, pan
    vector<VariantValue> params = vector<VariantValue>({amp, freq, attack, decay, 0.0});
    voice->setTriggerParams(params);
    synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
  }

  // ADDED CODE:
  // Helper functions, consts
  float bpm = 130;
  const float beat = 60 / bpm;
  const float measure = beat * 4;

  const float quarter = beat;
  const float half = beat * 2;
  const float whole = half * 2;
  const float eighth = quarter / 2;
  const float sixteenth = eighth / 2;

  float beatsElapsed(float be)
  {
    return (60 * be) / (bpm);
  }

  void playFifthChord(vector<float> freq, float playTime, float sus)
  {
    playNote(freq[0], playTime, sus, .05);
    playNote(freq[1], playTime, sus, .05);
    playNote(freq[2], playTime, sus, .05);
  }

  void playThird(vector<float> freq, float playTime, float sus)
  {
    playNote(freq[0], playTime, sus, .05);
    playNote(freq[1], playTime, sus, .05);
  }

  // SONG COMPONENTS:
  // bassline:
  void bassPattern(int sw, int sequenceStart, int transpose)
  { // 4 measures
    switch (sw)
    {
    case 1: //[X X X X] (start .5 of a beat in)
      for (int j = 0; j < 4; j++)
      {
        playBass(getFreq("C", 2, transpose), beatsElapsed(j) + sequenceStart + .5, eighth);
      }
      for (int j = 4; j < 8; j++)
      {
        playBass(getFreq("G", 2, transpose), beatsElapsed(4 + j) + sequenceStart + .5, eighth);
      }
      for (int j = 8; j < 12; j++)
      {
        playBass(getFreq("E", 2, transpose), beatsElapsed(8 + j) + sequenceStart + .5, eighth);
      }
      for (int j = 12; j < 16; j++)
      {
        playBass(getFreq("F", 2, transpose), beatsElapsed(12 + j) + sequenceStart + .5, eighth);
      }
    case 2: // [X - - X]
      playBass(getFreq("C", 2, transpose), beatsElapsed(0) + sequenceStart, half);
      playBass(getFreq("C", 2, transpose), beatsElapsed(3) + sequenceStart, quarter);

      playBass(getFreq("G", 2, transpose), beatsElapsed(4 + 0) + sequenceStart, half);
      playBass(getFreq("G", 2, transpose), beatsElapsed(4 + 3) + sequenceStart, quarter);

      playBass(getFreq("E", 2, transpose), beatsElapsed(8 + 0) + sequenceStart, half);
      playBass(getFreq("E", 2, transpose), beatsElapsed(8 + 3) + sequenceStart, quarter);

      playBass(getFreq("F", 2, transpose), beatsElapsed(12 + 0) + sequenceStart, half);
      playBass(getFreq("F", 2, transpose), beatsElapsed(12 + 3) + sequenceStart, quarter);
    case 3: //[X - - -]
      playBass(getFreq("C", 2, transpose), beatsElapsed(0) + sequenceStart, half);
      playBass(getFreq("G", 2, transpose), beatsElapsed(4 + 0) + sequenceStart, half);
      playBass(getFreq("E", 2, transpose), beatsElapsed(8 + 0) + sequenceStart, half);
      playBass(getFreq("F", 2, transpose), beatsElapsed(12 + 0) + sequenceStart, half);
    case 4: //[X - X -] (start on beat, second one off by .5
      playBass(getFreq("C", 2, transpose), beatsElapsed(0) + sequenceStart, quarter);
      playBass(getFreq("C", 2, transpose), beatsElapsed(3) + sequenceStart + .5, quarter);
      playBass(getFreq("G", 2, transpose), beatsElapsed(4 + 0) + sequenceStart, quarter);
      playBass(getFreq("G", 2, transpose), beatsElapsed(4 + 3) + sequenceStart + .5, quarter);
      playBass(getFreq("E", 2, transpose), beatsElapsed(8 + 0) + sequenceStart, quarter);
      playBass(getFreq("E", 2, transpose), beatsElapsed(8 + 3) + sequenceStart + .5, quarter);
      playBass(getFreq("F", 2, transpose), beatsElapsed(12 + 0) + sequenceStart, quarter);
      playBass(getFreq("F", 2, transpose), beatsElapsed(12 + 3) + sequenceStart + .5, quarter);
    }
  }

  // kick drum:
  void kickPattern(int sequenceStart)
  { // 4 measures of the same kick drum pattern
    for (int i = 0; i < 4; i++)
    {
      playKick(200, beatsElapsed(0 + (i * 4)) + sequenceStart);
      playKick(200, beatsElapsed(1 + (i * 4)) + sequenceStart);
      playKick(200, beatsElapsed(2.5 + (i * 4)) + sequenceStart);
      playKick(200, beatsElapsed(3 + (i * 4)) + sequenceStart);
    }
  }

  // hihat:
  void hiHatPattern(int sw, int sequenceStart)
  { // 3 different variations
    switch (sw)
    {
    case 1:
      playHiHat(beatsElapsed(3) + sequenceStart);
      playHiHat(beatsElapsed(7) + sequenceStart);
      playHiHat(beatsElapsed(7.5) + sequenceStart);
      playHiHat(beatsElapsed(12) + sequenceStart);
      playHiHat(beatsElapsed(13.5) + sequenceStart);
      playHiHat(beatsElapsed(14.5) + sequenceStart);
    case 2:
      playHiHat(beatsElapsed(1.5) + sequenceStart);
      playHiHat(beatsElapsed(2.5) + sequenceStart);
      playHiHat(beatsElapsed(3.5) + sequenceStart);
      playHiHat(beatsElapsed(12) + sequenceStart);
      playHiHat(beatsElapsed(13.5) + sequenceStart);
      playHiHat(beatsElapsed(14.5) + sequenceStart);
      playHiHat(beatsElapsed(15.5) + sequenceStart);
    case 3:
      playHiHat(beatsElapsed(4) + sequenceStart);
      playHiHat(beatsElapsed(8) + sequenceStart);
      playHiHat(beatsElapsed(10) + sequenceStart);
      playHiHat(beatsElapsed(12.5) + sequenceStart);
      playHiHat(beatsElapsed(13.5) + sequenceStart);
      playHiHat(beatsElapsed(14.5) + sequenceStart);
      playHiHat(beatsElapsed(15.5) + sequenceStart);
    case 0:
      playHiHat(sequenceStart);
      // play one?
    }
  }

  // Chord Progressions
  void mainChordProgression(float sequenceStart, int transpose)
  { // four measures total (4/4)
    playFifthChord(getFifthChordFreqs("E", 3, transpose, 2), sequenceStart, whole);
    playFifthChord(getFifthChordFreqs("B", 3, transpose, 2), beatsElapsed(4) + sequenceStart, whole);
    playFifthChord(getFifthChordFreqs("A", 3, transpose, 2), beatsElapsed(8) + sequenceStart, whole);
    playFifthChord(getFifthChordFreqs("D", 3, transpose, 1), beatsElapsed(12) + sequenceStart, whole);
  }

  void accompanyingChordProgression(float sequenceStart, int transpose)
  { // 4 measures total
    playThird(getFifthChordFreqs("C", 3, transpose, 0), beatsElapsed(1.5) + sequenceStart, quarter);
    playThird(getFifthChordFreqs("C", 3, transpose, 0), beatsElapsed(3) + sequenceStart, eighth);

    playThird(getFifthChordFreqs("G", 3, transpose, 0), beatsElapsed(4 + 1.5) + sequenceStart, quarter);
    playThird(getFifthChordFreqs("G", 3, transpose, 0), beatsElapsed(4 + 3) + sequenceStart, eighth);

    playThird(getFifthChordFreqs("E", 3, transpose, 0), beatsElapsed(8 + 1.5) + sequenceStart, quarter);
    playThird(getFifthChordFreqs("E", 3, transpose, 0), beatsElapsed(8 + 3) + sequenceStart, eighth);

    playThird(getFifthChordFreqs("F", 3, transpose, 0), beatsElapsed(12 + 1.5) + sequenceStart, quarter);
    playThird(getFifthChordFreqs("F", 3, transpose, 0), beatsElapsed(12 + 3) + sequenceStart, eighth);
  }

  void transitionalChords(float sequenceStart, int transpose)
  {
    playThird(getFifthChordFreqs("E", 2, transpose, 0), beatsElapsed(12) + sequenceStart, half);
    playThird(getFifthChordFreqs("F", 2, transpose, 2), beatsElapsed(14) + sequenceStart, half);
  }

  void endingChords(float sequenceStart, int transpose)
  {
    playFifthChord(getFifthChordFreqs("E", 3, transpose, 2), sequenceStart, whole);

    playNote(getFreq("C", 3, transpose), beatsElapsed(4) + sequenceStart, whole); // idk what chord this is but it's not a fifth
    playNote(getFreq("D", 3, transpose), beatsElapsed(4) + sequenceStart, whole);
    playNote(getFreq("G", 3, transpose), beatsElapsed(4) + sequenceStart, whole);

    playNote(getFreq("C", 3, transpose), beatsElapsed(8) + sequenceStart, whole * 2);
  }

  // MELODIES:
  void endingMelody(float sequenceStart, int transpose)
  {
    playNote(getFreq("C", 4, transpose), beatsElapsed(12) + sequenceStart, eighth);
    playNote(getFreq("D", 4, transpose), beatsElapsed(12.5) + sequenceStart, eighth);
    playNote(getFreq("E", 4, transpose), beatsElapsed(13) + sequenceStart, quarter);
    playNote(getFreq("D", 4, transpose), beatsElapsed(14) + sequenceStart, eighth);
    playNote(getFreq("D", 4, transpose), beatsElapsed(15) + sequenceStart, eighth);
    playNote(getFreq("D", 4, transpose), beatsElapsed(15.5) + sequenceStart, whole);
  }

  // Putting it all together!

  void playTune()
  {
    srand((unsigned)time(NULL)); // seed the random number
    int key = rand() % 12;       // this is the number of steps we'll transpose the composition up or down
    int HiHatRNG, bassRNG;
    cout << "STEPS FROM A: " << key << endl;

    HiHatRNG = rand() % 4; // reroll hi hat RNG
    playHiHat(HiHatRNG, beatsElapsed(0)); // add drums here

    HiHatRNG = rand() % 4; // reroll hi hat RNG
    playHiHat(HiHatRNG, beatsElapsed(16));
    bassRNG = rand() % 4; // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(16), key);

    HiHatRNG = rand() % 4; // reroll hi hat RNG
    playHiHat(HiHatRNG, beatsElapsed(32));
    bassRNG = rand() % 4; // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(32), key);
    mainChordProgression(beatsElapsed(32), key);
    accompanyingChordProgression(beatsElapsed(32), key);

    HiHatRNG = rand() % 4; // reroll hi hat RNG
    playHiHat(HiHatRNG, beatsElapsed(48));
    bassRNG = rand() % 4; // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(48), key);
    mainChordProgression(beatsElapsed(48), key);
    accompanyingChordProgression(beatsElapsed(48), key);

    kickPattern(beatsElapsed(48));
    kickPattern(beatsElapsed(52));
    kickPattern(beatsElapsed(56));
    kickPattern(beatsElapsed(60));

    // bridge
    playHiHat(3, beatsElapsed(4 * 4 * 4)); // riser
    bassRNG = rand() % 4;                  // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(4 * 4 * 4), key);
    transitionalChords(beatsElapsed(4 * 4 * 4), key);

    // chorus (it's the same as case 4.... i'll make it poppier in future)
    HiHatRNG = rand() % 4; // reroll hi hat RNG
    playHiHat(HiHatRNG, beatsElapsed(4 * 4 * (4 + 1)));
    bassRNG = rand() % 4; // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(4 * 4 * (4 + 1)), key);
    mainChordProgression(beatsElapsed(4 * 4 * (4 + 1)), key);
    accompanyingChordProgression(beatsElapsed(4 * 4 * (4 + 1)), key);

    kickPattern(beatsElapsed(4 * 4 * (4 + 1)));
    kickPattern(beatsElapsed(4 * 4 * (4 + 1)));
    kickPattern(beatsElapsed(4 * 4 * (4 + 1)));
    kickPattern(beatsElapsed(4 * 4 * (4 + 1)));

    // outro
    endingMelody(beatsElapsed(4 * 4 * (4 + 2)), key);
    bassRNG = rand() % 4; // reroll bass pattern RNG
    bassPattern(bassRNG, beatsElapsed(4 * 4 * (4 + 2)), key);
    endingChords(beatsElapsed(4 * 4 * (4 + 2)), key);
  }

  // 	void playTune(){
  // 	srand((unsigned) time(NULL)); //seed the random number
  // 	int key = rand() % 12; //this is the number of steps we'll transpose the composition up or down
  // 	int HiHatRNG, bassRNG;
  // 	cout << "STEPS FROM A: " << key << endl;

  // 	  for(int intro = 4; intro > 0; intro--){ // intro is the number of 4 measure components we'll have of the song before the bridge to the chorus
  // // 		  kickPattern(beatsElapsed((4*intro) - (4*(4 - 4))));
  // // 		kickPattern(beatsElapsed((4*intro) - (4*(4 - 4))));
  // // 		kickPattern(beatsElapsed((4*intro) - (4*(4 - 4))));
  // // 		kickPattern(beatsElapsed((4*intro) - (4*(4 - 4))));
  // // 		  playHiHat(HiHatRNG, beatsElapsed((4*intro) - (4*(4 - 4))));

  // 		  if(intro == 4){ // hi hat and kick only
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 		  }
  // 		  if(intro == 3){ //hi hat kick and bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed(4* 4*(4 - intro)), key);
  // 		  }
  // 		  if(intro == 2){ //hi hat, kick, chords, accompanyment, bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG,  beatsElapsed(4* 4*(4 - intro)), key);
  // 			  mainChordProgression( beatsElapsed(4* 4*(4 - intro)), key);
  // 			  accompanyingChordProgression( beatsElapsed(4* 4*(4 - intro)), key);
  // 		  }
  // 		  if(intro == 1){ //hi hat, kick, chords, accompanyment, bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG,  beatsElapsed(4* 4*(4 - intro)), key);
  // 			  mainChordProgression( beatsElapsed(4* 4*(4 - intro)), key);
  // 			  accompanyingChordProgression( beatsElapsed(4* 4*(4 - intro)), key);
  // 		  }
  // 	  } //note to self: make this a switch please

  // 	  //bridge
  // 	  playHiHat(3, beatsElapsed(4* 4 * 4)); //riser
  // 	  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 	bassPattern(bassRNG, beatsElapsed(4* 4 * 4), key);
  // 	  transitionalChords(beatsElapsed(4* 4 * 4), key);

  // 	  //chorus (it's the same as case 4.... i'll make it poppier in future)
  // 	  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 	   playHiHat(HiHatRNG, beatsElapsed(4* 4 * (4+1)));
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed(4* 4 * (4+1)), key);
  // 			  mainChordProgression(beatsElapsed(4* 4 * (4+1)), key);
  // 			  accompanyingChordProgression(beatsElapsed(4* 4 * (4+1)), key);

  // 			kickPattern(beatsElapsed(4* 4*(4+1)));
  // 			kickPattern(beatsElapsed(4* 4 * (4+1)));
  // 			 kickPattern(beatsElapsed(4* 4 * (4+1)));
  // 		kickPattern(beatsElapsed(4* 4 * (4+1)));

  // 	//outro
  // 	  endingMelody(beatsElapsed(4 * 4 * (4+2)), key);
  // 	bassRNG = rand() % 4; //reroll bass pattern RNG
  // 	bassPattern(bassRNG, beatsElapsed(4* 4 * (4+2)), key);
  // 	endingChords(beatsElapsed(4* 4 * (4+2)), key);
  //   }

  //   void playTune(){
  // 	srand((unsigned) time(NULL)); //seed the random number
  // 	int key = rand() % 12; //this is the number of steps we'll transpose the composition up or down
  // 	  int lengthofIntro = 1 + rand() % 4;
  // 	  int HiHatRNG, bassRNG;
  // 	cout << "STEPS FROM A: " << key << endl;

  // 	  for(int intro = lengthofIntro; intro > 0; intro--){ // intro is the number of 4 measure components we'll have of the song before the bridge to the chorus
  // 		  kickPattern(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))));
  // 			kickPattern(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))));
  // 			 kickPattern(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))));
  // 		kickPattern(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))));
  // 		  playHiHat(HiHatRNG, beatsElapsed((4*intro) - (4*(4 - lengthofIntro))));

  // 		  if(intro == 4){ // hi hat and kick only
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 		  }
  // 		  if(intro == 3){ //hi hat kick and bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 		  }
  // 		  if(intro == 2){ //hi hat, kick, chords, accompanyment, bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 			  mainChordProgression(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 			  accompanyingChordProgression(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 		  }
  // 		  if(intro == 1){ //hi hat, kick, chords, accompanyment, bass
  // 			  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 			  mainChordProgression(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 			  accompanyingChordProgression(beatsElapsed((4*intro) - (4*(4 - lengthofIntro))), key);
  // 		  }
  // 	  } //note to self: make this a switch please

  // 	  //bridge
  // 	  playHiHat(3, beatsElapsed(4 * lengthofIntro)); //riser
  // 	  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 	bassPattern(bassRNG, beatsElapsed(4 * lengthofIntro), key);
  // 	  transitionalChords(beatsElapsed(4 * lengthofIntro), key);

  // 	  //chorus (it's the same as case 4.... i'll make it poppier in future)
  // 	  HiHatRNG = rand() % 4; //reroll hi hat RNG
  // 	   playHiHat(HiHatRNG, beatsElapsed(4 * (lengthofIntro+1)));
  // 			  bassRNG = rand() % 4; //reroll bass pattern RNG
  // 			  bassPattern(bassRNG, beatsElapsed(4 * (lengthofIntro+1)), key);
  // 			  mainChordProgression(beatsElapsed(4 * (lengthofIntro+1)), key);
  // 			  accompanyingChordProgression(beatsElapsed(4 * (lengthofIntro+1)), key);

  // 			kickPattern(beatsElapsed(4*(lengthofIntro+1)));
  // 			kickPattern(beatsElapsed(4 * (lengthofIntro+1)));
  // 			 kickPattern(beatsElapsed(4 * (lengthofIntro+1)));
  // 		kickPattern(beatsElapsed(4 * (lengthofIntro+1)));

  // 	//outro
  // 	  endingMelody(beatsElapsed(4 * (lengthofIntro+2)), key);
  // 	bassRNG = rand() % 4; //reroll bass pattern RNG
  // 	bassPattern(bassRNG, beatsElapsed(4 * (lengthofIntro+2)), key);
  // 	endingChords(beatsElapsed(4 * (lengthofIntro+2)), key);

  //   }
};

int main()
{
  // Create app instance
  MyApp app;

  // Set up audio
  app.configureAudio(48000., 512, 2, 0);

  app.start();
  return 0;
}
