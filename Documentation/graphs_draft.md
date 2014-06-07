Alternate measure representation for monophonic instruments:

For each measure fraction on which a change can happen (e.g. pause and change probabilities are greater than 0) there will be an oriented graph G={a,n} where n are all the 12 notes and for each vertex there must be at leats one exiting edge.
Each note represented by a vertex is the abstract nth half step from the fundamental. Each edge has a weight that represents the percentage probability of changing to the destination vertex.

A single measure fraction could be expressed like a list of touples <from_note,to_note,probability>.

This is an exemple:

{
	0, 0, 20
	0, 1, 5
	0, 4, 30
	0, 5, 30
	0, 7, 10
	0, 10, 5

	1, 0, 30
	1, 2, 10
	1, 5, 30
	1, 7, 30

	2, 1, 5
	2, 2, 20
	2, 3, 5
	2, 5, 40
	2, 7, 30

	3, 1, 30
	3, 2, 5
	3, 4, 50
	3, 5, 15

	4, 1, 50
	4, 3, 5
	4, 4, 10
	4, 5, 30
	4, 10, 5

	5, 0, 10
	5, 4, 30
	5, 5, 25
	5, 6, 25
	5, 10, 10

	6, 5, 45
	6, 7, 45
	6, 10, 10

	7, 0, 10
	7, 5, 50
	7, 6, 5
	7, 7, 10
	7, 8, 5
	7, 10, 20

	8, 7, 80
	8, 9, 20

	9, 8, 20
	9, 10, 80

	10, 0, 20
	10, 4, 35
	10, 5, 35
	10, 9, 5
	10, 11, 5

	11, 10, 35
	11, 0, 65
}

The average size for a measure structure built up like this woud be of 50 KB (46KB -> Graphs + 256 B -> pause/change probabilities + measure metadata)

The e same concept could be applied to chord selection based on genre patterns.

