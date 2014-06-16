The Musician Process
== 

A musician is one of the core part of our software. His main goal is to play music! Good music? He should try to.
To do so, the musician process passes his entire life playing some notes that can 'fit' within the music that the other musicians play. The musicians are not left alone in this complex work but a director helps them to coordinate each others. To better understand how a director makes his decision check the corresponding documentation file. 
From the musicians point of the view, the director decides and tells to everyone some global parameters like:
 * Key signature {note, [MINOR,MAJOR]}
 * Chord { note(1-12), third[0,MINOR,MAJOR], fifth[0,PERFECT,DIMINISHED,AUGMENTED], seventh[0,DIMINISHED,MINOR,MAJOR] }
 * Time signature { upper, lower }
 * BPM
 * Tags { dynamic[], genre[], mood[] }
 
An example could be:
 * {A, MINOR}
 * { 4, MAJOR, PERFECT, 0 } (Indicates a C Major with the perfect 5th and without the seventh)
 * { 4, 4}
 * 120
 * { ["groove"], ["blues", "rockabilly"], ["intro", "soft"] }

