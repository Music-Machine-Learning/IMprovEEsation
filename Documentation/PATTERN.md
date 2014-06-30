#Pattern Definition

####TODO: everything's changed after the insertion of a relational database. Write down about it. Do not consider (or do it partially) the following statements.

A "pattern" is defined as a collection of measures.
 * instruments: the instruments that can play the measure (ex. bass, guitar)
 * tags: the set of words that characterize the dynamic of the measure (ex. blues, groove, fill)
 * time: the time signature (ex. 4/4, 3/4, 6/8)
 * measure: a CSV formatted string that defines the beats of the measure. We dived the measure in 128 beats. The first line of the string represents the syntax (ex. ID,CHANGEP,PAUSEP,1P,2P,3P,4P,5P,6P,7P,VELOCITY) while the following lines represent the associated data. For example a line like 32,0.4,0.0,0.4,0.0,0.0,0.0,0.6,0.0,0.0,80 says that the 32nd beat of the measure has a probability of 0.4 to change the current note, a 0.4 probabilty that the changed note is the 1st of scale, a 0.6 probability that the changed note is the 5th of the scale and so on. Moreover we use the MIDI velocity value in order to express the intensity of the playing note.
