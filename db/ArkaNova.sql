--
-- PostgreSQL database dump
--

-- Dumped from database version 17.0 (Debian 17.0-1.pgdg120+1)
-- Dumped by pg_dump version 17.0
--
-- Name: set_recorded_at(); Type: FUNCTION; Schema: public; Owner: kirixo
--

CREATE FUNCTION public.set_recorded_at() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    NEW.recorded_at := CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.set_recorded_at() OWNER TO kirixo;

--
-- Name: set_timestamps(); Type: FUNCTION; Schema: public; Owner: kirixo
--

CREATE FUNCTION public.set_timestamps() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF TG_OP = 'INSERT' THEN
        NEW.created_at := CURRENT_TIMESTAMP;
    ELSIF TG_OP = 'UPDATE' THEN
        NEW.updated_at := CURRENT_TIMESTAMP;
    END IF;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.set_timestamps() OWNER TO kirixo;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: measurement; Type: TABLE; Schema: public; Owner: kirixo
--

CREATE TABLE public.measurement (
    id integer NOT NULL,
    data bytea NOT NULL,
    recorded_at timestamp without time zone NOT NULL,
    sensor_id integer NOT NULL
);


ALTER TABLE public.measurement OWNER TO kirixo;

--
-- Name: measurement_id_seq; Type: SEQUENCE; Schema: public; Owner: kirixo
--

CREATE SEQUENCE public.measurement_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.measurement_id_seq OWNER TO kirixo;

--
-- Name: measurement_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: kirixo
--

ALTER SEQUENCE public.measurement_id_seq OWNED BY public.measurement.id;


--
-- Name: sensor; Type: TABLE; Schema: public; Owner: kirixo
--

CREATE TABLE public.sensor (
    id integer NOT NULL,
    created_at timestamp without time zone NOT NULL,
    updated_at timestamp without time zone,
    sensor_type_id integer NOT NULL,
    solar_panel_id integer NOT NULL
);


ALTER TABLE public.sensor OWNER TO kirixo;

--
-- Name: sensor_id_seq; Type: SEQUENCE; Schema: public; Owner: kirixo
--

CREATE SEQUENCE public.sensor_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sensor_id_seq OWNER TO kirixo;

--
-- Name: sensor_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: kirixo
--

ALTER SEQUENCE public.sensor_id_seq OWNED BY public.sensor.id;


--
-- Name: sensor_type; Type: TABLE; Schema: public; Owner: kirixo
--

CREATE TABLE public.sensor_type (
    id integer NOT NULL,
    name character varying(255) NOT NULL
);


ALTER TABLE public.sensor_type OWNER TO kirixo;

--
-- Name: sensor_type_id_seq; Type: SEQUENCE; Schema: public; Owner: kirixo
--

CREATE SEQUENCE public.sensor_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sensor_type_id_seq OWNER TO kirixo;

--
-- Name: sensor_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: kirixo
--

ALTER SEQUENCE public.sensor_type_id_seq OWNED BY public.sensor_type.id;


--
-- Name: solar_panel; Type: TABLE; Schema: public; Owner: kirixo
--

CREATE TABLE public.solar_panel (
    id integer NOT NULL,
    location character varying(255),
    created_at timestamp without time zone NOT NULL,
    updated_at timestamp without time zone,
    user_id integer NOT NULL
);


ALTER TABLE public.solar_panel OWNER TO kirixo;

--
-- Name: solar_panel_id_seq; Type: SEQUENCE; Schema: public; Owner: kirixo
--

CREATE SEQUENCE public.solar_panel_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.solar_panel_id_seq OWNER TO kirixo;

--
-- Name: solar_panel_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: kirixo
--

ALTER SEQUENCE public.solar_panel_id_seq OWNED BY public.solar_panel.id;


--
-- Name: user; Type: TABLE; Schema: public; Owner: kirixo
--

CREATE TABLE public."user" (
    id integer NOT NULL,
    email character varying(255) NOT NULL,
    password character varying(255) NOT NULL,
    created_at timestamp without time zone NOT NULL,
    updated_at timestamp without time zone
);


ALTER TABLE public."user" OWNER TO kirixo;

--
-- Name: user_id_seq; Type: SEQUENCE; Schema: public; Owner: kirixo
--

CREATE SEQUENCE public.user_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.user_id_seq OWNER TO kirixo;

--
-- Name: user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: kirixo
--

ALTER SEQUENCE public.user_id_seq OWNED BY public."user".id;


--
-- Name: measurement id; Type: DEFAULT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.measurement ALTER COLUMN id SET DEFAULT nextval('public.measurement_id_seq'::regclass);


--
-- Name: sensor id; Type: DEFAULT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor ALTER COLUMN id SET DEFAULT nextval('public.sensor_id_seq'::regclass);


--
-- Name: sensor_type id; Type: DEFAULT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor_type ALTER COLUMN id SET DEFAULT nextval('public.sensor_type_id_seq'::regclass);


--
-- Name: solar_panel id; Type: DEFAULT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.solar_panel ALTER COLUMN id SET DEFAULT nextval('public.solar_panel_id_seq'::regclass);


--
-- Name: user id; Type: DEFAULT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public."user" ALTER COLUMN id SET DEFAULT nextval('public.user_id_seq'::regclass);


--
-- Data for Name: sensor; Type: TABLE DATA; Schema: public; Owner: kirixo
--

COPY public.sensor (id, created_at, updated_at, sensor_type_id, solar_panel_id) FROM stdin;
3	2024-12-26 16:53:20.483424	\N	1	5
\.


--
-- Data for Name: sensor_type; Type: TABLE DATA; Schema: public; Owner: kirixo
--

COPY public.sensor_type (id, name) FROM stdin;
1	temperature
\.


--
-- Data for Name: solar_panel; Type: TABLE DATA; Schema: public; Owner: kirixo
--

COPY public.solar_panel (id, location, created_at, updated_at, user_id) FROM stdin;
5	Somewhere	2024-12-26 16:53:00.533029	\N	6
\.


--
-- Data for Name: user; Type: TABLE DATA; Schema: public; Owner: kirixo
--

COPY public."user" (id, email, password, created_at, updated_at) FROM stdin;
3	user@example.com	81dc9bdb52d04dc20036dbd8313ed055	2024-12-26 08:26:25.36655	\N
6	newuser@example.com	new81dc9bdb52d04dc20036dbd8313ed055	2024-12-26 16:46:08.792649	2024-12-26 16:48:51.288118
\.


--
-- Name: measurement_id_seq; Type: SEQUENCE SET; Schema: public; Owner: kirixo
--

SELECT pg_catalog.setval('public.measurement_id_seq', 590, true);


--
-- Name: sensor_id_seq; Type: SEQUENCE SET; Schema: public; Owner: kirixo
--

SELECT pg_catalog.setval('public.sensor_id_seq', 3, true);


--
-- Name: sensor_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: kirixo
--

SELECT pg_catalog.setval('public.sensor_type_id_seq', 1, false);


--
-- Name: solar_panel_id_seq; Type: SEQUENCE SET; Schema: public; Owner: kirixo
--

SELECT pg_catalog.setval('public.solar_panel_id_seq', 5, true);


--
-- Name: user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: kirixo
--

SELECT pg_catalog.setval('public.user_id_seq', 6, true);


--
-- Name: measurement measurement_pk; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.measurement
    ADD CONSTRAINT measurement_pk PRIMARY KEY (id);


--
-- Name: sensor sensor_pk; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor
    ADD CONSTRAINT sensor_pk PRIMARY KEY (id);


--
-- Name: sensor_type sensor_type_pk; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor_type
    ADD CONSTRAINT sensor_type_pk PRIMARY KEY (id);


--
-- Name: user set; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public."user"
    ADD CONSTRAINT set UNIQUE (email);


--
-- Name: solar_panel solar_panel_pk; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.solar_panel
    ADD CONSTRAINT solar_panel_pk PRIMARY KEY (id);


--
-- Name: user user_pk; Type: CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public."user"
    ADD CONSTRAINT user_pk PRIMARY KEY (id);


--
-- Name: measurement trg_measurement_insert; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_measurement_insert BEFORE INSERT ON public.measurement FOR EACH ROW EXECUTE FUNCTION public.set_recorded_at();


--
-- Name: sensor trg_sensor_insert; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_sensor_insert BEFORE INSERT ON public.sensor FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: sensor trg_sensor_update; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_sensor_update BEFORE UPDATE ON public.sensor FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: solar_panel trg_solar_panel_insert; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_solar_panel_insert BEFORE INSERT ON public.solar_panel FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: solar_panel trg_solar_panel_update; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_solar_panel_update BEFORE UPDATE ON public.solar_panel FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: user trg_user_insert; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_user_insert BEFORE INSERT ON public."user" FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: user trg_user_update; Type: TRIGGER; Schema: public; Owner: kirixo
--

CREATE TRIGGER trg_user_update BEFORE UPDATE ON public."user" FOR EACH ROW EXECUTE FUNCTION public.set_timestamps();


--
-- Name: measurement measurement_sensor; Type: FK CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.measurement
    ADD CONSTRAINT measurement_sensor FOREIGN KEY (sensor_id) REFERENCES public.sensor(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: sensor sensor_sensor_type; Type: FK CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor
    ADD CONSTRAINT sensor_sensor_type FOREIGN KEY (sensor_type_id) REFERENCES public.sensor_type(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: sensor sensor_solar_panel; Type: FK CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.sensor
    ADD CONSTRAINT sensor_solar_panel FOREIGN KEY (solar_panel_id) REFERENCES public.solar_panel(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: solar_panel solar_panel_user; Type: FK CONSTRAINT; Schema: public; Owner: kirixo
--

ALTER TABLE ONLY public.solar_panel
    ADD CONSTRAINT solar_panel_user FOREIGN KEY (user_id) REFERENCES public."user"(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

