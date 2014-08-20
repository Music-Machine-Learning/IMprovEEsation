# Director to others communication

## Musician Subscription
Each musician that want to take part to the session must send its subscription to the director:
```c
  struct subscription_s
  {
    uint32_t coupling; //see below

    uint8_t class;   //as in General Midi specifications

    uint8_t soloer;  //boolean, this instrument is a soloer or an accompanist
  };
```

The coupling field is used to make two or more instruments play on the same midi channel (e.g. emulating a piano for wich left hand plays accompainment and right hand plays lead solos)

Coupling -1 value is reserved for the Midi Player, 0 means there is no coupling, different values will join the instruments that share the same coupling.

## Director data
The director has to respond to each subscription with the actual id of the musician:
```c
    uint32_t id_player;
```
where the player id has the lower 8 bits representing its General Midi class and the upper 24 bits used to disambiguate between two instruments of the same type.

The director send at each measure a struct to all the musicians:
```c
  struct measure_s
  {
    uint8_t bpm;
    uint32_t soloist_id;

    //The succession of chords in a measure. The bitmap goes from the 11th to 0th bit, where each bit defines the chord grade
    struct tempo_s {
      uint8_t upper;
      uint8_t lower;
    } tempo;

    //The successions of tonal zones in a measure. The mode bitmap (11th...0th) identifies the scale
    struct tonal_zone_s {
      uint16_t note;
      uint16_t scale;
    } tonal_zones[]; //size of the current time signature upper value

    //The successions of chords in a measure. The mode bitmap (11th...0th) identifies the chord mode
    struct chord_s {
      uint16_t note;
      uint16_t mode;
    } chords[]; //size of the current time signature upper value

    //A string of 3 tags parted by a ; char (one field per each tag level genre, dynamin, mood).
    struct tags_s {
      uint size;  //string size
      char payload[]; //"genre;dynamics;mood\0"
    } tags;

  };
```
 
An example could be:
 * 120 //bpm
 * 3   //soloist_id
 * {4, 4} //tempo
 * [{A, 101010101101}, {A, 101010101101}, {A, 101010101101}, {A, 101010101101}] A minor scale for the whole measure
 * [{3, 000010010001}, {3, 000010010001}, {1, 000010001001}, {1, 000010001001}] (C Major, C Major, A minor, A minor)
 * "groove;blues;intro"


# Musicians - Player communication
At each measure musicians tell the player what they intend to play:
```c
  struct play_measure_s
  {
    uint32_t id;
    uint32_t size;

    struct notes_s {
      uint8_t note;  //the actual note heght expressed in midi format
      uint8_t tempo; //the duration of the note
      uint8_t id;    //incremental number of notes inside the measure (used to create chords: some notes with the same id start at the same time)
      uint8_t triplets;  //boolean: is this note a part of a triplet?
    } measure[];
  };
```


# Network Library
The network library should offer the next funcions.
(Please reference to the header file in include/improveesation/communication.h)

All the functions will throw a "network_exception" if some operations go wrong and a "end_of_imrpovisation_exception" when the improvisation is over (tecnically when the director close the associated connection socket).
```c
	/* network exception (i.e. connection error) */
	class network_exception: public std::exception

	/* End of communication exception, the improvisation ended. */
	class end_of_improvisation_exception: public network_exception
```

Musician:
```c
  // send_subscriptions is a syncronous call, waits for director response and returns the actual musician ID (and the midi player's address) or throws an exception if something bad happens
  uint32_t send_subscription(int director, struct subscription_s *proposal);
  uint32_t send_subscription(int director, uint32_t coupling, uint8_t instrument_class, uint8_t soloer);

  // sync receive
  uint32_t recv_player(int conn_socket);

  // sync receive
  void recv_measure(int director, struct measure_s *newMeasure);

  // async send
  void send_to_play(int player, int director, struct play_measure_s *measure);
```

Midi Player:
```c
  uint32_t recv_num_of_musicians(int director);

  // sync receive
  void recv_to_play(struct play_measure_s **note_list, struct list_head musicians_list);

  void send_ack(int director);
```

Director:
```c
  // sync receive
  uint32_t recv_player(int director);

  // sync receive
  void recv_subscription(int conn_socket, struct subscription_s *new_musician);

  // async send
  void send_num_of_musicians(int player_addr, uint32_t musicians_count);

  // async send: 0-> success, -1-> error
  int send_id(int musician_conn, uint32_t id);

  // async send
  void broadcast_measure(struct measure_s *next_measure, struct list_head *dests);

  // sync receive
  void sync_all(struct list_head *dests);
```

#Communication sequence diagram
![alt text](https://www.lucidchart.com/publicSegments/view/53b983ec-7b5c-4557-b037-60250a0092e1/image.png "Sequence diagram")
