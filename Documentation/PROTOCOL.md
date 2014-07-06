## Director to others communication

# Musician Subscription
Each musician that want to take part to the session must send its subscription to the director:
```c
  struct subscription
  {
    uint_32 coupling; //see below
    
    uint_8 class;   //as in General Midi specifications
    
    uint_8 soloer;  //boolean, this instrument is a soloer or an accompanist
  }
```

The coupling field is used to make two or more instruments play on the same midi channel (e.g. emulating a piano for wich left hand plays accompainment and right hand plays lead solos)

# Director data
The director has to respond to each subscription with the actual id of the musician:
```c
  struct subscription_response
  {
    uint_32 id_player;
  }
```
where the player id has the lower 8 bits representing its General Midi class and the upper 24 bits used to disambiguate between two instruments of the same type.

The director send at each measure a struct to all the musicians:
```c
  struct measure
  {
  
    uint_8 bpm;
    uint_32 soloist_id;
    
    //The succession of chords in a measure. The bitmap goes from the 11th to 0th bit, where each bit defines the chord grade
    struct tempo_t {
      uint_8 upper;
      uint_8 lower;
    } tempo;
    
    //The successions of tonal zones in a measure. The mode bitmap (11th...0th) identifies the scale
    struct tonal_zone_t {
      uint_16 note;
      uint_16 scale;
    }[ ] tonal_zones; //size of the current time signature upper value
    
    //The successions of chords in a measure. The mode bitmap (11th...0th) identifies the chord mode
    struct chord_t{
      uint_16 note; 
      uint_16 mode;
    }[ ] chords; //size of the current time signature upper value
    
    //A string of 3 tags parted by a ; char (one field per each tag level dynamin, genre, mood).  
    struct tags_t {
      uint size;  //string size    
      char payload[]; //"genre;dynamics;mood\0"
    } tags;
      
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
At each measure musicians tell the player what they intend to play:
```c
  struct play_measure
  {
    uint_32 id;
    
    struct notes {
      uint_8 note;  //the actual note heght expressed in midi format
      uint_8 tempo; //the duration of the note
      uint_8 id;    //incremental number of notes inside the measure (used to create chords: some notes with the same id start at the same time)
      uint_8 triplets;  //boolean: is this note a part of a triplet?
    }[ ] measure;
  }
```
