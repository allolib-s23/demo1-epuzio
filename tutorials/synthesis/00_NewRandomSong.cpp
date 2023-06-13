/*
  Original pseudo-randomized song (DogLove)

  Inspired (and partially borrowed from) from Devi McCallion's work as Girls Rituals and Mom
  Songs referenced: A.T. Field, Needle Mountain
*/

#include <cstdio> // for printing to stdout
#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"
#include "Gamma/DFT.h"

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/scene/al_SynthSequencer.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/math/al_Random.hpp"

#include "randomnessHelper.h" //theory class I wrote to transpose chords/notes
#include <stdlib.h>     //To use to generate random numbers
#include <time.h>       //To use to generate random numbers

using namespace al;
using namespace std;

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

    Mesh mSpectrogram;
    vector<float> spectrum;
    double a = 0;
    double b = 0;
    double timepose = 10;

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
        // float frequency = getInternalParameterValue("frequency");
        // float amplitude = getInternalParameterValue("amplitude");
        // g.pushMatrix();
        // g.translate(amplitude, amplitude, -4);
        // // g.scale(frequency/2000, frequency/4000, 1);
        // float scaling = 0.1;
        // g.scale(scaling * frequency / 200, scaling * frequency / 400, scaling * 1);
        // g.color(mEnvFollow.value(), frequency / 1000, mEnvFollow.value() * 10, 0.4);
        // g.draw(mMesh);
        // g.popMatrix();
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

// From https://github.com/allolib-s21/notes-Mitchell57:
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
        mBurst = gam::Burst(20000, 15000, 0.2);
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

using namespace gam;
using namespace al;
using namespace std;
class Sub : public SynthVoice {
public:

    // Unit generators
    float mNoiseMix;
    gam::Pan<> mPan;
    gam::ADSR<> mAmpEnv;
    gam::EnvFollow<> mEnvFollow;  // envelope follower to connect audio output to graphics
    gam::DSF<> mOsc;
    gam::NoiseWhite<> mNoise;
    gam::Reson<> mRes;
    gam::Env<2> mCFEnv;
    gam::Env<2> mBWEnv;
    // Additional members
    Mesh mMesh;

    // Initialize voice. This function will nly be called once per voice
    void init() override {
        mAmpEnv.curve(0); // linear segments
        mAmpEnv.levels(0,1.0,1.0,0); // These tables are not normalized, so scale to 0.3
        mAmpEnv.sustainPoint(2); // Make point 2 sustain until a release is issued
        mCFEnv.curve(0);
        mBWEnv.curve(0);
        mOsc.harmonics(12);
        // We have the mesh be a sphere
        addDisc(mMesh, 1.0, 30);

        createInternalTriggerParameter("amplitude", 0.3, 0.0, 1.0);
        createInternalTriggerParameter("frequency", 60, 20, 5000);
        createInternalTriggerParameter("attackTime", 0.1, 0.01, 3.0);
        createInternalTriggerParameter("releaseTime", 3.0, 0.1, 10.0);
        createInternalTriggerParameter("sustain", 0.7, 0.0, 1.0);
        createInternalTriggerParameter("curve", 4.0, -10.0, 10.0);
        createInternalTriggerParameter("noise", 0.0, 0.0, 1.0);
        createInternalTriggerParameter("envDur",1, 0.0, 5.0);
        createInternalTriggerParameter("cf1", 400.0, 10.0, 5000);
        createInternalTriggerParameter("cf2", 400.0, 10.0, 5000);
        createInternalTriggerParameter("cfRise", 0.5, 0.1, 2);
        createInternalTriggerParameter("bw1", 700.0, 10.0, 5000);
        createInternalTriggerParameter("bw2", 900.0, 10.0, 5000);
        createInternalTriggerParameter("bwRise", 0.5, 0.1, 2);
        createInternalTriggerParameter("hmnum", 12.0, 5.0, 20.0);
        createInternalTriggerParameter("hmamp", 1.0, 0.0, 1.0);
        createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);

    }

    //
    
    virtual void onProcess(AudioIOData& io) override {
        updateFromParameters();
        float amp = getInternalParameterValue("amplitude");
        float noiseMix = getInternalParameterValue("noise");
        while(io()){
            // mix oscillator with noise
            float s1 = mOsc()*(1-noiseMix) + mNoise()*noiseMix;

            // apply resonant filter
            mRes.set(mCFEnv(), mBWEnv());
            s1 = mRes(s1);

            // appy amplitude envelope
            s1 *= mAmpEnv() * amp;

            float s2;
            mPan(s1, s1,s2);
            io.out(0) += s1;
            io.out(1) += s2;
        }
        
        
        if(mAmpEnv.done() && (mEnvFollow.value() < 0.001f)) free();
    }

   virtual void onProcess(Graphics &g) {
          float frequency = getInternalParameterValue("frequency");
          float amplitude = getInternalParameterValue("amplitude");
          g.pushMatrix();
          g.translate(amplitude,  amplitude, -4);
          //g.scale(frequency/2000, frequency/4000, 1);
          float scaling = 0.1;
          g.scale(scaling * frequency/200, scaling * frequency/400, scaling* 1);
          g.color(mEnvFollow.value(), frequency/1000, mEnvFollow.value()* 10, 0.4);
          g.draw(mMesh);
          g.popMatrix();
   }
    virtual void onTriggerOn() override {
        updateFromParameters();
        mAmpEnv.reset();
        mCFEnv.reset();
        mBWEnv.reset();
        
    }

    virtual void onTriggerOff() override {
        mAmpEnv.triggerRelease();
//        mCFEnv.triggerRelease();
//        mBWEnv.triggerRelease();
    }

    void updateFromParameters() {
        mOsc.freq(getInternalParameterValue("frequency"));
        mOsc.harmonics(getInternalParameterValue("hmnum"));
        mOsc.ampRatio(getInternalParameterValue("hmamp"));
        mAmpEnv.attack(getInternalParameterValue("attackTime"));
    //    mAmpEnv.decay(getInternalParameterValue("attackTime"));
        mAmpEnv.release(getInternalParameterValue("releaseTime"));
        mAmpEnv.levels()[1]=getInternalParameterValue("sustain");
        mAmpEnv.levels()[2]=getInternalParameterValue("sustain");

        mAmpEnv.curve(getInternalParameterValue("curve"));
        mPan.pos(getInternalParameterValue("pan"));
        mCFEnv.levels(getInternalParameterValue("cf1"),
                      getInternalParameterValue("cf2"),
                      getInternalParameterValue("cf1"));


        mCFEnv.lengths()[0] = getInternalParameterValue("cfRise");
        mCFEnv.lengths()[1] = 1 - getInternalParameterValue("cfRise");
        mBWEnv.levels(getInternalParameterValue("bw1"),
                      getInternalParameterValue("bw2"),
                      getInternalParameterValue("bw1"));
        mBWEnv.lengths()[0] = getInternalParameterValue("bwRise");
        mBWEnv.lengths()[1] = 1- getInternalParameterValue("bwRise");

        mCFEnv.totalLength(getInternalParameterValue("envDur"));
        mBWEnv.totalLength(getInternalParameterValue("envDur"));
    }
};

// We make an app.
class MyApp : public App
{
public:
    SynthGUIManager<Sub> synthManager{"GenDemo"};

    virtual void onInit( ) override {
    imguiInit();
    navControl().active(false);  // Disable navigation via keyboard, since we
                              // will be using keyboard for note triggering
    // Set sampling rate for Gamma objects from app's audio
    gam::sampleRate(audioIO().framesPerSecond());
  }

    void onCreate() override
    {
        navControl().active(false);                   // Disable navigation via keyboard
        gam::sampleRate(audioIO().framesPerSecond()); // Set sampling rate for Gamma objects from app's audio

        imguiInit();
        
        synthManager.synthRecorder().verbose(true);
    }

    // The audio callback function. Called when audio hardware requires data
    void onSound(AudioIOData &io) override
    {
        synthManager.render(io); // Render audio
    }

    void onAnimate(double dt) override
    {
        imguiBeginFrame();                    // The GUI is prepared here
        synthManager.drawSynthControlPanel(); // Draw a window that contains the synth control panel
        imguiEndFrame();
    }

    // The graphics callback function.
    void onDraw(Graphics &g) override
    {
        // g.clear();
        // synthManager.render(g); // Render the synth's graphics
        // imguiDraw();            // GUI is drawn here
    }

    // Whenever a key is pressed, this function is called
    bool onKeyDown(Keyboard const &k) override
    {
        playTune();
        return true;
    }

    // Whenever a key is released this function is called
    bool onKeyUp(Keyboard const &k) override
    {
        return true;
    }

    void onExit() override { imguiShutdown(); }

    // INSTRUMENTS
    //  From https://github.com/allolib-s23/demo1-zachmiller-ucsb/blob/main/tutorials/synthesis/demo3.cpp
    void playNote(string preset, float freq, float time = 0.0, float duration = 2, float amp = 0.4, float attack_time = 0.5, float release_time = 0.5, float pan = 0.0) {
        // if (preset_voices.find(preset) == preset_voices.end()) {
        //     cout << "this preset does not exist in the specified folder" << endl;
        //     return;
        // }
        Sub *voice = preset_voices[preset];
        voice->setInternalParameterValue("frequency", freq);
        voice->setInternalParameterValue("amplitude", amp);
        voice->setInternalParameterValue("attackTime", attack_time);
        voice->setInternalParameterValue("releaseTime", release_time);
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

    // HELPER FUNCTIONS, CONSTS:
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

    void playChord(vector<float> freq, int numNotes, float playTime, float sus)
    {
        
        for (auto f : freq)
        {
            playNote(f, playTime, sus, (.2 / numNotes));
        }
    }

    void playChoralSynth(float freq, float playTime, float offset, float sus)
    {
        playNote(freq, playTime, sus, .1);
        playNote(freq + 1, playTime, sus, .1);
        playNote(freq + 2, playTime, sus, .1);
    }

    void playBassProgresssion(vector<vector<float>> ap, int startSequence){ //16 measures
        for(int i = 0; i < 2; i++){
            playBass(ap[0][0], beatsElapsed(i+startSequence), whole);
            playBass(ap[1][0], beatsElapsed(i+1+startSequence), whole);
            playBass(ap[2][0], beatsElapsed(i+2+startSequence), whole);
            playBass(ap[3][0], beatsElapsed(i+3.75+startSequence), eighth);

            playBass(ap[0][0], beatsElapsed(i+4+startSequence), whole);
            playBass(ap[1][0], beatsElapsed(i+5+startSequence), whole);
            playBass(ap[3][0], beatsElapsed(i+6+startSequence), whole+whole);
        }
    }

    void playBasicDrums(int startSequence){ //16 measures, syncopated
        for(int i = 0; i < 4; i++){
            playKick(300, beatsElapsed(startSequence+i+1));
            playKick(150, beatsElapsed(startSequence+i+3));
        }
    }


    // SONG:
    void playTune()
    {
        vector<bool> out  = getFibLFSRSequence(0b1001, 4, 16);
        // for(int i = 0; i < 16; i++){
        //     cout << out[i] << endl;
        // }
        // srand((unsigned)time(NULL)); // seed the random number
        // int key = rand() % 12;
        // vector<vector<float>> chordProgression = axisProgression("C", 3, key);   
        // playBassProgresssion(chordProgression, 0); //plays 
        // playBasicDrums(0);

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