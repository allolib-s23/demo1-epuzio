//Credit where credit is due:
//https://github.com/allolib-s21/notes-Mitchell57
//used to get Mitchell's Drum Test to work

#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <ostream>
#include <assert.h>  
#include <regex>

/*------------------------------------------------------------

    Scale, Chord, and Interval constants

------------------------------------------------------------*/
namespace theory
{
    /*
        Scale Constants
        
        Degrees:
            Tonic, Supertonic, Mediant, Subdominant, 
            Dominant, Submediant, Subtonic, Leading, Tonic2

        Scales:
            Aeolian / Minor, Ionian / Major, 
            Dorian, Phrygian, Locrian,
            Lydian, Mixolydian

            Pentatonic Major, Pentatonic Minor, 
            Harmonic Major, Harmonic Minor,
            Melodic Minor (asc), Melodic Minor (desc), 
            Bebop Dominant, Bebop Major,
            Major Minor, Half Diminished, 

            Algerian, Augmented, Blues, 
            Chromatic, Prometheus, Tritone,
            Locrian Major, Altered / Super Locrian, 
            Phrygian Dominant, Lydian Augmented / Acoustic,  
            Enigmatic, Double Harmonic / Flamenco, 
            
            Neapolitan Major, Neapolitan Minor, 
            Hungarian Minor, Hungarian Major
        
            Hirajoshi, In, Insen, Iwato, Persian
    */
    
    // Scale type labels
    enum scale_type {
        Chromatic=0, 
        Aeolian=1, Minor=1, Locrian=2, Ionian=3, Major=3, 
        Dorian=4, Phrygian=5, Lydian=6, Mixolydian=7,
        MelodicMinorDesc=7, MajorMinor=8, HalfDim = 9, LocrianMajor=10, Altered=11, SuperLocrian=11,
        PhrygianDom=12, LydianAug=13, Acoustic=13, 
        HarmonicMajor=14, HarmonicMinor=15, Enigmatic=16, DoubleHarmonic=17, Flamenco=17, 
        MelodicMinorAsc=18, NeapolitanMajor=19, NeapolitanMinor=20, HungarianMinor=21, HungarianMajor=22, 
        PentMajor=23, PentMinor=24,Algerian=25, Augmented=26, BebopDom=27, BebopMaj=28, Blues=29, Prometheus=30, Tritone=31,
        Hirajoshi=32, In=33, Insen=34, Iwato=35, Persian=36
    };

    // Scale degree labels
    enum scale_degree {
        Tonic=0, Supertonic=1, Mediant=2, Subdominant=3, Dominant=4, 
        Submediant=5, Subtonic=6, Leading=6, Tonic2=7,
        I=0, II=1, III=2, IV=3, V=4, VI=5, VIIb=6, VII=6, VIII=7,
    };

    // Scale interval constants
    const static int numScales = 37;
    const static int maxScale = 13;
    const static int scale_table[numScales][maxScale] = {
        {0,1,2,3,4,5,6,7,8,9,10,11,12},       // 0 Chromatic
        {0,2,3,5,7,8,10,12,-1,-1,-1,-1,-1},   // 1 Aeolian / Minor 
        {0,1,3,5,6,8,10,12,-1,-1,-1,-1,-1},   // 2 Locrian 
        {0,2,4,5,7,9,11,12,-1,-1,-1,-1,-1},   // 3 Ionian / Major   
        {0,2,3,5,7,9,10,12,-1,-1,-1,-1,-1},   // 4 Dorian
        {0,1,3,5,7,8,10,12,-1,-1,-1,-1,-1},   // 5 Phrygian 
        {0,2,4,6,7,9,11,12,-1,-1,-1,-1,-1},   // 6 Lydian 
        {0,2,4,5,7,9,10,12,-1,-1,-1,-1,-1},   // 7 Mixolydian / Melodic minor (descending)
        {0,2,4,5,7,8,10,12,-1,-1,-1,-1,-1},   // 8  MajorMinor
        {0,2,3,5,6,8,10,12,-1,-1,-1,-1,-1},   // 9  Half diminished
        {0,2,4,5,6,8,10,12,-1,-1,-1,-1,-1},   // 10 Locrian major 
        {0,1,3,4,6,8,10,12,-1,-1,-1,-1,-1},   // 11 Altered / Super Locrian
        {0,1,4,5,7,8,10,12,-1,-1,-1,-1,-1},   // 12 Phrygian dominant
        {0,2,4,6,8,9,11,12,-1,-1,-1,-1,-1},   // 13 Lydian Augmented / Acoustic
        {0,2,4,5,7,8,11,12,-1,-1,-1,-1,-1},   // 14 Harmonic Major   
        {0,2,3,5,7,8,11,12,-1,-1,-1,-1,-1},   // 15 Harmonic Minor  
        {0,1,4,6,8,10,11,12,-1,-1,-1,-1,-1},  // 16 Enigmatic
        {0,1,4,5,7,8,11,12,-1,-1,-1,-1,-1},   // 17 Double Harmonic / Flamenco
        {0,2,3,5,7,9,11,12,-1,-1,-1,-1,-1},   // 18 Melodic minor (ascending)   
        {0,1,3,5,7,9,11,12,-1,-1,-1,-1,-1},   // 19 Neapolitan major 
        {0,1,3,5,7,8,11,12,-1,-1,-1,-1,-1},   // 20 Neapolitan minor 
        {0,2,3,6,7,8,11,12,-1,-1,-1,-1,-1},   // 21 Hungarian Minor  
        {0,3,4,6,7,9,10,12,-1,-1,-1,-1,-1},   // 22 Hungarian Major
        {0,2,4,7,9,12,-1,-1,-1,-1,-1,-1,-1},  // 23 Major Pentatonic 
        {0,3,5,7,10,12,-1,-1,-1,-1,-1,-1,-1}, // 24 Minor Pentatonic 
        {0,2,3,6,7,8,11,12,14,15,17,-1,-1},   // 25 Algerian
        {0,3,4,7,8,11,12,-1,-1,-1,-1,-1,-1},  // 26 Augmented          
        {0,2,4,5,7,9,10,11,12,-1,-1,-1,-1},   // 27 Bebop dominant
        {0,2,4,5,7,8,9,11,12,-1,-1,-1,-1},    // 28 Bebop major      
        {0,3,5,6,7,10,12,-1,-1,-1,-1,-1,-1},  // 29 Blues            
        {0,2,4,6,9,10,12,-1,-1,-1,-1,-1,-1},  // 30 Prometheus 
        {0,1,4,6,7,10,12,-1,-1,-1,-1,-1,-1},  // 31 Tritone   
        {0,4,6,7,11,12,-1,-1,-1,-1,-1,-1,-1}, // 32 Hirajoshi        
        {0,1,5,7,8,12,-1,-1,-1,-1,-1,-1,-1},  // 33 In               
        {0,1,5,7,10,12,-1,-1,-1,-1,-1,-1,-1}, // 34 Insen            
        {0,1,5,6,10,12,-1,-1,-1,-1,-1,-1,-1}, // 35 Iwato            
        {0,1,4,5,6,8,11,12,-1,-1,-1,-1,-1},   // 36 Persian 

        // TODO: Add more world, jazz scales 
    };
    const static int numLabels = numScales+6;  // 6 scales with two names
    const static std::string scale_label[numLabels] = {
        "Chromatic", 
        "Aeolian", "Minor", "Locrian", "Ionian", "Major", 
        "Dorian", "Phrygian", "Lydian", "Mixolydian",
        "MelodicMinorDesc", "MajorMinor", "HalfDim", "LocrianMajor", "Altered", "SuperLocrian",
        "PhrygianDom", "LydianAug", "Acoustic", 
        "HarmonicMajor", "HarmonicMinor", "Enigmatic", "DoubleHarmonic", "Flamenco", 
        "MelodicMinorAsc", "NeapolitanMajor", "NeapolitanMinor", "HungarianMinor", "HungarianMajor", 
        "PentMajor", "PentMinor","Algerian", "Augmented", "BebopDom", "BebopMaj", "Blues", "Prometheus", "Tritone",
        "Hirajoshi", "In", "Insen", "Iwato", "Persian"
    };
    const static std::string degree_labels[9] = {
        "Tonic", "Supertonic", "Mediant", "Subdominant", "Dominant", "Submediant", "Subtonic", "Leading", "Tonic2"
    };

    /*
        Chord Constants:

            Quality = Major, minor, augmented, diminished, dominant, sus2, sus4
                (refers to set of intervals to use when building chord)

            Degree = root, third, fifth, seventh, ninth, eleventh, thirteenth, all
                (refers to each note in chord sequentially)
    */
    enum chord_quality {
        M, m, aug, dim, dom, sus2, sus4
    };
    enum chord_degree {
        root, third, fifth, seventh, ninth, eleventh, thirteenth, all
    };
    const static int numChord = 7;
    const static int maxChordLength = 7;
    const static int chord_table[numChord][maxChordLength] = {
        {0, 4, 7, 11, 14, 17, 21}, // Major
        {0, 3, 7, 10, 14, 17, 20}, // Minor
        {0, 4, 8, 10, 15, 19, 23}, // Aug
        {0, 3, 6, 9, 14, 17, 20},  // Diminished
        {0, 4, 7, 10, 13, 17, 20}, // Dom
        {0, 2, 7, -1,-1,-1},       // Sus2
        {0, 5, 7, -1,-1,-1},       // Sus4
    };

    /*
        Interval Constants

            Perfect    - P1, P4, P5, P8, 
            Minor      - m2, m3, m6, m7, 
            Major      - M2, M3, M6, M7,
            Diminished - d2, d3, d4, d5, d6, d7, d8,
            Augmented  - A1, A2, A3, A4, A5, A6, A7
    */
    enum interval_type {
        P1, P4, P5, P8, 
        m2, m3, m6, m7, 
        M2, M3, M6, M7,
        d2, d3, d4, d5, d6, d7, d8,
        A1, A2, A3, A4, A5, A6, A7
    };
    const static int numIntervals = 26;
    const static int interval_table[numIntervals] = {
        0, 5, 7, 12,          // Perfect
        1, 3, 8, 10,          // Minor
        2, 4, 9, 11,          // Major
        0, 2, 4, 6, 7, 9, 11, // Diminished
        1, 3, 5, 6, 8, 10, 12 // Augmented
    };
    const static std::string interval_labels[numIntervals] = {
        "P1", "P4", "P5", "P8", 
        "m2", "m3", "m6", "m7", 
        "M2", "M3", "M6", "M7",
        "d2", "d3", "d4", "d5", "d6", "d7", "d8",
        "A1", "A2", "A3", "A4", "A5", "A6", "A7"
    };
    
}
/*------------------------------------------------------------

    Static Helper Functions
        note parsing, chord parsing, midi->string, etc

------------------------------------------------------------*/
namespace helper{
    
    // Structures for easier passing of parsed strings between functions
    struct parsed_str
    {
        char note, sign;
        int octave;
    };

    struct parsed_chord 
    {
        std::string key, bass;
        std::vector<int> intervals; 
        theory::chord_quality quality;
    };

    // Regexes for parsing notes
    std::regex note_regex("[a-gA-G]");
    std::regex sign_regex("[#nb]");
    std::regex octave_regex("\\-1|[0-9]");

    // takes string input
    // returns {note, sign, octave} if valid
    static parsed_str parseString(std::string str)
    {
        std::string toParse = str;
        parsed_str ret;

        if(str.length() < 1 ){ throw std::out_of_range("Note(string) : input string ("+str+") is too short"); }

        // Make sure first character is a valid note letter
        if(regex_match(toParse.substr(0,1), note_regex))
        {
            ret.note = toParse[0];

            // If just a note, assume natural in 4th octave
            if(toParse.length() == 1)
            {
                ret.sign = 'n';
                ret.octave = 4;
                return ret;
            }
            // Else, pop off front character
            toParse = toParse.substr(1);
        }
        else{ throw std::out_of_range("Note(string) : input string ("+str+") is invalid (First char is not valid note)"); }

        // If there is a sign, save it and continue
        if(regex_match(toParse.substr(0,1), sign_regex))
        {
            ret.sign = toParse[0];
            if(toParse.length() == 1)
            {
                ret.octave = 4;
                return ret;
            }
            toParse = toParse.substr(1);
        }

        // Do the same for the octave, except convert it to int
        if(regex_match(toParse, octave_regex))
        {
            if(toParse[0] == '-')
            {
                ret.octave = -1*atoi(&toParse[1]);
            }
            else
            {
                ret.octave = atoi(&toParse[0]);
            }
            return ret;
        }
        throw std::out_of_range("Note(string) : input string ("+str+") is too long");
    }

    // Takes key ( letter[+sign] )
    // Returns note index as # of semitones from A
    static int noteIndex(std::string key)
    {
        char letter = key[0];
        int noteDist;
        switch (letter)
        {
            case 'C': case 'c':
                noteDist = -9;
                break;
            case 'D': case 'd':
                noteDist = -7;
                break;
            case 'E': case 'e':
                noteDist = -5;
                break;
            case 'F': case 'f':
                noteDist = -4;
                break;
            case 'G': case 'g':
                noteDist = -2;
                break;
            case 'A': case 'a':
                noteDist = 0;
                break;
            case 'B': case 'b':
                noteDist = 2;
                break;
            default:
                throw std::out_of_range("Note(string) : input letter ("+key+") is not a note");
                break;
        }

        if(key.length() == 1) return noteDist;
        else if(key.length() == 2)
        {
            if(key[1] == 'b') noteDist -= 1;
            else if(key[1] == '#') noteDist += 1;
            return noteDist;
        }
        throw std::out_of_range("Note(string) : input letter ("+key+") is not a note");
    }

    // takes parsed string input
    // returns midi index if valid
    static int parsedToMidi(parsed_str parsed)
    {
        // Validate input
        assert(regex_match(std::to_string(parsed.note), note_regex));
        assert(regex_match(std::to_string(parsed.sign), sign_regex));
        assert(parsed.octave > -1 && parsed.octave < 9);

        // Determine octave distance from 4
        int octDist = (parsed.octave-4)*12;
        int noteDist = 0;

        // Determine note distance from A
        switch(parsed.note)
        {
            case 'C': case 'c':
                noteDist = -9;
                break;
            case 'D': case 'd':
                noteDist = -7;
                break;
            case 'E': case 'e':
                noteDist = -5;
                break;
            case 'F': case 'f':
                noteDist = -4;
                break;
            case 'G': case 'g':
                noteDist = -2;
                break;
            case 'A': case 'a':
                noteDist = 0;
                break;
            case 'B': case 'b':
                noteDist = 2;
                break;
        }
        if(parsed.sign == 'b') noteDist -= 1;
        else if(parsed.sign == '#') noteDist += 1;

        // A4 = 69, so midi = (distance from A4) + 69
        return octDist + noteDist + 69;
    }

    

    // takes string input
    // returns Midi index if valid
    static int stringToMidi(std::string str)
    {
        parsed_str parsed = parseString(str);
        return parsedToMidi(parsed);
    }

    // Takes midi input
    // Returns string name if valid
    static std::string midiToString(int midi, char signPref='b', bool withOctave=true)
    {
        if(midi > 127 || midi <0){
            throw std::out_of_range("Note(midi) : midi index ("+std::to_string(midi)+") is out of range");
        } 
        int noteIdx = midi%12;
        int octave = ((midi-noteIdx)/12)-1;
        char letter;
        char sign = 'n';
        
        switch(noteIdx){
            case 0:
                letter = 'C';
                break;
            case 1:
                if(signPref=='#')
                {
                    letter='C';
                    sign='#';
                }
                else
                {
                    letter='D';
                    sign='b';
                }
                break;
            case 2:
                letter = 'D';
                break;
            case 3:
                if(signPref=='#')
                {
                    letter='D';
                    sign='#';
                }
                else
                {
                    letter='E';
                    sign='b';
                }
                break;
            case 4:
                letter = 'E';
                break;
            case 5:
                letter = 'F';
                break;
            case 6:
                if(signPref=='#')
                {
                    letter='F';
                    sign='#';
                }
                else
                {
                    letter='G';
                    sign='b';
                }
                break;
            case 7:
                letter = 'G';
                break;
            case 8:
                if(signPref=='#')
                {
                    letter='G';
                    sign='#';
                }
                else
                {
                    letter='A';
                    sign='b';
                }
                break;
            case 9:
                letter = 'A';
                break;
            case 10:
                if(signPref=='#')
                {
                    letter='A';
                    sign='#';
                }
                else
                {
                    letter='B';
                    sign='b';
                }
                break;
            case 11:
                letter = 'B';
                break;
        }

        std::string ret;
        ret += letter;
        if(sign == 'b' || sign=='#') ret += sign;
        if(withOctave) ret += std::to_string(octave);
        return ret;
    }

    

    // Fills in list of chord note intervals 
    parsed_chord buildChordIntervals(parsed_chord chord, int length)
    {
        for(int i=0; i<length; i++){
            int interval = theory::chord_table[chord.quality][i];
            if(interval >= 0) chord.intervals.push_back(interval);
        }
        return chord;
    }

    // input: string chordName (e.g. "Cmaj7, Dbsus2")
    // returns parsed_chord struct with root, quality, and a list of intervals
    static parsed_chord parseChord(std::string name)
    {
        std::string str = name;
        
        // Key matching
        std::regex key("^[a-gA-G]");
        std::regex sign("^[bn#]");
        
        // Quality matching
        std::regex major("^((maj)|(M))");
        std::regex minor("^((min)|(m)|(-))");
        std::regex dim("^((dim)|o)");
        std::regex aug("^((aug)|(\\+)|(\\+5))");
        
        // Extensions, Alterations, Figured bass
        std::regex extended("^(7|9|(11)|(13))");
        std::regex alter("^([#b][59])");
        std::regex add("^(add[24689])");
        std::regex drop("^(add[24689])");
        std::regex bass("^(\\/[a-gA-g][b#]?)");

        std::smatch m;
        parsed_chord chord;
        int length = 0;

        // First, pop off key and sign
        if(std::regex_search(str, m, key))
        {
            chord.key = m.str();
            str = m.suffix().str();
        }
        if(std::regex_search(str, m, sign))
        {
            chord.key += m.str();
            str = m.suffix().str();
        }

        // Then determine quality
        if(std::regex_search(str, m, major) || str.length()==0){
            chord.quality = theory::M;
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, minor)){
            chord.quality = theory::m;
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, aug)){
            chord.quality = theory::aug;
            str = m.suffix().str();
        }
        else if(std::regex_search(str, m, dim)){
            chord.quality = theory::dim;
            str = m.suffix().str();
        }
        else if(str.substr(0,4) == "sus2"){
            chord.quality = theory::sus2;
        }
        else if(str.substr(0,4) == "sus4"){
            chord.quality = theory::sus4;
        }
        else{
            chord.quality = theory::dom;
        }

        // then check for extensions
        if(std::regex_search(str, m, extended)){
            int extend = std::stoi(m[0]);
            if(extend == 7) length = 4;
            if(extend == 9) length = 5;
            if(extend == 11) length = 6;
            if(extend == 13) length = 7;
            str = m.suffix().str();
        }
        else{ length = 3; }

        // Build the base chord intervals
        chord = buildChordIntervals(chord, length);
        
        // Now look for alterations
        if(std::regex_search(str, m, alter))
        {
            if (m.str()[0] == 'b'){
                if(m.str()[1] == '5'){ chord.intervals[theory::fifth] -= 1; }
                if(m.str()[1] == '9') 
                {
                    // Add the ninth if it is not in chord
                    if(chord.intervals.size() < 5)
                    {
                        chord.intervals.push_back(theory::chord_table[chord.quality][theory::ninth]);
                    }
                    chord.intervals[chord.intervals.size()-1] -= 1;
                }
            }
            else if (m.str()[0] == '#')
            {
                if(m.str()[1] == '5') { chord.intervals[theory::fifth] += 1; }
                if(m.str()[1] == '9') 
                {
                    // Add the ninth if it is not in chord
                    if(chord.intervals.size() < 5)
                    {
                        chord.intervals.push_back(theory::chord_table[chord.quality][theory::ninth]);
                    }
                    chord.intervals[chord.intervals.size()-1] += 1;
                }
            }
            str = m.suffix().str();
        }
        
        // Then check for added tones
        if(std::regex_search(str, m, add))
        {
            int add = std::stoi(m.str().substr(3,4));
            int interval;
            switch(add)
            {
                case 2:
                    interval = theory::chord_table[chord.quality][theory::ninth] - 12;
                    break;
                case 4:
                    interval = theory::chord_table[chord.quality][theory::eleventh] - 12;
                    break;
                case 6:
                    interval = theory::chord_table[chord.quality][theory::thirteenth] - 12;
                    break;
                case 8:
                    interval = 12;
                    break;
                case 9:
                    interval = theory::chord_table[chord.quality][theory::ninth];
                    break;
            }
            chord.intervals.push_back(interval);
            std::sort (chord.intervals.begin(), chord.intervals.end());
            str = m.suffix().str();
        }

        // If there is figured bass, record it
        if(std::regex_search(str, m, bass))
        {
            chord.bass = m.str().substr(1);
            str = m.suffix().str();
        }
        else{ chord.bass = chord.key; }

        // If string is not empty, chord is invalid
        if(str.length() != 0){ throw std::out_of_range("Chord(string) : Chord ("+name+") is invalid, "+str+" was left over"); }
        return chord;
    }   
}

namespace theory {
    
    class Scale;
    class Chord;
    class Note 
    {
        public:
            int index;
            char signPref;

            Note(char note='A', char sign='n', int octave=4, char signPref='b');
            Note(std::string input, char signPref='n');
            Note(int midi, char signPref='b');

            std::string name();
            std::string key();
            int         midi();
            float       frequency(float root=440.0);
            int         octave();
            int         distanceTo(Note* b);

            void set(int midi, char signPref='b');
            void set(std::string input, char signPref='n');
            void set(char key='A', char sign='n', int octave=4, char signPref='b');

            bool setOctave(int octave=4);
            bool setKey(std::string key);
            bool setKey(char key, char sign);

            Note octaveUp(int num=1);
            Note octaveDown(int num=1);

            Note  interval(interval_type type, int direction=1);
            Note  interval(int semitones);
            Chord chord(std::string chord_name, int octave=3);
            Scale scale(scale_type type);

        private:
            void init(int midi, char signPref);
    };

    typedef std::vector<Note> notelist;

    class Chord
    {
        public:
            notelist notes;
            chord_quality quality;

            Chord(std::string name, int octave=3);
            Chord(Note* root, std::string name, int octave=3);
            Chord(std::vector<int> idxs);

            Note root();
            Note third();
            Note fifth();
            Note seventh();
            Note ninth();
            Note eleventh();
            Note thirteenth();

            void invert(int inversion=1);
            void drop(chord_degree degree=all);
            void raise(chord_degree degree=all);
            void match(Chord source);

            std::vector<int> indexes();

            float score();
            void sort();
        
        private:
            void init(helper::parsed_chord parsed, int octave=3);
    };

    class Scale{
        public:
            notelist notes;
            scale_type type;

            Scale(Note tonic, scale_type type);
            Scale(std::string tonic, scale_type type);

            Note degree(scale_degree degree);
            Note degree(int degree);
            Note index(int idx);

            Chord chord(scale_degree degree, int size=3);
            Chord chord(int degree, int size=3);
        
        private:
            void init(Note tonic, scale_type type);
            Chord buildChord(int degree, int size);
    };

// ------------------------------------------------------------------
//      Note methods
// ------------------------------------------------------------------ 

        // Constructors
        Note::Note(std::string input, char signPref){ this->set(input, signPref); }

        Note::Note(int midi, char signPref){ init(midi, signPref); }

        Note::Note(char note, char sign, int octave, char signPref)
        { 
            helper::parsed_str parsed = {note, sign, octave};
            init(helper::parsedToMidi(parsed), signPref);
        }
        
        // Main initializer
        void Note::init(int midi, char signPref)
        {
            if(midi > 127 || midi <0){ throw std::out_of_range("Note(midi) : midi index ("+std::to_string(midi)+") is out of range"); }
            this->index = midi;
            this->signPref = signPref;
        }

        // returns full note name (e.g. "Db6")
        std::string Note::name(){ return helper::midiToString(this->index, this->signPref); }

        // returns key without octave (e.g. "Db")
        std::string Note::key(){ return helper::midiToString(this->index, this->signPref, false); }

        // returns midi index
        int Note::midi(){ return this->index; }

        // returns frequency (based on root)
        float Note::frequency(float root)
        {
            int distance = this->index - 69;
            double multiplier = pow(2.0, 1.0/12);

            return (float)(root*pow(multiplier, distance));
        }

        // returns octave [-1, 9]
        int Note::octave()
        {
            return (this->midi()/12)-1;
        }

        // set note to new midi index [0-127]
        void Note::set(int midi, char signPref){ init(midi, signPref); }

        // set note to new string name
        void Note::set(std::string input, char signPref)
        { 
            helper::parsed_str parsed = helper::parseString(input);
            int idx = helper::parsedToMidi(parsed);
            if(signPref == 'n')
            {
                if(parsed.sign == '#') { init(idx, '#'); }
                else{ init(idx, 'b'); }
            }
            else{ init(idx, signPref); } 
        }
        
        // set note to new key, sign, and octave
        void Note::set(char key, char sign, int octave, char signPref)
        { 
            helper::parsed_str parsed = {key, sign, octave};
            init(helper::parsedToMidi(parsed), signPref);
        }

        // set octave of note without changing key
        bool Note::setOctave(int octave)
        {
            if(octave < -1 || octave > 9) return false;
            int noteIdx = this->index%12;
            int offset = (octave+1)*12;
            this->index = noteIdx + offset;

            return true;
        }

        // set key of note without changing octave
        bool Note::setKey(std::string key)
        {
            int octave = this->octave();
            this->set(key);
            return this->setOctave(octave);
        }

        // set key of note without changing octave
        bool Note::setKey(char key, char sign)
        {
            int octave = this->octave();
            this->set(key, sign, octave);
            return true;
        }

        // returns note at octave intervals above
        Note Note::octaveUp(int num){ return Note(this->index+(12*num)); }

        // returns note at octave intervals above
        Note Note::octaveDown(int num){ return Note((this->index)-(12*num)); }

        // returns distance (in semitones) to another note
        int Note::distanceTo(Note* b){ return b->midi() - this->index; }

        // returns note at specified interval above/below current note
        // direction = 1 for up
        // direction = -1 for down
        Note Note::interval(interval_type type, int direction)
        {
            int interval = interval_table[type] * direction;
            Note n = Note(index + interval);
            return n;
        }

        // returns note at specified interval above/below current note
        Note Note::interval(int semitones)
        {
            Note n = Note(index + semitones);
            return n;
        }

// ------------------------------------------------------------------
//      Chord methods
// ------------------------------------------------------------------ 

        // constructors
        Chord::Chord(std::string name, int octave)
        {
            helper::parsed_chord parsed = helper::parseChord(name);
            init(parsed, octave);
        }

        Chord::Chord(Note* root, std::string name, int octave)
        {
            helper::parsed_chord parsed = helper::parseChord(name);
            parsed.key = root->key();
            init(parsed, octave);
        }

        Chord::Chord(std::vector<int> idxs)
        {
            for(int idx: idxs){ notes.push_back(Note(idx)); }
        }

        void Chord::init(helper::parsed_chord parsed, int octave)
        {
            Note root = Note(parsed.key);
            quality = parsed.quality;
            
            root.setOctave(octave);
            int rootIdx = root.midi();

            for(int interval:parsed.intervals)
            {
                notes.push_back(Note(rootIdx + interval));
            }

            if(parsed.bass != parsed.key && parsed.bass.length() > 0)
            {
                int bassNote = helper::noteIndex(parsed.bass);
                int bassIdx = -1;
                for(int i=0; i<notes.size(); i++)
                {
                    int noteLoc = helper::noteIndex(notes[i].key());
                    if(noteLoc == bassNote){ bassIdx = i; }
                }

                if(bassIdx == -1){ throw std::out_of_range("Chord(string) : Figured bass ("+parsed.bass+") is not in chord"); }
                else{ this->invert(bassIdx); }
            }
        }

        // Returns chord with root (this) and type (name)
        Chord Note::chord(std::string name, int octave){ return Chord(this, name, octave); }

        // returns root note
        Note Chord::root(){ return notes[0]; }

        // returns third note
        Note Chord::third(){ return notes[1]; }

        // returns fifth note
        Note Chord::fifth(){ return notes[2]; }

        // returns seventh note
        Note Chord::seventh()
        { 
            if(notes.size() > 3){ return notes[3]; }
            else
            {
                int interval = chord_table[quality][3];
                if(interval < 0){ throw std::out_of_range("Chord(string) : Sus chords cannot be extended"); }
                return Note(root().index + interval);
            }
        }

        // returns ninth note
        Note Chord::ninth()
        { 
            if(notes.size() > 4){ return notes[4]; }
            else
            {
                int interval = chord_table[quality][4];
                if(interval < 0){ throw std::out_of_range("Chord(string) : Sus chords cannot be extended"); }
                return Note(root().index + interval);
            }
        }   

        // returns eleventh note
        Note Chord::eleventh()
        { 
            if(notes.size() > 5){ return notes[5]; }
            else
            {
                int interval = chord_table[quality][5];
                if(interval < 0){ throw std::out_of_range("Chord(string) : Sus chords cannot be extended"); }
                return Note(root().index + interval);
            }
        }

        // returns thirteenth note
        Note Chord::thirteenth()
        { 
            if(notes.size() > 6){ return notes[6]; }
            else
            {
                int interval = chord_table[quality][6];
                if(interval < 0){ throw std::out_of_range("Chord(string) : Sus chords cannot be extended"); }
                return Note(root().index + interval);
            }
        }   

        void Chord::drop(chord_degree degree)
        {
            if(degree == all){
                for(Note n: notes){
                    n.octaveDown();
                }
            }
            else{
                if(degree < notes.size()){
                    notes[degree].octaveDown();
                }
                else{
                    int interval = chord_table[quality][degree];
                    Note newNote = Note(root().midi() + interval - 12);
                }
            }
        }

        void Chord::raise(chord_degree degree)
        {
            if(degree == all){
                for(Note n: notes){
                    n.octaveUp();
                }
            }
            else{
                if(degree < notes.size()){
                    notes[degree].octaveUp();
                }
                else{
                    int interval = chord_table[quality][degree];
                    Note newNote = Note(root().midi() + interval + 12);
                }
            }
        }

        void Chord::invert(int inversion)
        {
            for(int i=0; i<inversion; i++){
                int newIdx = notes[0].index+12;
                if(newIdx > 127){
                    this->drop();
                    newIdx = notes[0].index+12;
                }
                std::rotate(notes.begin(), notes.begin()+1, notes.end());
            }
        }

        //helper funcs for chord match
        bool compareNote(Note a, Note b){ return (a.midi() < b.midi()); }

        void Chord::sort(){ std::sort(notes.begin(), notes.end(), compareNote); }

        std::vector<int> Chord::indexes()
        {
            std::vector<int> ret;
            for(Note n : notes){
                ret.push_back(n.midi());
            }
            return ret;
        }

        int inner_distance(std::vector<int> idxs)
        {
            int sum = 0;
            for(int i=0; i<idxs.size()-2; i++){
                sum += abs(idxs[i+1]-idxs[i]);
            }
            return sum;
        }

        float Chord::score()
        {
            int sum = 0.0;
            for(Note n : notes){
                sum += n.midi();
            }
            return sum / notes.size();
        }

        // Attempts to match average pitch of this chord to source chord 
        void Chord::match(Chord dest)
        {
            float destScore = dest.score();
            float score = this->score();
            float dist = score - destScore;
            float newScore, newDist;
            float threshold = (12.0/notes.size())+2;
            while(abs(dist) > threshold){
                if(dist > threshold) {
                    int i= notes.size()-1;
                    Note dropped = notes[i].octaveDown();
                    newScore = (this->score() - notes[i].index + dropped.index) / notes.size();
                    newDist = newScore - destScore;
                    if(newDist > dist) break;
                    else notes[i] = dropped;
                    this->sort();
                }
                else if(dist < -1*threshold){
                    Note raised = notes[0].octaveUp();
                    newScore = (this->score() - notes[0].index + raised.index) / notes.size();
                    newDist = newScore - destScore;
                    if(newDist > dist) break;
                    else notes[0] = raised;
                    this->sort();
                }
                score = this->score();
                dist = score - destScore;
            }
        }

        

// ------------------------------------------------------------------
//      Scale methods
// ------------------------------------------------------------------ 
        void Scale::init(Note tonic, scale_type type)
        {
            this->type = type;
            
            // loop through chord intervals to build list of notes
            for(int i=0; i<maxScale; i++){
                int interval = scale_table[type][i];
                if(interval >= 0 ){ // variable length scales, fixed length array, filled space with -1s
                    int idx = tonic.index + interval;
                    Note cnote = Note(idx);
                    notes.push_back(cnote);
                }
            } 
        }

        Scale::Scale(Note tonic, scale_type type){ init(tonic, type);}
        Scale::Scale(std::string tonic, scale_type type){ init(Note(tonic), type); }

        Note Scale::degree(scale_degree degree){ return notes[degree]; }

        Note Scale::degree(int degree){ return notes[degree-1]; }

        Note Scale::index(int idx){ return notes[idx]; }

        Chord Scale::chord(scale_degree degree, int size){ return this->buildChord(degree, size); }

        Chord Scale::chord(int degree, int size){ return this->buildChord(degree-1, size); }

        Chord Scale::buildChord(int degree, int size)
        {
            int idx = degree;
            std::vector<int> chordNotes;
            for(int i=0; i<size; i++){
                chordNotes.push_back(notes[idx].midi());
                idx = (idx+2)%notes.size();
            }
            return Chord(chordNotes);
        }

    
// ------------------------------------------------------------------
//     Tempo Class
// ------------------------------------------------------------------ 
    class Tempo{
        const static int numNotes = 6;
        const float note_length[numNotes] = {4, 2, 1, 0.5, 0.25, 0.125};

        public:
            struct timeSignature{
                int top;
                int bottom;
            };

            enum note_type{
                whole=0, w=0,
                half=1, h=1,
                quarter=2, q=2,
                eighth=3, e=3,
                sixteenth=4, s=4,
                thirtysecond=5, t=5,
            };

            timeSignature timeSig;
            note_type beatType;

            float bpm;
            float beatLength;
            float barLength;

            Tempo(float bpm=80, int sigTop=4, int sigBottom=4){
                assert(sigBottom%4 ==0);
                assert(bpm > 0);
                
                this->bpm = bpm;
                this->timeSig.top = sigTop;
                this->timeSig.bottom = sigBottom;

                this->beatLength = 1 / (bpm / 60);
                this->barLength = beatLength*(4.0/sigBottom)*sigTop;
            }

            float duration(note_type type, bool dot=false){
                float duration = note_length[type] * beatLength;
                if(dot) duration *= 1.5;
                return duration;
            }
    };
}