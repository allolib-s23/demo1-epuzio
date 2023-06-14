/*
  Original Arrangement of Worst In Me by Girls Rituals

  Objective: simplistic, synthesized retro song that includes lyrics
  Audio is from Synth V (Eleanor Forte)
*/

#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Gamma.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Spatial.h"
#include "Gamma/Types.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/DFT.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/math/al_Random.hpp"
#include "al/sound/al_SoundFile.hpp"

#include "randomnessHelper.h" 
#include <stdlib.h>     //To use to generate random numbers
#include <time.h>       

using namespace al;
using namespace std;
#define FFT_SIZE 4048;

//From Aviv's Demo:
class Vocals : public SynthVoice {
    public:
    SoundFilePlayerTS player;

    vector<float> soundfile_buffer;

    // Mesh for soundwave
    Mesh mSpectrogram;
    vector<float> spectrum;
    gam::STFT stft = gam::STFT(FFT_SIZE, FFT_SIZE/4, 0, gam::HANN, gam::MAG_FREQ);

    // Mesh for shapes
    Mesh mMesh;

    void init() override {
        const char name[] = "worst_in_me.wav";
        if(!player.open(name)){
          std::cerr << "File not found: " << name << std::endl;
          exit(1);
        }

        spectrum.resize(FFT_SIZE/2+1);
        mSpectrogram.primitive(Mesh::LINE_STRIP);

        // addDisc(mMesh, 0.3, 30);
        // mMesh.decompress();
        // mMesh.generateNormals();
    }

    void onProcess(AudioIOData& io) override {
        int frames = (int)io.framesPerBuffer();
        int channels = player.soundFile.channels;
        int bufferLength = frames * channels;
        if ((int)soundfile_buffer.size() < bufferLength) {
        soundfile_buffer.resize(bufferLength);
        }
        player.getFrames(frames, soundfile_buffer.data(), (int)soundfile_buffer.size());
        int second = (channels < 2) ? 0 : 1;
        while (io()) {
            int frame = (int)io.frame();
            int idx = frame * channels;
            io.out(0) = soundfile_buffer[idx];
            io.out(1) = soundfile_buffer[idx + second];
            

            if(stft(soundfile_buffer[idx])){
                // cout << "STFT" << endl;
                // loop through all frequency bins and scale the complex sample
                for(unsigned k = 0; k < stft.numBins(); ++k){
                    spectrum[k] = tanh(pow(stft.bin(k).real(), 1.3));
                }
            }
        }

        if (player.pauseSignal) free();
    }

    void onProcess(Graphics &g) override {

        // mSpectrogram.reset();
        // g.meshColor();
        // for(int i = 0; i < FFT_SIZE/4; i++){
        //     mSpectrogram.color(HSV(spectrum[i] * 50000000));
        //     mSpectrogram.vertex(i, spectrum[i], 0);
        // }

        // for(int i = 1; i <= 4; i++){
        //     double rand_scale_a = al::rnd::uniform(0.7, 1.3);
        //     double rand_scale_b = al::rnd::uniform(0.7, 1.3);
        //     double rand_scale_c = al::rnd::uniform(0.7, 1.3);
        //     mMesh.color(HSV(spectrum[FFT_SIZE/16 * i] * 30000000));
        //     Mesh copyMesh;
        //     copyMesh.copy(mMesh);
        //     copyMesh.scale(rand_scale_a, rand_scale_b, rand_scale_c);
        //     g.meshColor();
        //     g.pushMatrix();
        //     g.translate(-4 + 2 * i, -1, -10);
        //     g.draw(copyMesh);
        //     g.popMatrix();
        // }
        // g.pushMatrix();
        // g.translate(-4, 0, -10);
        // g.scale(100.0/FFT_SIZE, 50, 1.0);
        // g.lineWidth(2);
        // g.draw(mSpectrogram);
        // g.popMatrix();
    }
    void onTriggerOn() override { player.setPlay(); }

    void onTriggerOff() override { 
        player.setPause();
        player.setRewind(); 
    }


};



// From Christine's Arcane demo: https://github.com/allolib-s23/demo1-christinetu15/commit/be12f5e16f4d86acaf9e83267f1c59426d44538e?diff=unified#diff-5bc96b00c5884959857fb7e15d9c746d97d1dc1683e45b2430aeb15a1fab22a8
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
    gam::STFT stft = gam::STFT(4048, 4048 / 4, 0, gam::HANN, gam::MAG_FREQ);
    // This time, let's use spectrograms for each notes as the visual components.
    Mesh mSpectrogram;
    vector<float> spectrum;
    Mesh mMesh;
    double a = 0;
    double b = 0;
    double timepose = 0;
    double spin = al::rnd::uniformS();
    Vec3f note_position;
    Vec3f note_direction;

    virtual void init() override
    {
        // Declare the size of the spectrum
        spectrum.resize(4048 / 2 + 1);
        // mSpectrogram.primitive(Mesh::POINTS);
        mSpectrogram.primitive(Mesh::LINE_STRIP);
        mAmpEnv.levels(0, 1, 1, 0);
        mPanEnv.curve(2);
        env.decay(0.03);
        delay.maxDelay(1. / 27.5);
        delay.delay(1. / 900.0);

        addOctahedron(mMesh, .6);

        createInternalTriggerParameter("amplitude", 0.0, 0.0, 1.0);
        createInternalTriggerParameter("frequency", 60, 20, 5000);
        createInternalTriggerParameter("attackTime", 0.001, 0.001, 1.0);
        createInternalTriggerParameter("releaseTime", .1, 0.1, 10.0);
        createInternalTriggerParameter("sustain", 0.25, 0.0, 1.0);
        createInternalTriggerParameter("Pan1", 0.0, -1.0, 1.0);
        createInternalTriggerParameter("Pan2", 0.0, -1.0, 1.0);
        createInternalTriggerParameter("PanRise", 3, 0, 3.0); // range check
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
            // STFT for each notes
            if (stft(s1))
            { // Loop through all the frequency bins
                for (unsigned k = 0; k < stft.numBins(); ++k)
                {
                    // Here we simply scale the complex sample
                    spectrum[k] = tanh(pow(stft.bin(k).real(), 1.3));
                }
            }
        }
        if (mAmpEnv.done() && (mEnvFollow.value() < 0.001))
            free();
    }

    // The graphics processing function
    void onProcess(Graphics &g) override
    {
        a += spin;
        b += spin;
        timepose += 0.02;
        // Get the paramter values on every video frame, to apply changes to the
        // current instance
        float frequency = getInternalParameterValue("frequency");
        float amplitude = getInternalParameterValue("amplitude");
        // Now draw
        g.pushMatrix();
        g.depthTesting(true);
        g.lighting(true);
        g.translate(note_position + note_direction * timepose);
        g.rotate(a, Vec3f(0, 1, 0));
        g.rotate(b, Vec3f(1));
        g.scale(0.3 + mAmpEnv() * 0.2, 0.3 + mAmpEnv() * 0.5, amplitude);
        g.color(HSV(frequency / 1000, 0.5 + mAmpEnv() * 0.1, 0.3 + 0.5 * mAmpEnv()));
        g.draw(mMesh);
        g.popMatrix();
    }

    virtual void onTriggerOn() override
    {
        mAmpEnv.reset();
        timepose = 0;
        updateFromParameters();
        env.reset();
        delay.zero();
        mPanEnv.reset();
        note_position = {0, 0, -15};
        float angle = getInternalParameterValue("frequency") / 200;
        note_direction = {sin(angle), cos(angle), 0};
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
        mPanEnv.lengths()[0] = mPanRise;
        mPanEnv.lengths()[1] = mPanRise;
    }
};

//from https://github.com/allolib-s23/demo1-christinetu15/commit/be12f5e16f4d86acaf9e83267f1c59426d44538e?diff=unified#diff-5bc96b00c5884959857fb7e15d9c746d97d1dc1683e45b2430aeb15a1fab22a8
class SquareWave : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::Sine<> mOsc1;
  gam::Sine<> mOsc3;
  gam::Sine<> mOsc5;
  gam::Sine<> mOsc7;

  gam::Env<3> mAmpEnv;
  gam::EnvFollow<> mEnvFollow;
  Mesh mMesh;
  double a = 0;
    double b = 0;
    double timepose = 0;
    Vec3f note_position;
    Vec3f note_direction;


  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued
    addOctahedron(mMesh, .4);
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
    mOsc7.freq(f * 7);

    float a = getInternalParameterValue("amplitude");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    mPan.pos(getInternalParameterValue("pan"));
    while (io())
    {
      float s1 = mAmpEnv() * (mOsc1() * a +
                              mOsc3() * (a / 3.0) +
                              mOsc5() * (a / 5.0) +
                              mOsc7() * (a / 7.0));

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

  // The graphics processing function
  void onProcess(Graphics &g) override
  {
    // empty if there are no graphics to draw
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    timepose += 0.01;
    // Now draw
    g.pushMatrix();
    g.translate(note_position + note_direction * timepose);
    g.translate(sin(static_cast<double>(frequency)), cos(static_cast<double>(frequency)), -8);
    g.scale(frequency / 5000, frequency / 5000, frequency / 5000);
    g.color(frequency / 1000, 50, 200, 0.4);
    g.draw(mMesh);
    g.popMatrix();

    g.pushMatrix();
    g.translate(note_position + note_direction * timepose);
    g.translate(sin(static_cast<double>(frequency)), cos(static_cast<double>(frequency)), -12);
    g.scale(frequency / 5000, 2 * frequency / 5000, frequency / 5000);
    g.color(80, frequency / 1000, 50, 0.4);
    g.draw(mMesh);
    g.popMatrix();

    g.pushMatrix();
    g.translate(cos(static_cast<double>(frequency)), sin(static_cast<double>(frequency)), -16);
    g.scale(frequency / 5000, 3 * frequency / 5000, frequency / 5000);
    g.color(150, 110, frequency / 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();

    g.pushMatrix();
    g.translate(cos(static_cast<double>(frequency)), sin(static_cast<double>(frequency)), -20);
    g.scale(frequency / 5000, 3 * frequency / 5000, frequency / 5000);
    g.color(150, 110, frequency / 10, 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override { mAmpEnv.reset(); }
  void onTriggerOff() override { mAmpEnv.release(); }
};

// From https://github.com/allolib-s21/notes-Mitchell57:
class HiHat : public SynthVoice
{public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Changed amp envelope from Env<3> to AD<>
  gam::Burst mBurst; // Resonant noise with exponential decay

  // envelope follower to connect audio output to graphics
  gam::EnvFollow<> mEnvFollow;
  // Additional members
  Mesh mMesh;
  
  void init() override {
    // Initialize burst - Main freq, filter freq, duration
    mBurst = gam::Burst(20000, 15000, 0.05);

    // We have the mesh be a rectangle
    addSphere(mMesh);

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
    createInternalTriggerParameter("beats", 0.0, 0.0, 3.0);
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

  // The graphics processing function
  void onProcess(Graphics &g) override {
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    float beats = getInternalParameterValue("beats");
    // Now draw
    g.pushMatrix();
    // Move x according to frequency, y according to amplitude
    g.translate(beats - 1.5, -0.5, -8);
    // Scale in the x and y directions according to amplitude
    g.scale(0.5 * amplitude, 0.5 * amplitude, 1);
    // Set the color. Red and Blue according to sound amplitude and Green
    // according to frequency. Alpha fixed to 0.4
    g.color(frequency / 100, mEnvFollow.value(),  mEnvFollow.value(), 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  void onTriggerOn() override { mBurst.reset(); }
  //void onTriggerOff() override {  }
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
        createInternalTriggerParameter("amplitude", 0.8, 0.0, 1.0);
        createInternalTriggerParameter("frequency", 250, 20, 5000);
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
class Snare : public SynthVoice
{
public:
  // Unit generators
  gam::Pan<> mPan;
  gam::AD<> mAmpEnv; // Amplitude envelope
  gam::Sine<> mOsc; // Main pitch osc (top of drum)
  gam::Sine<> mOsc2; // Secondary pitch osc (bottom of drum)
  gam::Decay<> mDecay; // Pitch decay for oscillators
  // gam::ReverbMS<> reverb;	// Schroeder reverberator
  gam::Burst mBurst; // Noise to simulate rattle/chains

  // envelope follower to connect audio output to graphics
  gam::EnvFollow<> mEnvFollow;
  // Additional members
  Mesh mMesh;

  void init() override {
    // Initialize burst 
    mBurst = gam::Burst(10000, 5000, 0.3);

    // Initialize amplitude envelope
    mAmpEnv.attack(0.01);
    mAmpEnv.decay(0.01);
    mAmpEnv.amp(1.0);

    // Initialize pitch decay 
    mDecay.decay(0.8);

    // reverb.resize(gam::FREEVERB);
	  // 	reverb.decay(0.5); // Set decay length, in seconds
	  // 	reverb.damping(0.2); // Set high-frequency damping factor in [0, 1]

    // We have the mesh
    addDisc(mMesh, 1, 30);

    createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
    createInternalTriggerParameter("frequency", 60, 20, 5000);
  }

  // The audio processing function
  void onProcess(AudioIOData& io) override {
    mOsc.freq(200);
    mOsc2.freq(150);

    while (io()) {
      float decay = mDecay();
      mOsc.freqMul(decay);
      mOsc2.freqMul(decay);

      float amp = mAmpEnv();
      float s1 = mBurst() + (mOsc() * amp * 0.1)+ (mOsc2() * amp * 0.05);
      // s1 += reverb(s1) * 0.2;
      float s2;
      mPan(s1, s1, s2);
      io.out(0) += s1;
      io.out(1) += s2;
    }
    
    if (mAmpEnv.done()) free();
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
    g.translate(0, 0, 0);
    // Scale in the x and y directions according to amplitude
    g.scale(1, 1, 1);
    g.color(mEnvFollow.value(), mEnvFollow.value(),  mEnvFollow.value(), 0.4);
    g.draw(mMesh);
    g.popMatrix();
  }

  void onTriggerOn() override { mBurst.reset(); mAmpEnv.reset(); mDecay.reset();}
  void onTriggerOff() override { mAmpEnv.release(); mDecay.finish(); }
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
  // Draw parameters
  Mesh mMesh;
  double a;
  double b;
  double spin = al::rnd::uniformS();
  double timepose = 0;
  Vec3f note_position;
  Vec3f note_direction;

  // Additional members
  // Initialize voice. This function will only be called once per voice when
  // it is created. Voices will be reused if they are idle.
  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued

    // We have the mesh be a sphere
    addEllipse(mMesh, 0.3, 50, 50);
    mMesh.decompress();
    mMesh.generateNormals();

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
      free();
  }

  // The graphics processing function
  void onProcess(Graphics &g) override
  {
    a += spin;
    b += spin;
    timepose += 0.02;
    // Get the paramter values on every video frame, to apply changes to the
    // current instance
    float frequency = getInternalParameterValue("frequency");
    float amplitude = getInternalParameterValue("amplitude");
    // Now draw
    g.pushMatrix();
    // g.depthTesting(true);
    // g.lighting(true);
    g.translate(note_position + note_direction * timepose);
    g.rotate(a, Vec3f(0, 1, 0));
    g.rotate(b, Vec3f(1));
    g.scale(0.3 + mAmpEnv() * 0.2, 0.3 + mAmpEnv() * 0.5, amplitude);
    g.color(HSV((frequency / 1000)/5, (0.5 + mAmpEnv() * 0.1)/5, (0.3 + 0.5 * mAmpEnv())/5));
    g.draw(mMesh);
    g.popMatrix();
  }

  // The triggering functions just need to tell the envelope to start or release
  // The audio processing function checks when the envelope is done to remove
  // the voice from the processing chain.
  void onTriggerOn() override
  {
    float angle = getInternalParameterValue("frequency") / 200;
    mAmpEnv.reset();
    a = al::rnd::uniform();
    b = al::rnd::uniform();
    timepose = 0;
    note_position = {0, 0, 0};
    note_direction = {sin(angle), cos(angle), 0};
  }

  void onTriggerOff() override { mAmpEnv.release(); }
};


// We make an app.
class MyApp : public App
{
public:
  SynthGUIManager<SineEnv> synthManager {"synth8"};
  //    ParameterMIDI parameterMIDI;

  virtual void onInit( ) override {
    imguiInit();
    navControl().active(false);  // Disable navigation via keyboard, since we
                              // will be using keyboard for note triggering
    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
  }

    void onCreate() override {
        // Play example sequence. Comment this line to start from scratch
        //    synthManager.synthSequencer().playSequence("synth8.synthSequence");

        // bossa(0.0, 150, 64);

        synthManager.synthRecorder().verbose(true);
    }

    void onSound(AudioIOData& io) override {
        synthManager.render(io);  // Render audio
    }

    void onAnimate(double dt) override {
        imguiBeginFrame();
        synthManager.drawSynthControlPanel();
        imguiEndFrame();
    }

    void onDraw(Graphics& g) override {
        g.clear();
        synthManager.render(g);

        // Draw GUI
        imguiDraw();
    }

    bool onKeyDown(Keyboard const& k) override {
        // if (ParameterGUI::usingKeyboard()) {  // Ignore keys if GUI is using them
        // return true;
        // }
        playTune();
        return true;
    }

    bool onKeyUp(Keyboard const& k) override {
        int midiNote = asciiToMIDI(k.key());
        if (midiNote > 0) {
        synthManager.triggerOff(midiNote);
        }
        return true;
    }

    void onExit() override { imguiShutdown(); }
    // INSTRUMENTS
    //  From Professor Conrad's Frere Jacques Demo:
    void playNote(float freq, float time, float duration, float amp = .2, float attack = 0.01, float decay = 0.01)
    {
        auto *voice = synthManager.synth().getVoice<SquareWave>();
        // amp, freq, attack, release, pan
        vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.0, 0.0, 0.0});
        voice->setTriggerParams(params);
        synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
    }

    // From Mitchell's drums class:
    void playKick(float freq, float time, float duration = 0.5, float amp = 0.9, float attack = 0.01, float decay = 0.1)
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

    // From Hunter's plucked string demo:
    void playBass(float freq, float time, float duration, float amp = .2, float attack = 0.9, float decay = 0.1)
    {
        auto *voice = synthManager.synth().getVoice<PluckedString>();
        voice->setInternalParameterValue("frequency", freq);
        voice->setInternalParameterValue("amplitude", amp);
        synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
    }

    //  Modified from Professor Conrad's Frere Jacques Demo:
    void playSine(float freq, float time, float duration, float amp = .2, float attack = 0.01, float decay = 0.01)
    {
        auto *voice = synthManager.synth().getVoice<SineEnv>();
        // amp, freq, attack, release, pan
        vector<VariantValue> params = vector<VariantValue>({amp, freq, 0.0, 0.0, 0.0});
        voice->setTriggerParams(params);
        synthManager.synthSequencer().addVoiceFromNow(voice, time, duration);
    }

    // HELPER FUNCTIONS, CONSTS:
    float bpm = 110;
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

    // kick drum:
    void kickPattern(int sequenceStart)
    { // 24 measures of the same kick drum pattern
        for (int i = 0; i < 16; i++)
        {
            playKick(400, beatsElapsed(0 + (i * 4) + sequenceStart));
            playKick(300, beatsElapsed(1 + (i * 4) + sequenceStart));
            playKick(150, beatsElapsed(2.5 + (i * 4) + sequenceStart));
            playKick(300, beatsElapsed(3 + (i * 4) + sequenceStart));
        }
    }

    // snare drum:
    void hiHatPattern(int sequenceStart)
    { // 24 measures of the same syncopated hi hat pattern
        for (int i = 0; i < 16; i++)
        {
            playHiHat(beatsElapsed(1 + (i * 4) + sequenceStart));
            playHiHat(beatsElapsed(3 + (i * 4) + sequenceStart));
        }
    }

    void lfsrSnare(int sequenceStart){
      srand(time(NULL));
      vector<bool> seq = getFibLFSRSequence(rand()%31+1, 5, 64);
      for(int i = 0; i < 24; i++){
          cout << seq[i] << endl;
          if(seq[i]){
              playSnare(beatsElapsed(i));
          }
      }
    }

    // SONG:
    void playTune()
    {
      kickPattern(0);
      hiHatPattern(0);
      lfsrSnare(0);
      vector<float> melodyNotes = getMarkovNotes("C", 4, 0, 60);

      //melody:
      for(int i = 0; i < 64; i++){
        cout << melodyNotes[i] << endl;
        playBass(melodyNotes[i], beatsElapsed(i/2.0), eighth);
      }

      //accompaniment:
      vector<bool> chordOut = getFibLFSRSequence(rand()%31+1, 5, 16); //LFSR to randomly play a chord every other beat
      for(int i = 0; i < 64; i++){
        if(i % 2 == 0){
          if(chordOut[i]){
            playNote(melodyNotes[i], beatsElapsed(i/2.0), quarter);
            playNote(melodyNotes[i+1], beatsElapsed(i/2.0), quarter);
          }
        } 
      }

      //atmospheric chords:
      vector<bool> atmosphericChordOut = getFibLFSRSequence(rand()%31+1, 5, 16); //LFSR to randomly play a chord every other beat
      for(int i = 0; i < 64; i++){
        if(i % 8 == 0){
          if(atmosphericChordOut[i]){
            playNote(melodyNotes[i], beatsElapsed(i/2.0), whole*2, .1);
            playNote(melodyNotes[i+1], beatsElapsed(i/2.0), whole*2, .1);
            playNote(melodyNotes[i+2], beatsElapsed(i/2.0), whole*2, .1);
          }
        } 
      }
  }
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