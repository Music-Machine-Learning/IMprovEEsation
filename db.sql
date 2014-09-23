--
-- PostgreSQL database cluster dump
--

\connect postgres

SET default_transaction_read_only = off;

SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;

--
-- Roles
--

CREATE ROLE postgres;
ALTER ROLE postgres WITH SUPERUSER INHERIT CREATEROLE CREATEDB LOGIN REPLICATION;
CREATE ROLE read_only;
ALTER ROLE read_only WITH NOSUPERUSER INHERIT NOCREATEROLE NOCREATEDB LOGIN NOREPLICATION PASSWORD 'md54c37fd589c47b3d64e69baaec73ebfa2';

--
-- Database creation
--

CREATE DATABASE improveesation WITH TEMPLATE = template0 OWNER = postgres;
REVOKE ALL ON DATABASE improveesation FROM PUBLIC;
REVOKE ALL ON DATABASE improveesation FROM postgres;
GRANT ALL ON DATABASE improveesation TO postgres;
GRANT CONNECT,TEMPORARY ON DATABASE improveesation TO PUBLIC;
REVOKE ALL ON DATABASE postgres FROM PUBLIC;
REVOKE ALL ON DATABASE postgres FROM postgres;
GRANT ALL ON DATABASE postgres TO postgres;
REVOKE ALL ON DATABASE template1 FROM PUBLIC;
REVOKE ALL ON DATABASE template1 FROM postgres;
GRANT ALL ON DATABASE template1 TO postgres;
GRANT CONNECT ON DATABASE template1 TO PUBLIC;

\connect improveesation

SET default_transaction_read_only = off;

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

CREATE TABLE genre (
    id integer NOT NULL,
    name character varying(255)
);



CREATE SEQUENCE genre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE genre_id_seq OWNED BY genre.id;


CREATE TABLE instrument (
    id integer NOT NULL,
    octave_min smallint NOT NULL,
    octave_max smallint NOT NULL,
    name character varying(255) NOT NULL,
    id_class integer NOT NULL
);



CREATE TABLE instrument_class (
    id integer NOT NULL,
    name character varying(255) NOT NULL
);



CREATE SEQUENCE instrument_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;



ALTER SEQUENCE instrument_id_seq OWNED BY instrument.id;

CREATE TABLE pattern (
    id integer NOT NULL,
    measure_count integer,
    moods character varying(255)[],
    steps integer[] NOT NULL,
    dynamics character varying(255)[] NOT NULL,
    modes character varying(3)[] NOT NULL
);


CREATE SEQUENCE pattern_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pattern_id_seq OWNED BY pattern.id;

CREATE TABLE quarter (
    id integer NOT NULL,
    pos smallint NOT NULL,
    instrument_class smallint NOT NULL,
    chord_note smallint NOT NULL,
    chord_mode smallint NOT NULL,
    tag_dyna character varying(255) NOT NULL,
    tag_mood character varying(255) NOT NULL,
    scale_genre integer NOT NULL,
    solo boolean NOT NULL
);

CREATE SEQUENCE quarter_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE quarter_id_seq OWNED BY quarter.id;

CREATE TABLE scale (
    id smallint NOT NULL,
    description character varying(255) NOT NULL
);

CREATE TABLE scale_genre (
    id integer NOT NULL,
    id_scale integer NOT NULL,
    id_genre integer NOT NULL
);

CREATE SEQUENCE scale_genre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE scale_genre_id_seq OWNED BY scale_genre.id;

CREATE TABLE semiquaver (
    id integer NOT NULL,
    pos smallint NOT NULL,
    quarter integer NOT NULL,
    velocity_min smallint NOT NULL,
    velocity_max smallint NOT NULL,
    pchange double precision DEFAULT 0 NOT NULL,
    pchange_3qrt double precision DEFAULT 0 NOT NULL,
    pchange_3qvr double precision DEFAULT 0 NOT NULL,
    pchange_3smq double precision DEFAULT 0 NOT NULL,
    pnote double precision[] NOT NULL
);

CREATE SEQUENCE semiquaver_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE semiquaver_id_seq OWNED BY semiquaver.id;

CREATE TABLE subgenre (
    id integer NOT NULL,
    name character varying(255),
    id_genre integer
);

CREATE SEQUENCE subgenre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE subgenre_id_seq OWNED BY subgenre.id;

CREATE TABLE var_measure (
    id integer NOT NULL,
    start_meas integer,
    end_meas integer,
    target_patterns integer[],
    pattern integer
);

CREATE SEQUENCE var_measure_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE var_measure_id_seq OWNED BY var_measure.id;

ALTER TABLE ONLY genre ALTER COLUMN id SET DEFAULT nextval('genre_id_seq'::regclass);

ALTER TABLE ONLY pattern ALTER COLUMN id SET DEFAULT nextval('pattern_id_seq'::regclass);

ALTER TABLE ONLY quarter ALTER COLUMN id SET DEFAULT nextval('quarter_id_seq'::regclass);

ALTER TABLE ONLY scale_genre ALTER COLUMN id SET DEFAULT nextval('scale_genre_id_seq'::regclass);

ALTER TABLE ONLY semiquaver ALTER COLUMN id SET DEFAULT nextval('semiquaver_id_seq'::regclass);

ALTER TABLE ONLY subgenre ALTER COLUMN id SET DEFAULT nextval('subgenre_id_seq'::regclass);

ALTER TABLE ONLY var_measure ALTER COLUMN id SET DEFAULT nextval('var_measure_id_seq'::regclass);

COPY genre (id, name) FROM stdin;
1	blues
2	jazz
\.

SELECT pg_catalog.setval('genre_id_seq', 2, true);

COPY instrument (id, octave_min, octave_max, name, id_class) FROM stdin;
12	2	7	Marimba	1
127	0	7	Drum	15
0	0	7	Acoustic Grand Piano	0
1	0	7	Bright Acoustic Piano	0
2	0	7	Electric Grand Piano	0
3	0	7	Honky-tonk Piano	0
4	0	7	Electric Piano 1	0
5	0	7	Electric Piano 2	0
6	0	7	Harpsichord	0
7	1	7	Clavinet	0
10	0	7	Music Box	1
14	3	5	Tubular Bells	1
15	2	6	Dulcimer	1
16	0	7	Drawbar Organ	2
17	0	7	Percussive Organ	2
18	0	7	Rock Organ	2
19	0	7	Church Organ	2
20	0	7	Pump Organ	2
21	0	7	Accordion	2
13	2	6	Xylophone	1
24	2	6	Acoustic Guitar (nylon)	3
25	2	6	Acoustic Guitar (steel)	3
26	2	6	Electric Guitar (jazz)	3
27	2	6	Electric Guitar (clean)	3
28	2	6	Electric Guitar (muted)	3
29	2	6	Overdrive Guitar	3
30	2	6	Distortion Guitar	3
31	2	6	Guitar Harmonics	3
32	1	4	Acoustic Bass	4
34	1	4	Electric Bass (pick)	4
33	1	4	Electric Bass (finger)	4
35	1	4	Fretless Bass	4
36	1	4	Slap Bass 1	4
37	1	4	Slap Bass 2	4
38	1	4	Synth Bass 1	4
39	1	4	Synth Bass 2	4
8	4	7	Celesta	1
9	5	7	Glockenspiel	1
11	3	7	Vibraphone	1
23	0	7	Tango Accordion	2
22	3	7	Harmonica	2
\.

COPY instrument_class (id, name) FROM stdin;
0	Piano
1	Chromatic Percussion
2	Organ
3	Guitar
4	Bass
5	Strings
6	Ensemble
7	Brass
8	Reed
9	Pipe
10	Synth Lead
11	Synth Pad
12	Synth Effects
13	Ethnic
14	Percussive
15	Others
\.

SELECT pg_catalog.setval('instrument_id_seq', 1, false);

COPY pattern (id, measure_count, moods, steps, dynamics, modes) FROM stdin;
2	12	{pushed,slow}	{{0,-1},{5,-1},{0,-1},{7,0},{5,-1},{6,-1},{0,-1},{4,9},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,""},{7,""},{7,""},{m7,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}
3	12	{pushed,slow}	{{0,3},{8,11},{4,7},{0,0},{5,-1},{6,-1},{0,-1},{10,3},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,7},{7+,7},{7+,7},{7+,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}
1	12	{pushed,slow}	{{0},{0},{0},{0},{5},{5},{0},{0},{7},{7},{0},{0}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7}}
\.

SELECT pg_catalog.setval('pattern_id_seq', 4, true);


COPY quarter (id, pos, instrument_class, chord_note, chord_mode, tag_dyna, tag_mood, scale_genre, solo) FROM stdin;
29	0	2	0	145	groove	pushed	4	t
30	0	2	0	137	groove	pushed	4	t
32	1	2	0	145	groove	pushed	4	t
33	1	2	0	137	groove	pushed	4	t
34	2	2	5	145	groove	pushed	4	t
35	2	2	5	137	groove	pushed	4	t
36	3	2	9	145	groove	pushed	4	t
37	3	2	7	1169	groove	pushed	4	t
21	0	4	0	145	groove	pushed	4	f
22	0	4	0	137	groove	pushed	4	f
23	1	4	0	145	groove	pushed	4	f
24	2	4	0	145	groove	pushed	4	f
25	3	4	0	145	groove	pushed	4	f
26	1	4	0	137	groove	pushed	4	f
27	2	4	0	137	groove	pushed	4	f
28	3	4	0	137	groove	pushed	4	f
11	0	0	0	145	groove	pushed	4	f
12	0	0	0	137	groove	pushed	4	f
13	0	0	0	1169	groove	pushed	4	f
10	3	0	7	1169	groove	pushed	4	f
7	1	0	2	145	groove	pushed	4	f
8	2	0	2	145	groove	pushed	4	f
5	2	0	9	137	groove	pushed	4	f
6	3	0	9	137	groove	pushed	4	f
3	2	0	0	145	groove	pushed	4	f
9	1	0	7	1169	groove	pushed	4	f
14	1	0	0	1169	groove	pushed	4	f
15	2	0	0	1169	groove	pushed	4	f
16	3	0	0	1169	groove	pushed	4	f
17	0	0	5	1169	groove	pushed	4	f
18	1	0	5	1169	groove	pushed	4	f
19	2	0	5	1169	groove	pushed	4	f
20	3	0	5	1169	groove	pushed	4	f
\.

SELECT pg_catalog.setval('quarter_id_seq', 37, true);

COPY scale (id, description) FROM stdin;
2741	major
2733	minor
1453	natural minor
1193	minor pentatonic
1257	minor hexatonic blues
669	major hexatonic blues
\.

COPY scale_genre (id, id_scale, id_genre) FROM stdin;
1	1193	1
2	1257	1
3	2741	2
4	669	1
\.

SELECT pg_catalog.setval('scale_genre_id_seq', 8, true);

COPY semiquaver (id, pos, quarter, velocity_min, velocity_max, pchange, pchange_3qrt, pchange_3qvr, pchange_3smq, pnote) FROM stdin;
10	1	7	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
11	3	7	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
12	1	8	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
15	3	8	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.25,0,0.25,0,0}
17	1	5	100	200	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
18	3	5	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
19	1	6	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
20	3	6	100	200	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
21	1	9	100	200	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
22	2	9	100	200	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
23	3	9	100	200	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
24	1	10	100	200	0.5	0	0	0	{0.349999999999999978,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
25	3	10	100	200	0.5	0	0	0	{0,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.200000000000000011,0,0,0.299999999999999989,0}
26	1	13	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
27	3	13	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
28	3	11	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
29	1	11	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
30	1	12	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
31	3	12	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
32	2	12	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
33	4	12	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
34	1	14	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
35	1	15	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
36	3	15	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
37	1	16	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
38	3	16	100	200	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
39	1	17	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
40	1	18	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
41	1	19	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
42	1	20	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
43	3	17	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
44	3	18	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
45	3	19	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
46	3	20	100	200	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
47	1	22	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
48	3	22	100	200	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
49	1	26	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
50	1	27	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
51	1	28	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
52	3	26	100	200	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
53	3	27	100	200	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
54	3	28	100	200	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
60	3	25	100	200	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
61	3	23	100	200	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
62	3	24	100	200	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
63	3	21	100	200	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
55	1	25	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
57	1	23	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
58	1	24	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
59	1	21	100	200	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
64	1	29	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
65	2	29	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
66	3	29	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
67	4	29	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
68	1	30	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
69	2	30	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
70	3	30	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
71	4	30	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
72	1	32	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
73	2	32	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
74	3	32	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
75	4	32	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
77	1	33	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
78	2	33	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
79	3	33	100	200	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
80	4	33	100	200	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
81	1	34	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
82	2	34	100	200	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
83	3	34	100	200	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
84	4	34	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
85	1	35	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
86	4	35	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
87	3	35	100	200	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
88	2	35	100	200	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
89	1	36	100	200	0.299999999999999989	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
90	2	36	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
91	3	36	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
92	4	36	100	200	0.200000000000000011	0	0	0	{0.299999999999999989,0.200000000000000011,0,0,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
93	1	37	100	200	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
94	2	37	100	200	0.100000000000000006	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
95	3	37	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
96	4	37	100	200	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0,0,0.299999999999999989,0,0}
\.

SELECT pg_catalog.setval('semiquaver_id_seq', 96, true);

COPY subgenre (id, name, id_genre) FROM stdin;
1	base	1
2	bebop	1
3	coltrane	1
4	free	2
5	smooth	2
6	swing	2
\.

SELECT pg_catalog.setval('subgenre_id_seq', 6, true);

COPY var_measure (id, start_meas, end_meas, target_patterns, pattern) FROM stdin;
2	0	4	{2,3}	1
\.

SELECT pg_catalog.setval('var_measure_id_seq', 2, true);

ALTER TABLE ONLY genre
    ADD CONSTRAINT genre_pkey PRIMARY KEY (id);

ALTER TABLE ONLY instrument_class
    ADD CONSTRAINT instrument_class_pkey PRIMARY KEY (id);

ALTER TABLE ONLY instrument
    ADD CONSTRAINT instrument_pkey PRIMARY KEY (id);

ALTER TABLE ONLY pattern
    ADD CONSTRAINT pattern_pkey PRIMARY KEY (id);

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_pkey PRIMARY KEY (id);

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_pkey PRIMARY KEY (id);

ALTER TABLE ONLY scale
    ADD CONSTRAINT scale_pkey PRIMARY KEY (id);

ALTER TABLE ONLY semiquaver
    ADD CONSTRAINT semiquaver_pkey PRIMARY KEY (id);

ALTER TABLE ONLY subgenre
    ADD CONSTRAINT subgenre_pkey PRIMARY KEY (id);

ALTER TABLE ONLY var_measure
    ADD CONSTRAINT var_measure_pkey PRIMARY KEY (id);

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_instrument_class_fkey FOREIGN KEY (instrument_class) REFERENCES instrument_class(id);

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_scale_genre_fkey FOREIGN KEY (scale_genre) REFERENCES scale_genre(id);

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_id_genre_fkey FOREIGN KEY (id_genre) REFERENCES genre(id);

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_id_scale_fkey FOREIGN KEY (id_scale) REFERENCES scale(id);

ALTER TABLE ONLY semiquaver
    ADD CONSTRAINT semiquaver_quarter_fkey FOREIGN KEY (quarter) REFERENCES quarter(id);

ALTER TABLE ONLY subgenre
    ADD CONSTRAINT subgenre_id_genre_fkey FOREIGN KEY (id_genre) REFERENCES genre(id);

ALTER TABLE ONLY var_measure
    ADD CONSTRAINT var_measure_pattern_fkey FOREIGN KEY (pattern) REFERENCES pattern(id);

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT USAGE ON SCHEMA public TO read_only;

REVOKE ALL ON TABLE genre FROM PUBLIC;
GRANT SELECT ON TABLE genre TO read_only;

REVOKE ALL ON SEQUENCE genre_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE genre_id_seq TO read_only;

REVOKE ALL ON TABLE instrument FROM PUBLIC;
GRANT SELECT ON TABLE instrument TO read_only;

REVOKE ALL ON TABLE instrument_class FROM PUBLIC;
GRANT SELECT ON TABLE instrument_class TO read_only;

REVOKE ALL ON SEQUENCE instrument_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE instrument_id_seq TO read_only;

REVOKE ALL ON TABLE pattern FROM PUBLIC;
GRANT SELECT ON TABLE pattern TO read_only;

REVOKE ALL ON SEQUENCE pattern_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE pattern_id_seq TO read_only;

REVOKE ALL ON TABLE quarter FROM PUBLIC;
GRANT SELECT ON TABLE quarter TO read_only;

REVOKE ALL ON SEQUENCE quarter_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE quarter_id_seq TO read_only;

REVOKE ALL ON TABLE scale FROM PUBLIC;
GRANT SELECT ON TABLE scale TO read_only;

REVOKE ALL ON TABLE scale_genre FROM PUBLIC;
GRANT SELECT ON TABLE scale_genre TO read_only;

REVOKE ALL ON SEQUENCE scale_genre_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE scale_genre_id_seq TO read_only;

REVOKE ALL ON TABLE semiquaver FROM PUBLIC;
GRANT SELECT ON TABLE semiquaver TO read_only;

REVOKE ALL ON SEQUENCE semiquaver_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE semiquaver_id_seq TO read_only;

REVOKE ALL ON TABLE subgenre FROM PUBLIC;
GRANT SELECT ON TABLE subgenre TO read_only;

REVOKE ALL ON SEQUENCE subgenre_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE subgenre_id_seq TO read_only;

REVOKE ALL ON TABLE var_measure FROM PUBLIC;
GRANT SELECT ON TABLE var_measure TO read_only;

REVOKE ALL ON SEQUENCE var_measure_id_seq FROM PUBLIC;
GRANT USAGE ON SEQUENCE var_measure_id_seq TO read_only;

ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public REVOKE ALL ON SEQUENCES  FROM PUBLIC;
ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public REVOKE ALL ON SEQUENCES  FROM read_only;
ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public GRANT USAGE ON SEQUENCES  TO read_only;

\connect postgres

SET default_transaction_read_only = off;

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: postgres; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON DATABASE postgres IS 'default administrative connection database';


--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;


--
-- PostgreSQL database dump complete
--

\connect template1

SET default_transaction_read_only = off;

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: template1; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON DATABASE template1 IS 'default template for new databases';


--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database cluster dump complete
--

