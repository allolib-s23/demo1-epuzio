//To make playing notes a little easier - referenced https://gist.github.com/stuartmemo/3766449 and http://techlib.com/reference/musical_note_frequencies.html for formula
#include <string>
#include <vector>
using namespace std;

float freqFromA(int distance){
    return 27.50 * pow(2, (distance)/12.0);
}

vector<bool> LFSR(bool input1, bool input2){ 
    //Linear Feedback Shift Register (based on the Illyana by Omiindustriies)
    //Given 2 input signals, calculate 4 output signals (or, xor, and, nand)
    vector<bool> output;
    output.push_back(input1 || input2); //OR
    output.push_back(input1 ^ input2); //XOR
    output.push_back(input1 && input2); //AND
    output.push_back(!(input1 && input2)); //NAND
    return output;
}

bool fibLFSR(int input){ 
    //Linear Feedback Shift Register (based on the 4-bit Fibonacci LFSR on Wikipedia: https://en.wikipedia.org/wiki/Linear-feedback_shift_register)
    // should have a maximum cycle of 15
    //taps are 4, 3
    bool output = 1;
    output = ((input>>3) ^ (input>>2)&0xC);
    cout << "OUTPUT : " << (input>>3) << " or " << ((input>>2)&0xC) << endl;
    cout << "out: " << ((input>>3) ^ (input>>2)&0xC) <<endl;
    return output;
}

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
