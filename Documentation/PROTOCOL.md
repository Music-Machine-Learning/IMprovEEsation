## Director to others communication

# Musician Registration
TODO

# Director data
The director send at each measure a struct to all the musicians
```c
  {
  
    uint_8 bpm;
    uint_32 soloist_id;
    
    //The succession of chords in a measure. The bitmap goes from the 11th to 0th bit, where each bit defines the chord grade
    struct tempo_t {
      uint_8 upper;
      uint_8 lower;
    }
    
    //The successions of tonal zones in a measure. The mode bitmap (11th...0th) identifies the scale
    struct tonal_zone{
      uint_16 note;
      uint_16 scale;
    }[ ] //size of the current time signature upper value
    
    //The successions of chords in a measure. The mode bitmap (11th...0th) identifies the chord mode
    struct chord_t{
      uint_16 note; 
      uint_16 mode;
    }[ ] //size of the current time signature upper value
    
    //A string of 3 tags parted by a ; char (one field per each tag level dynamin, genre, mood).  
    struct tags_t {
      uint size;  //string size    
      char payload[];
    }
      
  }
```
 
An example could be:
 * 120 //bpm
 * 3   //soloist_id
 * {4, 4} //tempo
 * [{A, 101010101101}, {A, 101010101101}, {A, 101010101101}, {A, 101010101101}] A minor scale for the whole measure
 * [{3, 000010010001}, {3, 000010010001}, {1, 000010001001}, {1, 000010001001}] (C Major, C Major, A minor, A minor)
 * "groove;blues;intro"


## Musicians - Player communication
TODO
