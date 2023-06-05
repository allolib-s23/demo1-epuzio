#include <string>
#include <vector>
#include <random>
#include <iostream>
using namespace std;

//LINEAR FEEDBACK SHIFT REGISTER: SEMI-RANDOM PATTERNS
bool fibLFSR(int seed, int taps, int numBits){ 
    //Linear Feedback Shift Register (based on the Fibonacci LFSR, Wikipedia: https://en.wikipedia.org/wiki/Linear-feedback_shift_register)
    //helper to xor every bit in an integer of length numBits
    bool output = 0;
    for(int i = 0; i < numBits; i++){
        output = output ^ ((seed ^ taps)>>i); //xor each bit in input sequentially
    }
    return output;
}

int getLFSRTaps(int length){
    //Taps for m-sequence are taken from Wikipedia: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
    //length field corresponds to how many bits are in the desired LFSR cycle 
    //in binary, the output for getLFSRTaps holds 1 at a given position if we use that position to calculate the output for the next cycle,
    //and 0 if we don't use the value at that position to calculate the output. 
    unordered_map<int, int> taps{
        {2, 0x3}, {3, 0x6}, {4, 0xC}, {5, 0x14}, {6, 0x30}, {7, 0x60}, {8, 0xB8},
        {9, 0x110}, {10, 0x240}, {11, 0x500}, {12, 0xE08}, {13, 0x1C80}, {14, 0x3802},
        {15, 0x6000}, {16, 0xD008}
    };
    return taps[length];
}

vector<bool> getFibLFSRSequence(int seed, int numBits, int sequenceLength){
    //input is the seed for the LFSR, cycleLength is the number of bits in the LFSR,
    // sequenceLength is how many output bits we want to generate
    //Reference: https://simple.wikipedia.org/wiki/Linear-feedback_shift_register
    int taps = getLFSRTaps(numBits);
    int prevSequence = seed;
    vector<bool> output;
    for(int i = 0; i < sequenceLength; i++){
        bool outputBit = fibLFSR(prevSequence, taps, numBits);
        output.push_back(outputBit);
        prevSequence = (prevSequence>>1) | (outputBit<<numBits);
    }
    return output;
}




//TRANSPOSITION AND CHORD PROGRESSIONS

float getDist(string n){ //
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    return notes[n];
}

float getFreqFromDist(int distance){
    return 27.50 * pow(2, (distance)/12.0);
}

//To make playing notes a little easier - referenced https://gist.github.com/stuartmemo/3766449 and http://techlib.com/reference/musical_note_frequencies.html for formula
float getFreq(string n, int octave, int transpose = 0){
    float dist = getDist(n) + transpose + ((octave - 1) * 12);
    float frequency = getFreqFromDist(dist);
    return frequency;
}

float getFreq(int n, int octave, int transpose = 0){
    float dist = n + transpose + ((octave - 1) * 12);
    float frequency = getFreqFromDist(dist);
    return frequency;
}

vector<float> getFifthChordFreqs(string n, int octave, int transpose = 0, int inversion = 0){
    float rootFreq = 0, thirdFreq = 0, fifthFreq = 0;
    vector<float> frequencies;
    float dist = getDist(n) + transpose + ((octave - 1) * 12);
    rootFreq = getFreqFromDist(dist);
    
    switch(inversion){
        case 0: //Root Position
            thirdFreq = getFreqFromDist(dist + 4);
            fifthFreq = getFreqFromDist(dist + 7);
            break;
        case 1: // First Inversion
            thirdFreq = getFreqFromDist(dist - 8);
            fifthFreq = getFreqFromDist(dist - 5);
            break;
        case 2: //Second Inversion
            thirdFreq = getFreqFromDist(dist + 4);
            fifthFreq = getFreqFromDist(dist - 5);
            break;
    }
    frequencies.push_back(rootFreq);
    frequencies.push_back(thirdFreq);
    frequencies.push_back(fifthFreq);
    return frequencies;
}

vector<vector<float>> axisProgression(string n, int octave, int transpose = 0){
    //Referenced: https://en.wikipedia.org/wiki/I%E2%80%93V%E2%80%93vi%E2%80%93IV_progression
    //Return chords corresponding to 1, 5, 4, 6 from semi-preset order w/ random inversions
    
    srand((unsigned)time(NULL));
    vector<float> c1 = getFifthChordFreqs(n, octave, transpose + 0, rand() % 3);
    vector<float> c4 = getFifthChordFreqs(n, octave, transpose + 5, rand() % 3);
    vector<float> c5 = getFifthChordFreqs(n, octave, transpose + 7, rand() % 3);
    vector<float> c6 = getFifthChordFreqs(n, octave, transpose + 9, rand() % 3);
    unordered_map<int, vector<vector<float>>> axisProgs{
        {0, {c1, c5, c6, c4}}, {1, {c5, c6, c4, c1}}, {2, {c6, c4, c1, c5}}, {3, {c4, c1, c5, c6}}
    };
    vector<vector<float>> output = axisProgs[rand() % 4];
    return output;
}




//MARKOV CHAINS CHORD PROGRESSIONS:
//referenced https://sites.math.washington.edu/~conroy/m381-general/markovMusic/markovMusic.htm
//and https://scholarship.claremont.edu/cgi/viewcontent.cgi?article=1848&context=jhm
vector<float> markovNotes(string n, int octave, int transpose, int sequenceLength){
    //The following probabilities are heavily weighted towards a major 7 chord starting at the note A
    //(1 = .36, 3 = .2, 5 = .25, 7 = .1)
    //The remaining .1 percent is divided between minor third, tritone, raised fifth
    vector<int> noteTransitionMatrix = {36, 0, 0, 3, 20, 0, 3, 25, 3, 0, 0, 20};

    //Go down an octave, stay at current octave, or go up an octave?
    vector<int> octaveTransitionMatrix = {10, 85, 5};

    random_device rd;
    mt19937 gen(rd());
    discrete_distribution<> nextNote(noteTransitionMatrix.begin(), noteTransitionMatrix.end());
    discrete_distribution<> nextOctave(octaveTransitionMatrix.begin(), octaveTransitionMatrix.end());
    vector<float> frequencies;
    
    int nN, nO;
    for(int i = 0; i < sequenceLength; i++){
        nN = nextNote(gen);
        nO = nextOctave(gen);
        frequencies.push_back(getFreq(nN + transpose, nO, 0));
    }   
    return frequencies;
} 
