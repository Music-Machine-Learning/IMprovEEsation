The player should be runned with the command ./player, using the option --test to ensure that it's working (it works standalone).
If so, you should hear some scales and a stupid drum beat.

#Setup

To setup our midi infrastructure we should have timidity installed:

	sudo apt-get install timidity

First, we should enable the MIDI module (check with lsmod if it's effectively enabled):

	sudo modprobe snd_virmidi

Then we should run timidity as follows:

	timidity -iA -Os -D 11 --verbose=1

meaning that we want timidity enabled in background with ALSA sequencer as input and output and the drumset playing on the 10th channel (we expressed 11 because the channel list in timidity starts from 1 unlike the ALSA interface). We need the verbosity as it tells us the successful selection of instruments.

Assuming that we want to use the input midi system interface identified by 20 (we can check them using aconnect -i -l) and timidity using the id 128 (we can check this using aconnect -o -l) we must connect the two interfaces with the command:

	aconnect 20:0 128:0

Now we're ready to play our fantastic scale! :-)


#TODO

Drums must be set by the director always in the 10th channel, otherwise it won't play.
Drums are not identified by an id.

Triplets are not active at the moment.

I need the notes to be splitted in measures:
	a note which occupies a span between two measures cannot be represented by the struct play_measure as it is, because it is assumed that the first note of the list starts on the "1", i.e. there's no way to tell me that the note starts later.


