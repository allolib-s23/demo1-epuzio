//To make playing notes a little easier - referenced https://gist.github.com/stuartmemo/3766449 and http://techlib.com/reference/musical_note_frequencies.html for formula
#include <string>
#include <vector>
using namespace std;

float freqFromA(int distance){
    return 27.50 * pow(2, (distance)/12.0);
}

float getFreq(string n, int octave, int transpose = 0){ //chorus effect!
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    float dist = notes[n] + transpose + ((octave - 1) * 12) +1;
    float frequency = freqFromA(dist);
    cout << "notes " << n << octave << ": "<< notes[n]<< endl;
    cout << "dist: " << dist << endl;
    cout << "freq: " << frequency << "!" << endl;
    return frequency;
}

vector<float> getFifthChordFreqs(string n, int octave, int inversion){
    float rootFreq, thirdFreq, fifthFreq;
    vector<float> frequencies = {rootFreq, thirdFreq, fifthFreq};
    unordered_map<string, float> notes{
        {"A", 0}, {"A#", 1}, {"Bb", 1}, {"B", 2}, {"C", 3}, {"C#", 4}, {"Db", 4}, {"D", 5}, {"D#", 6}, 
        {"Eb", 6}, {"E", 7}, {"F", 8}, {"F#", 9}, {"Gb", 9}, {"G", 10}, {"G#", 11}, {"Ab", 11}
    };
    float dist = notes[n] + ((octave - 1) * 12) +1;
    rootFreq = freqFromA(dist);
    switch(inversion){
        case 0: //Root Position
            thirdFreq = freqFromA(dist + 4);
            fifthFreq = freqFromA(dist + 7);
        case 1: // First Inversion
            thirdFreq = freqFromA(dist - 8);
            fifthFreq = freqFromA(dist - 5);
        case 2: //Second Inversion
            thirdFreq = freqFromA(dist + 4);
            fifthFreq = freqFromA(dist - 5);
    }
    return frequencies;
}

// Table for reference:
/*
    const float C = 16.35;
    const float Cs = 17.32;
    const float Db = 17.32;
    const float D = 18.35;
    const float Ds = 19.45;
    const float D = 19.45;
    const float E = 20.6; 
    const float F = 21.83;
    const float Fs = 23.12;
    const float Gb = 23.12;
    const float G = 24.50;
    const float Gs = 25.96;
    const float Ab = 25.96;
    const float A = 27.50;
    const float As = 29.14;
    const float Bb = 29.14;
    const float B = 30.87;
*/