#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <unordered_map>
using namespace std;

//PSEUDO-RANDOM SEQUENCES USING LFSR

//LFSR helper to return taps, given the number of bits in an LFSR cycle
//Referenced: https://en.wikipedia.org/wiki/Linear-feedback_shift_register (under "m-sequence")
//Double-check endianness for m-sequences
int getLFSRTaps(int length){
    unordered_map<int, int> taps{
        {2, 0x3}, {3, 0x6}, {4, 0x3}, {5, 0x5}, {6, 0x3}, {7, 0x3}, {8, 0xB8},
        {9, 0x110}, {10, 0x240}, {11, 0x500}, {12, 0xE08}, {13, 0x1C80}, {14, 0x3802},
        {15, 0x6000}, {16, 0xD008}
    };
    return taps[length];
}

//Fibonnacci Linear Feedback Shift Register
//Referenced: https://simple.wikipedia.org/wiki/Linear-feedback_shift_register
//Seed is the seed for the LFSR, cycleLength is the number of bits in the LFSR,
//SequenceLength is how many output bits we want to generate
vector<bool> getFibLFSRSequence(int seed, int numBits, int sequenceLength){
    int taps = getLFSRTaps(numBits);
    int prevSequence = seed;
    vector<bool> output;
    for(int i = 0; i < sequenceLength; i++){
        bool outputBit = (__builtin_parity(prevSequence & taps)&1);
        output.push_back(outputBit);
        prevSequence = (prevSequence>>1) | (outputBit<<(numBits-1));
    }
    return output;
}




//TRANSPOSITION AND CHORD PROGRESSIONS

//Convert a note to the distance from that note to A
float getDist(string n){ 
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    return notes[n];
}

//Return a frequency, given a distance from A0
float getFreqFromDist(int distance){ 
    return 27.50 * pow(2, (distance)/12.0);
}

//Get the frequency of a note, given a string, octave, and number of half-steps to transpose that note
//Referenced: https://gist.github.com/stuartmemo/3766449
float getFreq(string n, int octave, int transpose = 0){
    float dist = getDist(n) + transpose + ((octave - 1) * 12);
    float frequency = getFreqFromDist(dist);
    return frequency;
}

//Get the frequency of a note given a distance from A, octave, and number of half-steps to transpose that note
float getFreq(int n, int octave, int transpose = 0){
    float dist = n + transpose + ((octave - 1) * 12);
    float frequency = getFreqFromDist(dist);
    return frequency;
}

//Get frequencies belonging to the major fifth of the given note
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

//Return frequencies corresponding to an Axis Progression in a semi-random order with random inversions
//Referenced: https://en.wikipedia.org/wiki/I%E2%80%93V%E2%80%93vi%E2%80%93IV_progression
vector<vector<float>> axisProgression(string n, int octave, int transpose = 0){
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




//RANDOM MELODIES USING MARKOV CHAINS

//Referenced: https://sites.math.washington.edu/~conroy/m381-general/markovMusic/markovMusic.htm
//and https://scholarship.claremont.edu/cgi/viewcontent.cgi?article=1848&context=jhm
//The following probabilities are heavily weighted towards a major 7 chord starting at the note A
//(1 = .36, 3 = .2, 5 = .25, 7 = .1)
//The remaining .1 percent is divided between minor third, tritone, raised fifth
// vector<int> noteTransitionMatrix = {36, 0, 0, 3, 20, 0, 3, 25, 3, 0, 0, 20};
vector<float> getMarkovNotes(string n, int octave, int transpose, int sequenceLength){
    vector<float> output;
    vector<discrete_distribution<int>> markovNoteProbabilities = {
        discrete_distribution<int>({36, 0, 0, 3, 20, 0, 3, 25, 3, 0, 0, 20}), //A
		discrete_distribution<int>({20, 36, 0, 0, 3, 20, 0, 3, 25, 3, 0, 0}), //A# / Bb
		discrete_distribution<int>({0, 20, 36, 0, 0, 3, 20, 0, 3, 25, 3, 0}), //B
		discrete_distribution<int>({0, 0, 20, 36, 0, 0, 3, 20, 0, 3, 25, 3}), //C
		discrete_distribution<int>({3, 0, 0, 02, 36, 0, 0, 3, 20, 0, 3, 25}), // C# / Db
		discrete_distribution<int>({25, 3, 0, 0, 20, 36, 0, 0, 3, 20, 0, 3}), // D 
		discrete_distribution<int>({3, 25, 3, 0, 0, 20, 36, 0, 0, 3, 20, 0}), // D# / Eb
		discrete_distribution<int>({0, 3, 25, 3, 0, 0, 2, 36, 0, 0, 3, 20}), //E
		discrete_distribution<int>({20, 0, 3, 25, 3, 0, 0, 20, 36, 0, 0, 3}), //F #
        discrete_distribution<int>({3, 20, 0, 3, 25, 3, 0, 0, 20, 36, 0, 0}), //F
		discrete_distribution<int>({0, 3, 20, 0, 3, 25, 3, 0, 0, 20, 36, 0}), // G
		discrete_distribution<int>({0, 0, 3, 2, 0, 3, 25, 3, 0, 0, 20, 36}) //G# / Ab
    }; 
    vector<discrete_distribution<int>> markovOctaveProbabilities = {
        discrete_distribution<int>({0, 0, 0, 0, 0, 0}), //0
        discrete_distribution<int>({0, 0, 0, 0, 0, 0}), //1
        discrete_distribution<int>({0, 0, 0, 0, 0, 0}), //2
        discrete_distribution<int>({0, 0, 0, 40, 55, 5}), //3
        discrete_distribution<int>({0, 0, 0, 5, 90, 5}), //4
        discrete_distribution<int>({0, 0, 0, 5, 65, 30}), //5
    }; 
    random_device rd;
    mt19937 gen(rd());
    int currentNote = getDist(n);
    int currentOctave = octave;
    for(int i = 0; i < sequenceLength; i++){
        currentNote = markovNoteProbabilities[currentNote](gen);
        currentOctave = markovOctaveProbabilities[currentOctave](gen);
        output.push_back(getFreq(currentNote, currentOctave, transpose));
    } 
    return output;
} 

//helper to return probability of playing a note at a 1/2 beat for a 4/4 measure
bool getBernoulliProb(int numerator, int denominator){
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution dist(numerator/(float)denominator);
    return dist(gen);
}

//Calculate whether to play a note based on the number of notes already played/number of notes in the measure
vector<bool> getNoteSpacingForMeasure(int numNotes){
    vector<bool> output;
        float denominator = 8; //In 4/4 time, 8 eigth notes per measure
        bool currentNote = 0;
        for(int i = 0; i < 8; i++){
            if(i % 2 ==0){ //more likely to play a note if it's at the start of a beat
                currentNote = getBernoulliProb(numNotes, denominator/2);
            } else{
                currentNote = getBernoulliProb(numNotes, denominator);
            }
            output.push_back(currentNote);
            denominator -= 1;
            if(currentNote){numNotes--;}
        }
    return output;
}