#include <string>
#include <vector>
#include "al/math/al_Random.hpp"
using namespace std;

//LINEAR FEEDBACK SHIFT REGISTER: SEMI-RANDOM PATTERNS
bool fibLFSR(int seed, int taps, int numBits){ 
    //Linear Feedback Shift Register (based on the Fibonacci LFSR, Wikipedia: https://en.wikipedia.org/wiki/Linear-feedback_shift_register)
    //helper to xor every bit in an integer of length numBits
    bool output = 0;
    for(int i = 0; i < numBits; i++){
        output = output ^ ((seed & (taps) & (1 << i))>>i); //xor each bit in input sequentially
        cout << ((seed & (taps) & (1 << i))>>i) << " " << output <<endl;
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
    int input = seed;
    vector<bool> output;
    for(int i = 0; i < sequenceLength; i++){
        cout << "input: " << input << endl;
        bool outputBit = fibLFSR(seed, taps, numBits);
        output.push_back(outputBit);
        input = ((input<<1) & ~(1<<(numBits))) | outputBit; //0xFFF... ensures that input doesn't get too big
        cout << "Next bit: " << outputBit << endl;
    }
    return output;
}


//MARKOV CHAINS CHORD PROGRESSIONS:
//referenced https://sites.math.washington.edu/~conroy/m381-general/markovMusic/markovMusic.htm
//and https://scholarship.claremont.edu/cgi/viewcontent.cgi?article=1848&context=jhm
//and https://web.archive.org/web/20120206123155/http://www.cs.northwestern.edu/~pardo/publications/pardo-birmingham-aaai-05.pdf
vector<float> markovNotes(string seed, int sequenceLength){
    //The following probabilities are heavily weighted towards major 7 chords starting at each note
    //(1 = .36, 3 = .2, 5 = .25, 7 = .1)
    //The remaining .1 percent is divided between minor third, tritone, raised fifth
    vector<vector<float>> noteTransitionMatrix = {
        {.36, 0, 0, .03, .2, 0, .03, .25, .03, 0, 0, .2}, //A
		{.2, .36, 0, 0, .03, .2, 0, .03, .25, .03, 0, 0}, //A# / Bb
		{0, .2, .36, 0, 0, .03, .2, 0, .03, .25, .03, 0}, //B
		{0, 0, .2, .36, 0, 0, .03, .2, 0, .03, .25, .03}, //C
		{.03, 0, 0, .2, .36, 0, 0, .03, .2, 0, .03, .25}, // C# / Db
		{.25, .03, 0, 0, .2, .36, 0, 0, .03, .2, 0, .03}, // D 
		{.03, .25, .03, 0, 0, .2, .36, 0, 0, .03, .2, 0}, // D# / Eb
		{0, .03, .25, .03, 0, 0, .2, .36, 0, 0, .03, .2}, //E
		{.2, 0, .03, .25, .03, 0, 0, .2, .36, 0, 0, .03}, //F #
        {.03, .2, 0, .03, .25, .03, 0, 0, .2, .36, 0, 0}, //F
		{0, .03, .2, 0, .03, .25, .03, 0, 0, .2, .36, 0}, // G
		{0, 0, .03, .2, 0, .03, .25, .03, 0, 0, .2, .36}}; //G# / Ab

    //Go down an octave, stay at current octave, or go up an octave?
    vector<vector<float>> octaveTransitionMatrix = {
        {0.2, 0.75, 0.05}, //A
        {0.2, 0.75, 0.05}, //Bb
        {0.15, 0.8, 0.05}, //B
        {0.15, 0.8, 0.05}, //C
        {0.1, 0.85, 0.05}, //C#
        {0.1, 0.85, 0.05}, //D
        {0.1, 0.80, 0.1}, //D#
        {0.1, 0.80, 0.1}, //E
        {0.05, 0.80, 0.15}, //F
        {0.05, 0.80, 0.15}, //F#
        {0.05, 0.75, 0.2}, //G
        {0.05, 0.75, 0.2}, //G#
    };

    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };

    for(int i = 0; i < sequenceLength; i++){

    }
        
} 





//TRANSPOSITION AND CHORD PROGRESSIONS
float freqFromA(int distance){
    return 27.50 * pow(2, (distance)/12.0);
}

//To make playing notes a little easier - referenced https://gist.github.com/stuartmemo/3766449 and http://techlib.com/reference/musical_note_frequencies.html for formula
float getFreq(string n, int octave, int transpose = 0){
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    float dist = notes[n] + transpose + ((octave - 1) * 12);
    float frequency = freqFromA(dist);
    return frequency;
}

vector<float> getFifthChordFreqs(string n, int octave, int transpose = 0, int inversion = 0){
    float rootFreq = 440, thirdFreq = 440, fifthFreq = 440;
    vector<float> frequencies;
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    float dist = notes[n] + transpose + ((octave - 1) * 12);
    rootFreq = freqFromA(dist);
    
    switch(inversion){
        case 0: //Root Position
            thirdFreq = freqFromA(dist + 4);
            fifthFreq = freqFromA(dist + 7);
            break;
        case 1: // First Inversion
            thirdFreq = freqFromA(dist - 8);
            fifthFreq = freqFromA(dist - 5);
            break;
        case 2: //Second Inversion
            thirdFreq = freqFromA(dist + 4);
            fifthFreq = freqFromA(dist - 5);
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
