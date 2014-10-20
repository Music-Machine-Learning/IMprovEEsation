--
-- PostgreSQL database cluster dump
--

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

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
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

--
-- Name: genre; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE genre (
    id integer NOT NULL,
    name character varying(255)
);


ALTER TABLE public.genre OWNER TO postgres;

--
-- Name: genre_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE genre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.genre_id_seq OWNER TO postgres;

--
-- Name: genre_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE genre_id_seq OWNED BY genre.id;


--
-- Name: instrument; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE instrument (
    id integer NOT NULL,
    octave_min smallint NOT NULL,
    octave_max smallint NOT NULL,
    name character varying(255) NOT NULL,
    id_class integer NOT NULL
);


ALTER TABLE public.instrument OWNER TO postgres;

--
-- Name: instrument_class; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE instrument_class (
    id integer NOT NULL,
    name character varying(255) NOT NULL
);


ALTER TABLE public.instrument_class OWNER TO postgres;

--
-- Name: instrument_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE instrument_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.instrument_id_seq OWNER TO postgres;

--
-- Name: instrument_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE instrument_id_seq OWNED BY instrument.id;


--
-- Name: pattern; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE pattern (
    id integer NOT NULL,
    measure_count integer,
    moods character varying(255)[],
    steps integer[] NOT NULL,
    dynamics character varying(255)[] NOT NULL,
    modes character varying(3)[] NOT NULL
);


ALTER TABLE public.pattern OWNER TO postgres;

--
-- Name: pattern_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE pattern_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.pattern_id_seq OWNER TO postgres;

--
-- Name: pattern_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE pattern_id_seq OWNED BY pattern.id;


--
-- Name: quarter; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

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


ALTER TABLE public.quarter OWNER TO postgres;

--
-- Name: quarter_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE quarter_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.quarter_id_seq OWNER TO postgres;

--
-- Name: quarter_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE quarter_id_seq OWNED BY quarter.id;


--
-- Name: scale; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE scale (
    id smallint NOT NULL,
    description character varying(255) NOT NULL
);


ALTER TABLE public.scale OWNER TO postgres;

--
-- Name: scale_genre; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE scale_genre (
    id integer NOT NULL,
    id_scale integer NOT NULL,
    id_genre integer NOT NULL
);


ALTER TABLE public.scale_genre OWNER TO postgres;

--
-- Name: scale_genre_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE scale_genre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.scale_genre_id_seq OWNER TO postgres;

--
-- Name: scale_genre_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE scale_genre_id_seq OWNED BY scale_genre.id;


--
-- Name: semiquaver; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

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


ALTER TABLE public.semiquaver OWNER TO postgres;

--
-- Name: semiquaver_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE semiquaver_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.semiquaver_id_seq OWNER TO postgres;

--
-- Name: semiquaver_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE semiquaver_id_seq OWNED BY semiquaver.id;


--
-- Name: subgenre; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE subgenre (
    id integer NOT NULL,
    name character varying(255),
    id_genre integer
);


ALTER TABLE public.subgenre OWNER TO postgres;

--
-- Name: subgenre_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE subgenre_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.subgenre_id_seq OWNER TO postgres;

--
-- Name: subgenre_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE subgenre_id_seq OWNED BY subgenre.id;


--
-- Name: var_measure; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE var_measure (
    id integer NOT NULL,
    start_meas integer,
    end_meas integer,
    target_patterns integer[],
    pattern integer
);


ALTER TABLE public.var_measure OWNER TO postgres;

--
-- Name: var_measure_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE var_measure_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.var_measure_id_seq OWNER TO postgres;

--
-- Name: var_measure_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE var_measure_id_seq OWNED BY var_measure.id;


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY genre ALTER COLUMN id SET DEFAULT nextval('genre_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY pattern ALTER COLUMN id SET DEFAULT nextval('pattern_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY quarter ALTER COLUMN id SET DEFAULT nextval('quarter_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY scale_genre ALTER COLUMN id SET DEFAULT nextval('scale_genre_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY semiquaver ALTER COLUMN id SET DEFAULT nextval('semiquaver_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY subgenre ALTER COLUMN id SET DEFAULT nextval('subgenre_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY var_measure ALTER COLUMN id SET DEFAULT nextval('var_measure_id_seq'::regclass);


--
-- Data for Name: genre; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY genre (id, name) FROM stdin;
1	blues
2	jazz
\.


--
-- Name: genre_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('genre_id_seq', 2, true);


--
-- Data for Name: instrument; Type: TABLE DATA; Schema: public; Owner: postgres
--

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
8	4	7	Celesta	1
9	5	7	Glockenspiel	1
11	3	7	Vibraphone	1
23	0	7	Tango Accordion	2
22	3	7	Harmonica	2
32	1	2	Acoustic Bass	4
34	1	2	Electric Bass (pick)	4
33	1	2	Electric Bass (finger)	4
35	1	2	Fretless Bass	4
36	1	2	Slap Bass 1	4
37	1	2	Slap Bass 2	4
38	1	2	Synth Bass 1	4
39	1	2	Synth Bass 2	4
\.


--
-- Data for Name: instrument_class; Type: TABLE DATA; Schema: public; Owner: postgres
--

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


--
-- Name: instrument_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('instrument_id_seq', 1, false);


--
-- Data for Name: pattern; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY pattern (id, measure_count, moods, steps, dynamics, modes) FROM stdin;
2	12	{pushed,slow}	{{0,-1},{5,-1},{0,-1},{7,0},{5,-1},{6,-1},{0,-1},{4,9},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,""},{7,""},{7,""},{m7,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}
3	12	{pushed,slow}	{{0,3},{8,11},{4,7},{0,0},{5,-1},{6,-1},{0,-1},{10,3},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,7},{7+,7},{7+,7},{7+,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}
1	12	{pushed,slow}	{{0},{0},{0},{0},{5},{5},{0},{0},{7},{7},{0},{0}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7}}
\.


--
-- Name: pattern_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('pattern_id_seq', 4, true);


--
-- Data for Name: quarter; Type: TABLE DATA; Schema: public; Owner: postgres
--

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
9	1	0	7	1169	groove	pushed	4	f
14	1	0	0	1169	groove	pushed	4	f
15	2	0	0	1169	groove	pushed	4	f
16	3	0	0	1169	groove	pushed	4	f
17	0	0	5	1169	groove	pushed	4	f
18	1	0	5	1169	groove	pushed	4	f
19	2	0	5	1169	groove	pushed	4	f
20	3	0	5	1169	groove	pushed	4	f
163	0	4	3	145	groove	pushed	4	f
164	0	4	3	137	groove	pushed	4	f
165	1	4	3	145	groove	pushed	4	f
166	2	4	3	145	groove	pushed	4	f
167	3	4	3	145	groove	pushed	4	f
168	1	4	3	137	groove	pushed	4	f
169	2	4	3	137	groove	pushed	4	f
170	3	4	3	137	groove	pushed	4	f
171	0	4	5	145	groove	pushed	4	f
172	0	4	5	137	groove	pushed	4	f
173	1	4	5	145	groove	pushed	4	f
174	2	4	5	145	groove	pushed	4	f
175	3	4	5	145	groove	pushed	4	f
176	1	4	5	137	groove	pushed	4	f
177	2	4	5	137	groove	pushed	4	f
178	3	4	5	137	groove	pushed	4	f
179	0	4	7	145	groove	pushed	4	f
180	0	4	7	137	groove	pushed	4	f
181	1	4	7	145	groove	pushed	4	f
182	2	4	7	145	groove	pushed	4	f
183	3	4	7	145	groove	pushed	4	f
184	1	4	7	137	groove	pushed	4	f
185	2	4	7	137	groove	pushed	4	f
186	3	4	7	137	groove	pushed	4	f
187	0	4	9	145	groove	pushed	4	f
188	0	4	9	137	groove	pushed	4	f
189	1	4	9	145	groove	pushed	4	f
190	2	4	9	145	groove	pushed	4	f
191	3	4	9	145	groove	pushed	4	f
192	1	4	9	137	groove	pushed	4	f
193	2	4	9	137	groove	pushed	4	f
194	3	4	9	137	groove	pushed	4	f
195	0	4	10	145	groove	pushed	4	f
196	0	4	10	137	groove	pushed	4	f
197	1	4	10	145	groove	pushed	4	f
198	2	4	10	145	groove	pushed	4	f
199	3	4	10	145	groove	pushed	4	f
200	1	4	10	137	groove	pushed	4	f
201	2	4	10	137	groove	pushed	4	f
202	3	4	10	137	groove	pushed	4	f
38	0	0	5	1169	fill	pushed	4	f
39	0	4	5	1169	fill	pushed	4	f
40	1	4	5	1169	fill	pushed	4	f
41	1	0	5	1169	fill	pushed	4	f
\.


--
-- Name: quarter_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('quarter_id_seq', 202, true);


--
-- Data for Name: scale; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY scale (id, description) FROM stdin;
2741	major
2733	minor
1453	natural minor
1193	minor pentatonic
1257	minor hexatonic blues
669	major hexatonic blues
\.


--
-- Data for Name: scale_genre; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY scale_genre (id, id_scale, id_genre) FROM stdin;
1	1193	1
2	1257	1
3	2741	2
4	669	1
\.


--
-- Name: scale_genre_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('scale_genre_id_seq', 8, true);


--
-- Data for Name: semiquaver; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY semiquaver (id, pos, quarter, velocity_min, velocity_max, pchange, pchange_3qrt, pchange_3qvr, pchange_3smq, pnote) FROM stdin;
97	0	38	60	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
99	2	38	60	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
101	2	41	60	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
102	0	41	60	127	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0,0,0,0.100000000000000006,0,0,0,0}
98	0	39	60	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
175	0	195	60	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.599999999999999978,0}
177	0	197	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
179	0	198	60	127	0.100000000000000006	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
181	0	199	60	127	0.299999999999999989	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
161	0	181	60	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011}
17	0	5	60	127	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
19	0	6	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
21	0	9	60	127	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
24	0	10	60	127	0.5	0	0	0	{0.349999999999999978,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
26	0	13	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
29	0	11	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
30	0	12	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
34	0	14	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
68	0	30	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
72	0	32	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
77	0	33	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
81	0	34	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
85	0	35	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
89	0	36	60	127	0.299999999999999989	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
125	2	185	60	127	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
110	0	170	60	127	0.400000000000000022	0	0	0	{0,0,0,0,0.699999999999999956,0,0,0.149999999999999994,0,0,0,0.149999999999999994,0}
112	0	172	60	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
114	0	176	60	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
116	0	177	60	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.5,0,0,0.25,0,0,0}
118	0	178	60	127	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.550000000000000044,0,0,0,0,0,0}
120	0	180	60	127	0.299999999999999989	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0,0}
122	0	184	60	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
124	0	185	60	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
166	2	183	60	127	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.25,0,0,0,0.25}
132	0	193	60	127	0.100000000000000006	0	0	0	{0,0.200000000000000011,0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0}
35	0	15	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
37	0	16	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
39	0	17	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
40	0	18	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
41	0	19	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
42	0	20	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
47	0	22	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
49	0	26	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
50	0	27	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
51	0	28	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
55	0	25	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
57	0	23	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
58	0	24	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
59	0	21	60	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
131	2	192	60	127	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.400000000000000022,0,0}
64	0	29	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
168	2	187	60	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
73	1	32	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
74	2	32	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
75	3	32	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
78	1	33	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
126	0	186	60	127	0.400000000000000022	0	0	0	{0,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0.0500000000000000028,0}
128	0	188	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
130	0	192	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
79	2	33	60	127	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
80	3	33	60	127	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
82	1	34	60	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
83	2	34	60	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
84	3	34	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
86	3	35	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
87	2	35	60	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
88	1	35	60	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
90	1	36	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
91	2	36	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
48	2	22	60	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
134	0	194	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.5,0,0}
136	0	196	60	127	0.800000000000000044	0	0	0	{0.25,0,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0,0,0.400000000000000022,0}
138	0	200	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
140	0	201	60	127	0.800000000000000044	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
142	0	202	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
144	0	163	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
146	0	165	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011,0,0,0.25,0}
148	0	166	60	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0.5,0,0,0,0,0,0,0.25,0}
150	0	167	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
153	0	173	60	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
155	0	174	60	127	0.800000000000000044	0	0	0	{0,0.0500000000000000028,0,0,0,0,0.450000000000000011,0,0,0,0.5,0,0}
157	0	175	60	127	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.349999999999999978,0,0,0,0.200000000000000011,0,0}
159	0	179	60	127	0.100000000000000006	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.650000000000000022,0,0,0,0.100000000000000006}
163	0	182	60	127	0.800000000000000044	0	0	0	{0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0,0,0,0.200000000000000011}
165	0	183	60	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
103	0	40	60	127	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0.100000000000000006,0,0,0,0,0,0,0}
104	0	164	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0.650000000000000022,0,0,0.100000000000000006,0,0,0,0.25,0}
106	0	168	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
108	0	169	60	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0.25,0,0,0.25,0,0,0,0.25,0}
100	2	40	60	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
105	2	164	60	127	0.400000000000000022	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
107	2	168	60	127	0.699999999999999956	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
109	2	169	60	127	0.5	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
111	2	170	60	127	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
113	2	172	60	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.299999999999999989,0,0,0.200000000000000011,0,0,0}
115	2	176	60	127	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
117	2	177	60	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
119	2	178	60	127	0.5	0	0	0	{0.25,0,0,0,0,0,0.5,0,0,0.25,0,0,0}
121	2	180	60	127	0.5	0	0	0	{0,0,0,0.25,0,0,0,0,0.25,0,0,0.5,0}
123	2	184	60	127	0.200000000000000011	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.0500000000000000028,0,0,0.149999999999999994,0}
127	2	186	60	127	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.5,0,0,0,0}
129	2	188	60	127	0.100000000000000006	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
133	2	193	60	127	0.5	0	0	0	{0.25,0,0,0,0,0.25,0,0,0,0,0.5,0,0}
135	2	194	60	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
137	2	196	60	127	0.299999999999999989	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
139	2	200	60	127	0.5	0	0	0	{0,0,0.100000000000000006,0,0,0,0.400000000000000022,0,0,0,0,0.5,0}
141	2	201	60	127	0.100000000000000006	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
143	2	202	60	127	0.100000000000000006	0	0	0	{0,0,0.200000000000000011,0,0,0,0.299999999999999989,0,0,0,0,0.5,0}
145	2	163	60	127	0.100000000000000006	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
147	2	165	60	127	0.5	0	0	0	{0,0,0,0,0.299999999999999989,0,0,0,0.200000000000000011,0,0,0.5,0}
149	2	166	60	127	0.299999999999999989	0	0	0	{0.25,0,0,0,0.25,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
151	2	167	60	127	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
152	2	171	60	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006,0,0}
154	2	173	60	127	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
156	2	174	60	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
158	2	175	60	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
160	2	179	60	127	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
162	2	181	60	127	0.5	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.25,0,0,0,0}
164	2	182	60	127	0.200000000000000011	0	0	0	{0,0,0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006}
170	2	189	60	127	0.5	0	0	0	{0.200000000000000011,0,0,0,0,0.400000000000000022,0,0,0,0,0.400000000000000022,0,0}
11	2	7	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
15	2	8	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.25,0,0.25,0,0}
18	2	5	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
20	2	6	60	127	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
22	1	9	60	127	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
23	2	9	60	127	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
25	2	10	60	127	0.5	0	0	0	{0,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.200000000000000011,0,0,0.299999999999999989,0}
27	2	13	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
28	2	11	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
31	2	12	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
32	1	12	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
33	3	12	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
36	2	15	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
38	2	16	60	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
43	2	17	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
44	2	18	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
45	2	19	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
46	2	20	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
52	2	26	60	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
53	2	27	60	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
54	2	28	60	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
60	2	25	60	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
61	2	23	60	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
62	2	24	60	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
63	2	21	60	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
65	1	29	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
66	2	29	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
67	3	29	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
69	1	30	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
70	2	30	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
71	3	30	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
92	3	36	60	127	0.200000000000000011	0	0	0	{0.299999999999999989,0.200000000000000011,0,0,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
94	1	37	60	127	0.100000000000000006	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
95	2	37	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
96	3	37	60	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0,0,0.299999999999999989,0,0}
172	2	190	60	127	0.5	0	0	0	{0.25,0,0.200000000000000011,0,0,0.25,0,0,0,0,0.299999999999999989,0,0}
174	2	191	60	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
176	2	195	60	127	0.299999999999999989	0	0	0	{0,0,0.25,0.25,0,0,0.25,0,0,0,0,0.25,0}
167	0	187	60	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0.200000000000000011,0,0,0.25,0,0,0,0,0.75,0,0}
169	0	189	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
171	0	190	60	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
10	0	7	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
12	0	8	60	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
178	2	197	60	127	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
180	2	198	60	127	0.5	0	0	0	{0,0,0,0.25,0,0,0.25,0,0,0,0,0.5,0}
182	2	199	60	127	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
93	0	37	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
173	0	191	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
\.


--
-- Name: semiquaver_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('semiquaver_id_seq', 182, true);


--
-- Data for Name: subgenre; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY subgenre (id, name, id_genre) FROM stdin;
1	base	1
2	bebop	1
3	coltrane	1
4	free	2
5	smooth	2
6	swing	2
\.


--
-- Name: subgenre_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('subgenre_id_seq', 6, true);


--
-- Data for Name: var_measure; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY var_measure (id, start_meas, end_meas, target_patterns, pattern) FROM stdin;
2	0	4	{2,3}	1
\.


--
-- Name: var_measure_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('var_measure_id_seq', 2, true);


--
-- Name: genre_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY genre
    ADD CONSTRAINT genre_pkey PRIMARY KEY (id);


--
-- Name: instrument_class_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY instrument_class
    ADD CONSTRAINT instrument_class_pkey PRIMARY KEY (id);


--
-- Name: instrument_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY instrument
    ADD CONSTRAINT instrument_pkey PRIMARY KEY (id);


--
-- Name: pattern_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY pattern
    ADD CONSTRAINT pattern_pkey PRIMARY KEY (id);


--
-- Name: quarter_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_pkey PRIMARY KEY (id);


--
-- Name: scale_genre_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_pkey PRIMARY KEY (id);


--
-- Name: scale_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY scale
    ADD CONSTRAINT scale_pkey PRIMARY KEY (id);


--
-- Name: semiquaver_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY semiquaver
    ADD CONSTRAINT semiquaver_pkey PRIMARY KEY (id);


--
-- Name: subgenre_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY subgenre
    ADD CONSTRAINT subgenre_pkey PRIMARY KEY (id);


--
-- Name: var_measure_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY var_measure
    ADD CONSTRAINT var_measure_pkey PRIMARY KEY (id);


--
-- Name: quarter_instrument_class_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_instrument_class_fkey FOREIGN KEY (instrument_class) REFERENCES instrument_class(id);


--
-- Name: quarter_scale_genre_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY quarter
    ADD CONSTRAINT quarter_scale_genre_fkey FOREIGN KEY (scale_genre) REFERENCES scale_genre(id);


--
-- Name: scale_genre_id_genre_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_id_genre_fkey FOREIGN KEY (id_genre) REFERENCES genre(id);


--
-- Name: scale_genre_id_scale_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY scale_genre
    ADD CONSTRAINT scale_genre_id_scale_fkey FOREIGN KEY (id_scale) REFERENCES scale(id);


--
-- Name: semiquaver_quarter_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY semiquaver
    ADD CONSTRAINT semiquaver_quarter_fkey FOREIGN KEY (quarter) REFERENCES quarter(id);


--
-- Name: subgenre_id_genre_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY subgenre
    ADD CONSTRAINT subgenre_id_genre_fkey FOREIGN KEY (id_genre) REFERENCES genre(id);


--
-- Name: var_measure_pattern_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY var_measure
    ADD CONSTRAINT var_measure_pattern_fkey FOREIGN KEY (pattern) REFERENCES pattern(id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT USAGE ON SCHEMA public TO read_only;


--
-- Name: genre; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE genre FROM PUBLIC;
REVOKE ALL ON TABLE genre FROM postgres;
GRANT ALL ON TABLE genre TO postgres;
GRANT SELECT ON TABLE genre TO read_only;


--
-- Name: genre_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE genre_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE genre_id_seq FROM postgres;
GRANT ALL ON SEQUENCE genre_id_seq TO postgres;
GRANT USAGE ON SEQUENCE genre_id_seq TO read_only;


--
-- Name: instrument; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE instrument FROM PUBLIC;
REVOKE ALL ON TABLE instrument FROM postgres;
GRANT ALL ON TABLE instrument TO postgres;
GRANT SELECT ON TABLE instrument TO read_only;


--
-- Name: instrument_class; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE instrument_class FROM PUBLIC;
REVOKE ALL ON TABLE instrument_class FROM postgres;
GRANT ALL ON TABLE instrument_class TO postgres;
GRANT SELECT ON TABLE instrument_class TO read_only;


--
-- Name: instrument_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE instrument_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE instrument_id_seq FROM postgres;
GRANT ALL ON SEQUENCE instrument_id_seq TO postgres;
GRANT USAGE ON SEQUENCE instrument_id_seq TO read_only;


--
-- Name: pattern; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE pattern FROM PUBLIC;
REVOKE ALL ON TABLE pattern FROM postgres;
GRANT ALL ON TABLE pattern TO postgres;
GRANT SELECT ON TABLE pattern TO read_only;


--
-- Name: pattern_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE pattern_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE pattern_id_seq FROM postgres;
GRANT ALL ON SEQUENCE pattern_id_seq TO postgres;
GRANT USAGE ON SEQUENCE pattern_id_seq TO read_only;


--
-- Name: quarter; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE quarter FROM PUBLIC;
REVOKE ALL ON TABLE quarter FROM postgres;
GRANT ALL ON TABLE quarter TO postgres;
GRANT SELECT ON TABLE quarter TO read_only;


--
-- Name: quarter_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE quarter_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE quarter_id_seq FROM postgres;
GRANT ALL ON SEQUENCE quarter_id_seq TO postgres;
GRANT USAGE ON SEQUENCE quarter_id_seq TO read_only;


--
-- Name: scale; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE scale FROM PUBLIC;
REVOKE ALL ON TABLE scale FROM postgres;
GRANT ALL ON TABLE scale TO postgres;
GRANT SELECT ON TABLE scale TO read_only;


--
-- Name: scale_genre; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE scale_genre FROM PUBLIC;
REVOKE ALL ON TABLE scale_genre FROM postgres;
GRANT ALL ON TABLE scale_genre TO postgres;
GRANT SELECT ON TABLE scale_genre TO read_only;


--
-- Name: scale_genre_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE scale_genre_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE scale_genre_id_seq FROM postgres;
GRANT ALL ON SEQUENCE scale_genre_id_seq TO postgres;
GRANT USAGE ON SEQUENCE scale_genre_id_seq TO read_only;


--
-- Name: semiquaver; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE semiquaver FROM PUBLIC;
REVOKE ALL ON TABLE semiquaver FROM postgres;
GRANT ALL ON TABLE semiquaver TO postgres;
GRANT SELECT ON TABLE semiquaver TO read_only;


--
-- Name: semiquaver_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE semiquaver_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE semiquaver_id_seq FROM postgres;
GRANT ALL ON SEQUENCE semiquaver_id_seq TO postgres;
GRANT USAGE ON SEQUENCE semiquaver_id_seq TO read_only;


--
-- Name: subgenre; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE subgenre FROM PUBLIC;
REVOKE ALL ON TABLE subgenre FROM postgres;
GRANT ALL ON TABLE subgenre TO postgres;
GRANT SELECT ON TABLE subgenre TO read_only;


--
-- Name: subgenre_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE subgenre_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE subgenre_id_seq FROM postgres;
GRANT ALL ON SEQUENCE subgenre_id_seq TO postgres;
GRANT USAGE ON SEQUENCE subgenre_id_seq TO read_only;


--
-- Name: var_measure; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE var_measure FROM PUBLIC;
REVOKE ALL ON TABLE var_measure FROM postgres;
GRANT ALL ON TABLE var_measure TO postgres;
GRANT SELECT ON TABLE var_measure TO read_only;


--
-- Name: var_measure_id_seq; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON SEQUENCE var_measure_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE var_measure_id_seq FROM postgres;
GRANT ALL ON SEQUENCE var_measure_id_seq TO postgres;
GRANT USAGE ON SEQUENCE var_measure_id_seq TO read_only;


--
-- Name: DEFAULT PRIVILEGES FOR SEQUENCES; Type: DEFAULT ACL; Schema: public; Owner: read_only
--

ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public REVOKE ALL ON SEQUENCES  FROM PUBLIC;
ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public REVOKE ALL ON SEQUENCES  FROM read_only;
ALTER DEFAULT PRIVILEGES FOR ROLE read_only IN SCHEMA public GRANT USAGE ON SEQUENCES  TO read_only;


--
-- PostgreSQL database dump complete
--

\connect postgres

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
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

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
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

