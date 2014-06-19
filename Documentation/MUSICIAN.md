The Musician Process
== 

A musician is one of the core part of our software. His main goal is to play music! Good music? He should try to.
To do so, the musician process passes his entire life playing some notes that can 'fit' within the music that the other musicians play. The musicians are not left alone in this complex work but a director helps them to coordinate each others. To better understand how a director makes his decision check the corresponding documentation file. 
From the musicians point of the view, the director decides and tells to everyone some global parameters like:
 * Key signature {note, mode('mj','min')}
 * Chord { keymode('mj','min'), chordgrade(1-12), chordmode('mj','min','7','7+') }
 * Time signature { upper, lower }
 * BPM
 * Tags { dynamic[], genre[], mood[] }
 
An example could be:
 * {A, min}
 * {min, 4, mj} (Indicates a C Major)
 * {4, 4}
 * 120
 * {["groove"], ["blues", "rockabilly"], ["intro", "soft"]}


According to these parameters the musician can take his behavior decisions. Basically he looks for patterns in a global database. These patterns are data collections that express how the single notes can be played within a certain probability.

## Musician Main Loop
