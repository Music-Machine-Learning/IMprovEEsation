Alternate measure representation for monophonic instruments:

For each measure fraction on which a change can happen (e.g. pause and change probabilities are greater than 0) there will be an oriented graph G={a,n} where n are all the 12 notes and for each vertex there must be at leats one exiting edge.
Each note represented by a vertex is the abstract nth half step from the fundamental. Each edge has a weight that represents the percentage probability of changing to the destination vertex.

A single measure fraction could be expressed like:

0 0 60
0 1 10
0 4 80
0 5 80
0 7 40
0 10 10
1 0 80
1 2 40
1 5 80
1 7 80
2 1 10
2 2 40
2 3 10
2 5 60
2 7 50
3 1 70
3 2 10
3 4 80
3 5 60
4 1 80
4 3 50
4 4 60
4 5 70
4 10 50
5 0 40
5 4 75
5 5 60
5 6 60
5 10 40
6 5 70
6 7 70
6 10 20
7 0 50
7 5 80
7 6 60
7 7 50
7 8 20
7 10 60
8 7 80
8 9 20
9 8 20
9 10 80
10 0 40
10 4 60
10 5 60
10 9 20
10 11 20
11 10 70
11 0 80

The average size for a measure structure built up like this woud be of 50 KB (46KB -> Graphs + 256 B -> pause/change probabilities + measure metadata)

The e same concept could be applied to chord selection based on genre patterns.

