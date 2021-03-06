--
-- PostgreSQL database cluster dump
--

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
    modes character varying(3)[] NOT NULL,
    prioargs_id integer NOT NULL
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
-- Name: prioargs; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE prioargs (
    id integer NOT NULL,
    prioarray integer[] NOT NULL
);


ALTER TABLE public.prioargs OWNER TO postgres;

--
-- Name: prioargs_instrument; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE prioargs_instrument (
    id integer NOT NULL,
    prioargs_id integer,
    instrument_id integer NOT NULL,
    solo boolean NOT NULL
);


ALTER TABLE public.prioargs_instrument OWNER TO postgres;

--
-- Name: prioargs_instrument_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE prioargs_instrument_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.prioargs_instrument_id_seq OWNER TO postgres;

--
-- Name: prioargs_instrument_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE prioargs_instrument_id_seq OWNED BY prioargs_instrument.id;


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

ALTER TABLE ONLY prioargs_instrument ALTER COLUMN id SET DEFAULT nextval('prioargs_instrument_id_seq'::regclass);


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

COPY pattern (id, measure_count, moods, steps, dynamics, modes, prioargs_id) FROM stdin;
2	12	{pushed,slow}	{{0,-1},{5,-1},{0,-1},{7,0},{5,-1},{6,-1},{0,-1},{4,9},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,""},{7,""},{7,""},{m7,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}	0
3	12	{pushed,slow}	{{0,3},{8,11},{4,7},{0,0},{5,-1},{6,-1},{0,-1},{10,3},{2,-1},{7,-1},{4,9},{2,7}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7,7},{7+,7},{7+,7},{7+,7},{7,""},{m7,""},{7,""},{m7,7},{m7,""},{7,""},{m7,7},{m7,7}}	0
1	12	{pushed,slow}	{{0},{0},{0},{0},{5},{5},{0},{0},{7},{7},{0},{0}}	{groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,groove,fill}	{{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7},{7}}	0
\.


--
-- Name: pattern_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('pattern_id_seq', 4, true);


--
-- Data for Name: prioargs; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY prioargs (id, prioarray) FROM stdin;
0	{6,5,0,8,7,4,1,3,2}
1	{2,3,7,4,8,6,5,0,1}
2	{2,3,0,6,5,1,4,7,8}
\.


--
-- Data for Name: prioargs_instrument; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY prioargs_instrument (id, prioargs_id, instrument_id, solo) FROM stdin;
7	\N	12	f
9	\N	0	f
10	\N	1	f
11	\N	2	f
12	\N	3	f
13	\N	4	f
14	\N	5	f
15	\N	6	f
16	\N	7	f
17	\N	10	f
18	\N	14	f
19	\N	15	f
20	\N	16	f
21	\N	17	f
22	\N	18	f
23	\N	19	f
24	\N	20	f
25	\N	21	f
26	\N	13	f
27	\N	24	f
28	\N	25	f
29	\N	26	f
30	\N	27	f
31	\N	28	f
32	\N	29	f
33	\N	30	f
34	\N	31	f
35	\N	8	f
36	\N	9	f
37	\N	11	f
38	\N	23	f
39	\N	22	f
40	\N	32	f
41	\N	34	f
42	\N	33	f
43	\N	35	f
44	\N	36	f
45	\N	37	f
46	\N	38	f
47	\N	39	f
48	\N	12	t
50	\N	0	t
51	\N	1	t
52	\N	2	t
53	\N	3	t
54	\N	4	t
55	\N	5	t
56	\N	6	t
57	\N	7	t
58	\N	10	t
59	\N	14	t
60	\N	15	t
67	\N	13	t
68	\N	24	t
69	\N	25	t
70	\N	26	t
71	\N	27	t
72	\N	28	t
73	\N	29	t
74	\N	30	t
75	\N	31	t
76	\N	8	t
77	\N	9	t
78	\N	11	t
81	\N	32	t
82	\N	34	t
83	\N	33	t
84	\N	35	t
85	\N	36	t
86	\N	37	t
87	\N	38	t
88	\N	39	t
2	1	127	f
49	1	127	t
61	2	16	t
62	2	17	t
63	2	18	t
64	2	19	t
65	2	20	t
66	2	21	t
79	2	23	t
80	2	22	t
\.


--
-- Name: prioargs_instrument_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('prioargs_instrument_id_seq', 88, true);


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
207	2	0	9	137	groove	slow	4	f
208	3	0	9	137	groove	slow	4	f
209	1	0	2	145	groove	slow	4	f
210	2	0	2	145	groove	slow	4	f
211	1	0	7	1169	groove	slow	4	f
212	3	0	7	1169	groove	slow	4	f
213	0	0	0	145	groove	slow	4	f
214	0	0	0	137	groove	slow	4	f
215	0	0	0	1169	groove	slow	4	f
216	1	0	0	1169	groove	slow	4	f
217	2	0	0	1169	groove	slow	4	f
218	3	0	0	1169	groove	slow	4	f
219	0	0	5	1169	groove	slow	4	f
220	1	0	5	1169	groove	slow	4	f
221	2	0	5	1169	groove	slow	4	f
222	3	0	5	1169	groove	slow	4	f
223	0	4	0	145	groove	slow	4	f
224	0	4	0	137	groove	slow	4	f
225	1	4	0	145	groove	slow	4	f
226	2	4	0	145	groove	slow	4	f
227	3	4	0	145	groove	slow	4	f
228	1	4	0	137	groove	slow	4	f
229	2	4	0	137	groove	slow	4	f
230	3	4	0	137	groove	slow	4	f
231	0	2	0	145	groove	slow	4	t
232	0	2	0	137	groove	slow	4	t
234	1	2	0	145	groove	slow	4	t
235	1	2	0	137	groove	slow	4	t
236	2	2	5	145	groove	slow	4	t
237	2	2	5	137	groove	slow	4	t
238	3	2	9	145	groove	slow	4	t
239	3	2	7	1169	groove	slow	4	t
240	0	0	5	1169	fill	slow	4	f
241	0	4	5	1169	fill	slow	4	f
242	1	4	5	1169	fill	slow	4	f
243	1	0	5	1169	fill	slow	4	f
365	0	4	3	145	groove	slow	4	f
366	0	4	3	137	groove	slow	4	f
367	1	4	3	145	groove	slow	4	f
368	2	4	3	145	groove	slow	4	f
369	3	4	3	145	groove	slow	4	f
370	1	4	3	137	groove	slow	4	f
371	2	4	3	137	groove	slow	4	f
372	3	4	3	137	groove	slow	4	f
373	0	4	5	145	groove	slow	4	f
374	0	4	5	137	groove	slow	4	f
375	1	4	5	145	groove	slow	4	f
376	2	4	5	145	groove	slow	4	f
377	3	4	5	145	groove	slow	4	f
378	1	4	5	137	groove	slow	4	f
379	2	4	5	137	groove	slow	4	f
380	3	4	5	137	groove	slow	4	f
381	0	4	7	145	groove	slow	4	f
382	0	4	7	137	groove	slow	4	f
383	1	4	7	145	groove	slow	4	f
384	2	4	7	145	groove	slow	4	f
385	3	4	7	145	groove	slow	4	f
386	1	4	7	137	groove	slow	4	f
387	2	4	7	137	groove	slow	4	f
388	3	4	7	137	groove	slow	4	f
389	0	4	9	145	groove	slow	4	f
390	0	4	9	137	groove	slow	4	f
391	1	4	9	145	groove	slow	4	f
392	2	4	9	145	groove	slow	4	f
393	3	4	9	145	groove	slow	4	f
394	1	4	9	137	groove	slow	4	f
395	2	4	9	137	groove	slow	4	f
396	3	4	9	137	groove	slow	4	f
397	0	4	10	145	groove	slow	4	f
398	0	4	10	137	groove	slow	4	f
399	1	4	10	145	groove	slow	4	f
400	2	4	10	145	groove	slow	4	f
401	3	4	10	145	groove	slow	4	f
402	1	4	10	137	groove	slow	4	f
403	2	4	10	137	groove	slow	4	f
404	3	4	10	137	groove	slow	4	f
280	0	15	0	0	groove	slow	4	f
281	1	15	0	0	groove	slow	4	f
282	2	15	0	0	groove	slow	4	f
283	3	15	0	0	groove	slow	4	f
284	0	15	0	0	groove	pushed	4	f
285	1	15	0	0	groove	pushed	4	f
286	2	15	0	0	groove	pushed	4	f
287	3	15	0	0	groove	pushed	4	f
\.


--
-- Name: quarter_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('quarter_id_seq', 287, true);


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
97	0	38	80	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
99	2	38	80	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
101	2	41	80	127	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
102	0	41	80	127	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0,0,0,0.100000000000000006,0,0,0,0}
98	0	39	80	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
175	0	195	80	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.599999999999999978,0}
177	0	197	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
179	0	198	80	127	0.100000000000000006	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
181	0	199	80	127	0.299999999999999989	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
161	0	181	80	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011}
17	0	5	80	127	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
19	0	6	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
21	0	9	80	127	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
24	0	10	80	127	0.5	0	0	0	{0.349999999999999978,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
26	0	13	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
29	0	11	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
30	0	12	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
34	0	14	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
68	0	30	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
72	0	32	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
77	0	33	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
81	0	34	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
85	0	35	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
89	0	36	80	127	0.299999999999999989	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
125	2	185	80	127	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
110	0	170	80	127	0.400000000000000022	0	0	0	{0,0,0,0,0.699999999999999956,0,0,0.149999999999999994,0,0,0,0.149999999999999994,0}
112	0	172	80	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
114	0	176	80	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
116	0	177	80	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.5,0,0,0.25,0,0,0}
118	0	178	80	127	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.550000000000000044,0,0,0,0,0,0}
120	0	180	80	127	0.299999999999999989	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0,0}
122	0	184	80	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
124	0	185	80	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
166	2	183	80	127	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.25,0,0,0,0.25}
132	0	193	80	127	0.100000000000000006	0	0	0	{0,0.200000000000000011,0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0}
35	0	15	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
37	0	16	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
39	0	17	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
40	0	18	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
41	0	19	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
42	0	20	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
47	0	22	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
49	0	26	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
50	0	27	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
51	0	28	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
55	0	25	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
57	0	23	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
142	0	202	60	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
58	0	24	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
59	0	21	80	127	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
131	2	192	80	127	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.400000000000000022,0,0}
64	0	29	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
168	2	187	80	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
73	1	32	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
74	2	32	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
75	3	32	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
78	1	33	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
126	0	186	80	127	0.400000000000000022	0	0	0	{0,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0.0500000000000000028,0}
128	0	188	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
130	0	192	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
79	2	33	80	127	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
80	3	33	80	127	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
82	1	34	80	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
83	2	34	80	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
84	3	34	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
86	3	35	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
87	2	35	80	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
88	1	35	80	127	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
90	1	36	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
91	2	36	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
48	2	22	80	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
134	0	194	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.5,0,0}
136	0	196	80	127	0.800000000000000044	0	0	0	{0.25,0,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0,0,0.400000000000000022,0}
138	0	200	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
140	0	201	80	127	0.800000000000000044	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
144	0	163	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
146	0	165	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011,0,0,0.25,0}
148	0	166	80	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0.5,0,0,0,0,0,0,0.25,0}
150	0	167	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
153	0	173	80	127	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
155	0	174	80	127	0.800000000000000044	0	0	0	{0,0.0500000000000000028,0,0,0,0,0.450000000000000011,0,0,0,0.5,0,0}
157	0	175	80	127	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.349999999999999978,0,0,0,0.200000000000000011,0,0}
159	0	179	80	127	0.100000000000000006	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.650000000000000022,0,0,0,0.100000000000000006}
163	0	182	80	127	0.800000000000000044	0	0	0	{0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0,0,0,0.200000000000000011}
165	0	183	80	127	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
103	0	40	80	127	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0.100000000000000006,0,0,0,0,0,0,0}
104	0	164	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0.650000000000000022,0,0,0.100000000000000006,0,0,0,0.25,0}
106	0	168	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
108	0	169	80	127	0.800000000000000044	0	0	0	{0.25,0,0,0,0.25,0,0,0.25,0,0,0,0.25,0}
100	2	40	80	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
105	2	164	80	127	0.400000000000000022	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
107	2	168	80	127	0.699999999999999956	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
109	2	169	80	127	0.5	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
111	2	170	80	127	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
113	2	172	80	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.299999999999999989,0,0,0.200000000000000011,0,0,0}
115	2	176	80	127	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
143	2	202	60	127	0.100000000000000006	0	0	0	{0,0,0.200000000000000011,0,0,0,0.299999999999999989,0,0,0,0,0.5,0}
117	2	177	80	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
119	2	178	80	127	0.5	0	0	0	{0.25,0,0,0,0,0,0.5,0,0,0.25,0,0,0}
121	2	180	80	127	0.5	0	0	0	{0,0,0,0.25,0,0,0,0,0.25,0,0,0.5,0}
123	2	184	80	127	0.200000000000000011	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.0500000000000000028,0,0,0.149999999999999994,0}
127	2	186	80	127	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.5,0,0,0,0}
129	2	188	80	127	0.100000000000000006	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
133	2	193	80	127	0.5	0	0	0	{0.25,0,0,0,0,0.25,0,0,0,0,0.5,0,0}
135	2	194	80	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
137	2	196	80	127	0.299999999999999989	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
139	2	200	80	127	0.5	0	0	0	{0,0,0.100000000000000006,0,0,0,0.400000000000000022,0,0,0,0,0.5,0}
141	2	201	80	127	0.100000000000000006	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
145	2	163	80	127	0.100000000000000006	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
147	2	165	80	127	0.5	0	0	0	{0,0,0,0,0.299999999999999989,0,0,0,0.200000000000000011,0,0,0.5,0}
149	2	166	80	127	0.299999999999999989	0	0	0	{0.25,0,0,0,0.25,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
151	2	167	80	127	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
152	2	171	80	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006,0,0}
154	2	173	80	127	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
156	2	174	80	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
158	2	175	80	127	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
160	2	179	80	127	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
162	2	181	80	127	0.5	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.25,0,0,0,0}
164	2	182	80	127	0.200000000000000011	0	0	0	{0,0,0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006}
170	2	189	80	127	0.5	0	0	0	{0.200000000000000011,0,0,0,0,0.400000000000000022,0,0,0,0,0.400000000000000022,0,0}
11	2	7	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
15	2	8	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.25,0,0.25,0,0}
18	2	5	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
20	2	6	80	127	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
22	1	9	80	127	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
23	2	9	80	127	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
25	2	10	80	127	0.5	0	0	0	{0,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.200000000000000011,0,0,0.299999999999999989,0}
27	2	13	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
28	2	11	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
31	2	12	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
32	1	12	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
33	3	12	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
36	2	15	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
38	2	16	80	127	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
43	2	17	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
44	2	18	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
45	2	19	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
231	0	213	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
232	0	214	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
236	0	216	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
270	0	232	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
274	0	234	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
279	0	235	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
283	0	236	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
287	0	237	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
291	0	238	40	90	0.299999999999999989	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
327	2	387	40	90	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
312	0	372	40	90	0.400000000000000022	0	0	0	{0,0,0,0,0.699999999999999956,0,0,0.149999999999999994,0,0,0,0.149999999999999994,0}
314	0	374	40	90	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
316	0	378	40	90	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
46	2	20	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
52	2	26	80	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
53	2	27	80	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
54	2	28	80	127	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
60	2	25	80	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
61	2	23	80	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
62	2	24	80	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
63	2	21	80	127	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
65	1	29	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
66	2	29	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
67	3	29	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
69	1	30	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
70	2	30	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
71	3	30	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
167	0	187	80	127	0.800000000000000044	0	0	0	{0,0.200000000000000011,0.200000000000000011,0,0,0.25,0,0,0,0,0.75,0,0}
169	0	189	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
171	0	190	80	127	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
10	0	7	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
12	0	8	80	127	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
178	2	197	80	127	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
180	2	198	80	127	0.5	0	0	0	{0,0,0,0.25,0,0,0.25,0,0,0,0,0.5,0}
182	2	199	80	127	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
93	0	37	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
173	0	191	80	127	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
318	0	379	40	90	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.5,0,0,0.25,0,0,0}
320	0	380	40	90	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.550000000000000044,0,0,0,0,0,0}
92	3	36	80	127	0.200000000000000011	0	0	0	{0.299999999999999989,0.200000000000000011,0,0,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
94	1	37	80	127	0.100000000000000006	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
95	2	37	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
96	3	37	80	127	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0,0,0.299999999999999989,0,0}
172	2	190	80	127	0.5	0	0	0	{0.25,0,0.200000000000000011,0,0,0.25,0,0,0,0,0.299999999999999989,0,0}
299	0	240	40	90	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
301	2	240	40	90	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
303	2	243	40	90	0.800000000000000044	0	0	0	{0,0.800000000000000044,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
304	0	243	40	90	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0,0,0,0.100000000000000006,0,0,0,0}
300	0	241	40	90	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
377	0	397	40	90	0.800000000000000044	0	0	0	{0.25,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.599999999999999978,0}
379	0	399	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
174	2	191	80	127	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
176	2	195	80	127	0.299999999999999989	0	0	0	{0,0,0.25,0.25,0,0,0.25,0,0,0,0,0.25,0}
381	0	400	40	90	0.100000000000000006	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
383	0	401	40	90	0.299999999999999989	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
363	0	383	40	90	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011}
219	0	207	40	90	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
221	0	208	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
223	0	211	40	90	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
226	0	212	40	90	0.5	0	0	0	{0.349999999999999978,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
228	0	215	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
322	0	382	40	90	0.299999999999999989	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0,0}
324	0	386	40	90	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
326	0	387	40	90	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
368	2	385	40	90	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.25,0,0,0,0.25}
334	0	395	40	90	0.100000000000000006	0	0	0	{0,0.200000000000000011,0,0,0,0.25,0,0,0,0,0.550000000000000044,0,0}
237	0	217	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
239	0	218	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
241	0	219	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
242	0	220	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
243	0	221	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
244	0	222	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
249	0	224	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
251	0	228	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
252	0	229	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
253	0	230	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0.25,0,0,0,0.25,0,0,0}
257	0	227	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
259	0	225	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
260	0	226	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
261	0	223	40	90	0.699999999999999956	0	0	0	{0,0.5,0,0,0,0,0.25,0,0,0.25,0,0,0}
333	2	394	40	90	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.400000000000000022,0,0}
266	0	231	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
370	2	389	40	90	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
275	1	234	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
276	2	234	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
277	3	234	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
280	1	235	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
328	0	388	40	90	0.400000000000000022	0	0	0	{0,0,0,0.200000000000000011,0,0,0,0,0.75,0,0,0.0500000000000000028,0}
330	0	390	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
332	0	394	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
281	2	235	40	90	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
282	3	235	40	90	0.5	0	0	0	{0.200000000000000011,0.100000000000000006,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
284	1	236	40	90	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
285	2	236	40	90	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
286	3	236	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
288	3	237	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
289	2	237	40	90	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
290	1	237	40	90	0.200000000000000011	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
292	1	238	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
293	2	238	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
250	2	224	40	90	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
336	0	396	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0,0,0.200000000000000011,0,0,0,0,0.5,0,0}
338	0	398	40	90	0.800000000000000044	0	0	0	{0.25,0,0.200000000000000011,0,0,0,0.149999999999999994,0,0,0,0,0.400000000000000022,0}
340	0	402	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0,0,0.25,0,0,0,0,0.75,0}
342	0	403	40	90	0.800000000000000044	0	0	0	{0.200000000000000011,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0}
344	0	404	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0,0.149999999999999994,0,0,0,0,0.75,0}
346	0	365	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
348	0	367	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0.550000000000000044,0,0,0,0.200000000000000011,0,0,0.25,0}
350	0	368	40	90	0.800000000000000044	0	0	0	{0.25,0,0,0,0.5,0,0,0,0,0,0,0.25,0}
352	0	369	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
355	0	375	40	90	0.800000000000000044	0	0	0	{0,0.25,0,0,0,0,0.75,0,0,0,0,0,0}
357	0	376	40	90	0.800000000000000044	0	0	0	{0,0.0500000000000000028,0,0,0,0,0.450000000000000011,0,0,0,0.5,0,0}
359	0	377	40	90	0.800000000000000044	0	0	0	{0.200000000000000011,0.25,0,0,0,0,0.349999999999999978,0,0,0,0.200000000000000011,0,0}
361	0	381	40	90	0.100000000000000006	0	0	0	{0.0500000000000000028,0,0,0.200000000000000011,0,0,0,0,0.650000000000000022,0,0,0,0.100000000000000006}
365	0	384	40	90	0.800000000000000044	0	0	0	{0,0,0,0.149999999999999994,0,0,0,0,0.650000000000000022,0,0,0,0.200000000000000011}
367	0	385	40	90	0.800000000000000044	0	0	0	{0,0,0,0.25,0,0,0,0,0.75,0,0,0,0}
305	0	242	40	90	0.800000000000000044	0	0	0	{0.800000000000000044,0.100000000000000006,0,0,0,0.100000000000000006,0,0,0,0,0,0,0}
306	0	366	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0.650000000000000022,0,0,0.100000000000000006,0,0,0,0.25,0}
308	0	370	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0.75,0,0,0,0,0,0,0.25,0}
310	0	371	40	90	0.800000000000000044	0	0	0	{0.25,0,0,0,0.25,0,0,0.25,0,0,0,0.25,0}
302	2	242	40	90	0.800000000000000044	0	0	0	{0,0.200000000000000011,0,0,0,0,0.800000000000000044,0,0,0,0,0,0}
307	2	366	40	90	0.400000000000000022	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
309	2	370	40	90	0.699999999999999956	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
311	2	371	40	90	0.5	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
313	2	372	40	90	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0,0,0,0.5,0}
315	2	374	40	90	0.5	0	0	0	{0,0.5,0,0,0,0,0.299999999999999989,0,0,0.200000000000000011,0,0,0}
317	2	378	40	90	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
319	2	379	40	90	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
321	2	380	40	90	0.5	0	0	0	{0.25,0,0,0,0,0,0.5,0,0,0.25,0,0,0}
323	2	382	40	90	0.5	0	0	0	{0,0,0,0.25,0,0,0,0,0.25,0,0,0.5,0}
325	2	386	40	90	0.200000000000000011	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.0500000000000000028,0,0,0.149999999999999994,0}
329	2	388	40	90	0.5	0	0	0	{0.25,0,0,0.25,0,0,0,0,0.5,0,0,0,0}
331	2	390	40	90	0.100000000000000006	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
335	2	395	40	90	0.5	0	0	0	{0.25,0,0,0,0,0.25,0,0,0,0,0.5,0,0}
337	2	396	40	90	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
339	2	398	40	90	0.299999999999999989	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
341	2	402	40	90	0.5	0	0	0	{0,0,0.100000000000000006,0,0,0,0.400000000000000022,0,0,0,0,0.5,0}
343	2	403	40	90	0.100000000000000006	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
345	2	404	40	90	0.100000000000000006	0	0	0	{0,0,0.200000000000000011,0,0,0,0.299999999999999989,0,0,0,0,0.5,0}
347	2	365	40	90	0.100000000000000006	0	0	0	{0.25,0,0,0,0.25,0,0,0,0,0,0,0.5,0}
349	2	367	40	90	0.5	0	0	0	{0,0,0,0,0.299999999999999989,0,0,0,0.200000000000000011,0,0,0.5,0}
351	2	368	40	90	0.299999999999999989	0	0	0	{0.25,0,0,0,0.25,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
353	2	369	40	90	0.5	0	0	0	{0,0,0,0,0.5,0,0,0,0.100000000000000006,0,0,0.400000000000000022,0}
354	2	373	40	90	0.5	0	0	0	{0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006,0,0}
356	2	375	40	90	0.5	0	0	0	{0.200000000000000011,0.299999999999999989,0,0,0,0,0.5,0,0,0,0,0,0}
358	2	376	40	90	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
360	2	377	40	90	0.5	0	0	0	{0,0.5,0,0,0,0,0.5,0,0,0,0,0,0}
362	2	381	40	90	0.5	0	0	0	{0,0,0,0.5,0,0,0,0,0.5,0,0,0,0}
364	2	383	40	90	0.5	0	0	0	{0.25,0,0,0.5,0,0,0,0,0.25,0,0,0,0}
366	2	384	40	90	0.200000000000000011	0	0	0	{0,0,0,0.5,0,0,0,0,0.400000000000000022,0,0,0,0.100000000000000006}
372	2	391	40	90	0.5	0	0	0	{0.200000000000000011,0,0,0,0,0.400000000000000022,0,0,0,0,0.400000000000000022,0,0}
213	2	209	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
217	2	210	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.25,0,0.25,0,0}
220	2	207	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0,0,0,0.5,0,0,0,0}
222	2	208	40	90	0.5	0	0	0	{0.25,0.5,0,0,0,0,0,0,0.25,0,0,0,0}
224	1	211	40	90	0.5	0	0	0	{0.25,0.25,0,0.25,0,0,0,0,0.25,0,0,0,0}
225	2	211	40	90	0.5	0	0	0	{0.200000000000000011,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.149999999999999994,0,0,0.149999999999999994,0}
227	2	212	40	90	0.5	0	0	0	{0,0.200000000000000011,0,0.149999999999999994,0,0.149999999999999994,0,0,0.200000000000000011,0,0,0.299999999999999989,0}
229	2	215	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
230	2	213	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
233	2	214	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
234	1	214	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
235	3	214	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
238	2	217	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
240	2	218	40	90	0.400000000000000022	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
245	2	219	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
246	2	220	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
247	2	221	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
248	2	222	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
254	2	228	40	90	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
255	2	229	40	90	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
256	2	230	40	90	0.5	0	0	0	{0,0.25,0,0,0,0.25,0,0,0,0.5,0,0,0}
262	2	227	40	90	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
263	2	225	40	90	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
264	2	226	40	90	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
265	2	223	40	90	0.5	0	0	0	{0,0.25,0,0,0,0,0.25,0,0,0.5,0,0,0}
267	1	231	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
268	2	231	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
269	3	231	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
271	1	232	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
272	2	232	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
273	3	232	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.100000000000000006,0,0,0.100000000000000006,0,0.200000000000000011,0,0}
294	3	238	40	90	0.200000000000000011	0	0	0	{0.299999999999999989,0.200000000000000011,0,0,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
296	1	239	40	90	0.100000000000000006	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
297	2	239	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
298	3	239	40	90	0.5	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.200000000000000011,0.100000000000000006,0.100000000000000006,0,0,0,0,0.299999999999999989,0,0}
374	2	392	40	90	0.5	0	0	0	{0.25,0,0.200000000000000011,0,0,0.25,0,0,0,0,0.299999999999999989,0,0}
376	2	393	40	90	0.5	0	0	0	{0,0,0,0,0,0.5,0,0,0,0,0.5,0,0}
378	2	397	40	90	0.299999999999999989	0	0	0	{0,0,0.25,0.25,0,0,0.25,0,0,0,0,0.25,0}
369	0	389	40	90	0.800000000000000044	0	0	0	{0,0.200000000000000011,0.200000000000000011,0,0,0.25,0,0,0,0,0.75,0,0}
371	0	391	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
373	0	392	40	90	0.800000000000000044	0	0	0	{0,0,0,0,0,0.25,0,0,0,0,0.75,0,0}
212	0	209	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
214	0	210	40	90	0.5	0	0	0	{0.25,0.25,0,0,0,0.25,0,0,0.25,0,0,0,0}
380	2	399	40	90	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
382	2	400	40	90	0.5	0	0	0	{0,0,0,0.25,0,0,0.25,0,0,0,0,0.5,0}
384	2	401	40	90	0.5	0	0	0	{0,0,0,0,0,0,0.5,0,0,0,0,0.5,0}
295	0	239	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0.200000000000000011,0,0.100000000000000006,0.200000000000000011,0.200000000000000011,0,0,0,0,0.200000000000000011,0,0}
375	0	393	40	90	0.800000000000000044	0	0	0	{0.100000000000000006,0,0,0,0,0.200000000000000011,0,0,0,0,0.699999999999999956,0,0}
183	0	280	40	90	1	0	0	0	{0,0,0,1,0,0,0,0,0,0,0,1,0}
184	2	280	40	90	1	0	0	0	{0,0,0,0.800000000000000044,0,0,0.200000000000000011,0,0,0,0,0,0}
185	0	281	40	90	1	0	0	0	{0,0,0,0,0,0,0,0,0,0,1,0,0}
186	1	281	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
187	2	281	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
188	3	281	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
195	0	284	80	120	1	0	0	0	{0,0,1,0,0,0,0,0,0,0,0,1,0}
189	0	282	40	90	1	0	0	0	{0,0,0,0,0,0,0,0,0,0,0,1,0}
190	2	282	40	90	1	0	0	0	{0,0,0,0.800000000000000044,0,0,0.200000000000000011,0,0,0,0,0,0}
191	0	283	40	90	1	0	0	0	{0,0,0,0,0,0,0,0,0,0,1,0,0}
192	1	283	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
193	2	283	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
194	3	283	40	90	1	0	0	0	{0,0,0,0.25,0,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.100000000000000006,0.149999999999999994,0,0}
198	1	285	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
196	2	284	80	120	1	0	0	0	{0.800000000000000044,0,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
197	0	285	80	120	1	0	0	0	{0,0,0,0,0,1,0,0,0,0,0,0,0}
199	2	285	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
200	3	285	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
201	0	286	80	120	1	0	0	0	{0,0,0,0,0,0,0,0,0,0,0,1,0}
202	2	286	80	120	1	0	0	0	{0.800000000000000044,0,0,0,0,0,0,0,0.200000000000000011,0,0,0,0}
203	0	287	80	120	1	0	0	0	{0,0,0,0,0,1,0,0,0,0,0,0,0}
204	1	287	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
205	2	287	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
206	3	287	80	120	1	0	0	0	{0.25,0,0,0,0,0.149999999999999994,0,0.100000000000000006,0.100000000000000006,0.100000000000000006,0,0,0}
\.


--
-- Name: semiquaver_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('semiquaver_id_seq', 206, true);


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
-- Name: prioargs_instrument_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY prioargs_instrument
    ADD CONSTRAINT prioargs_instrument_pkey PRIMARY KEY (id);


--
-- Name: prioargs_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY prioargs
    ADD CONSTRAINT prioargs_pkey PRIMARY KEY (id);


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
-- Name: instr_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY prioargs_instrument
    ADD CONSTRAINT instr_fk FOREIGN KEY (instrument_id) REFERENCES instrument(id) MATCH FULL;


--
-- Name: prio_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY pattern
    ADD CONSTRAINT prio_fk FOREIGN KEY (prioargs_id) REFERENCES prioargs(id) MATCH FULL;


--
-- Name: prio_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY prioargs_instrument
    ADD CONSTRAINT prio_fk FOREIGN KEY (prioargs_id) REFERENCES prioargs(id) MATCH FULL;


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
-- Name: prioargs; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE prioargs FROM PUBLIC;
REVOKE ALL ON TABLE prioargs FROM postgres;
GRANT ALL ON TABLE prioargs TO postgres;
GRANT SELECT ON TABLE prioargs TO read_only;


--
-- Name: prioargs_instrument; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE prioargs_instrument FROM PUBLIC;
REVOKE ALL ON TABLE prioargs_instrument FROM postgres;
GRANT ALL ON TABLE prioargs_instrument TO postgres;
GRANT SELECT ON TABLE prioargs_instrument TO read_only;


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

SET default_transaction_read_only = off;

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

SET default_transaction_read_only = off;

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

