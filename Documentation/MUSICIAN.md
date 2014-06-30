The Musician Process
== 

A musician is one of the core part of our software. His main goal is to play music! Good music? He should try to.
To do so, the musician process passes his entire life playing some notes that can 'fit' within the music that the other musicians play. The musicians are not left alone in this complex work but a director helps them to coordinate each others. To better understand how a director makes his decision check the corresponding documentation file. 
From the musicians point of the view, the director decides and tells to everyone some global parameters.
To see which this parameters are, check the Protocol file.

According to these parameters the musician can take his behavior decisions. Basically he looks for patterns in a global database. These patterns are data collections that express how the single notes can be played within a certain probability.

The following pseudocode explains the life cycle of a musician.

## Musician Main Loop
```c

find_good_semiquaver(DB, data, semiquavers)
{
	/* This is the musician logic part. At each time step he asks to his mind 
	 * what he should play. Then his mind looks in the database asking some 
	 * GOOD query according to the data retreived from the director.
	 * Moreover the musician mind chooses if he should play strictly the found
	 * note or if he should changes the note in some random manner.
	 */
}

musician_process()
{
	lookup_id = 0;
	semiquavers = [];
	register(director, playing_process);
	while(1){
		data = get_director_data(lookup_id); //blocking call each measure
		sqcount = countSemiquavers(data.timesignature); // in 4/4 there are 16th semiquavers
		for (i = 0; i < sqcount; i++) {
			note = find_good_semiquaver(DB, data, semiquavers); //could return NO_CHANGE constant
	
			semiquavers.append(note)
	
			send(playing_process, note, lookup_id, data.bpm);
	
			
		}
		lookup_id++;
	}
}


playing_process()
{
	playing_id = 0;
	last_bpm = 0;
	while(1){

		notes = recvfromall(playing_id); //sync with the all musicians

		play(notes);

		playing_id++;
	}
}
 
```
